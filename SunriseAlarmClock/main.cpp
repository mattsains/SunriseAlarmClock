#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "sevensegment.h"
#include "i2c.h"
#include "RTC.h"

enum ui_state {
    TIME,
    SET_Y,
    SET_MO,
    SET_D,
    SET_H,
    SET_M,
    SET_ALRM_H,
    SET_ALRM_M,
    SET_ALRM_ON
};

void ledOn() {
    DDRB |= (1<<PB0);
    PORTB |= (1<<PB0);
}

void ledOff() {
    DDRB |= (1<<PB0);
    PORTB &= ~(1<<PB0);
}

void blinkFast() {
    ledOn();
    _delay_ms(150);
    ledOff();
    _delay_ms(150);
}

void blinkSlow() {
    ledOn();
    _delay_ms(500);
    ledOff();
    _delay_ms(500);
}

void blinkFastForever() {
    for(;;) blinkFast();
}

void blinkSlowForever() {
    for(;;) blinkSlow();
}

void error() {
    ledOff();
    ledOn();
    ledOff();
    ledOn();
    ledOff();
}

// Latest time fetched from the RTC
Time time;

// An array containing display pixels
byte segments[5];

// UI state machine
ui_state current_state = TIME;


// Used to set the time
int y_offset;
byte mo_offset, d_offset, h_offset, m_offset;

int main(void){
    I2C::enable();
    SevenSegment::enable();

    // Enable 1Hz signal from RTC and enable interrupt routine to set the time
    RTC::writeRegister(0xe, 0); //enable 1Hz signal from RTC

    EICRA = 0b1100; // enable interrupt on rising edge of 1Hz signal
    EIMSK = 0b10;
    
    // Set up PWM timers for LEDs
    TCCR1A = (1<<COM1A1) // Turn on normal PWM output
    |(1<<COM1B1)
    |(1<<WGM11); // Fast PWM with TOP=ICR1
    
    TCCR1B = (1<<WGM12)
    |(1<<WGM13)
    |(1<<CS10); // No prescaling
    
    ICR1H = 255; //Set top to max
    ICR1L = 255;
    
    DDRB |= (1<<PB1)| (1<<PB2);
    
    // Set up timer for buttons
    TCCR2A = (1<<WGM21); // CTC mode
    TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);
    OCR2A = 255; // As long as possible (32ms)
    TIMSK2 = (1<<OCIE2A); // Enable interrupt for buttons
    
    // Set up timer for display update
    TCCR0A = (1<<WGM01); // CTC mode
    TCCR0B = (1<<CS02); // No prescaling
    OCR0A = 210; // Just faster than it takes to update display
    
    TIMSK0 = (1<<OCIE0A); // Enable interrupt on timer 0A
    
    sei();
    
    
    byte _segments[5];
    
    for(;;) {
        Time t;
        
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            t = time;
        }
        switch(current_state) {
            case TIME:
            _segments[0] = (t.hour/10==0)?0:SevenSegment::encodeDigit(t.hour/10);
            _segments[1] = SevenSegment::encodeDigit(t.hour%10);
            _segments[2] = SevenSegment::encodeDigit(t.minute/10);
            _segments[3] = SevenSegment::encodeDigit(t.minute%10);
            if (t.second % 2 == 0) {
                _segments[4] = 1<<2 | 1<<3;
            }
            else {
                _segments[4] = 0;
            }
            break;
            
            case SET_Y:
            t.year = (t.year + y_offset)%10000;
            _segments[0] = SevenSegment::encodeDigit(t.year/1000);
            _segments[1] = SevenSegment::encodeDigit((t.year%1000)/100);
            _segments[2] = SevenSegment::encodeDigit((t.year%100)/10);
            _segments[3] = SevenSegment::encodeDigit(t.year%10);
            _segments[4] = 1<<2;
            break;
            
            case SET_MO:
            t.month = (t.month + mo_offset)%12;
            _segments[0] = (t.month/10==0)?0:SevenSegment::encodeDigit(t.month/10);
            _segments[1] = SevenSegment::encodeDigit(t.month%10);
            _segments[2] = SevenSegment::encodeDigit(t.date/10);
            _segments[3] = SevenSegment::encodeDigit(t.date%10);
            _segments[4] = 1<<2;
            break;
            
            case SET_D:
             t.month = (t.month + mo_offset)%12;
             t.date = (t.date + d_offset)%31;
             _segments[0] = (t.month/10==0)?0:SevenSegment::encodeDigit(t.month/10);
             _segments[1] = SevenSegment::encodeDigit(t.month%10);
             _segments[2] = SevenSegment::encodeDigit(t.date/10);
             _segments[3] = SevenSegment::encodeDigit(t.date%10);
             _segments[4] = 1<<3;
             break;
             
            default:
            ;
        }
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            for(byte i=0; i<5; i++) {
                segments[i] = _segments[i];
            }
        }
    }
}

// ISR for RTC 1Hz
ISR(INT1_vect) {
    time = RTC::getTime();
}

bool previousButtonValues[3];
#define HOLD_WAIT 10
byte hold_countdown[3] ={HOLD_WAIT,HOLD_WAIT,HOLD_WAIT};

// ISR for buttons
ISR(TIMER2_COMPA_vect) {
    bool values[3];
    bool pressed[3];
    for(byte i=0; i<3; i++) {
        values[i] = !(BTN_PIN&(1<<(BTN_0_PIN+i)));
        pressed[i] = values[i] && !previousButtonValues[i];
        if (!values[i]) {
            hold_countdown[i] = HOLD_WAIT;
            } else if(hold_countdown[i]>0) {
            hold_countdown[i]--;
        }
    }
    
    switch(current_state) {
        case TIME:
        if (pressed[0]) {
            current_state = SET_Y;
        }
        break;
        case SET_Y:
        if (pressed[0]) {
            current_state = TIME;
        }
        else if (pressed[1]) {
            y_offset++;
        }
        else if (hold_countdown[1]==0) {
            y_offset+=10;
            hold_countdown[1]=HOLD_WAIT;
        }
        else if (pressed[2]) {
            current_state = SET_MO;
        }
        break;
        case SET_MO:
        if (pressed[0]) {
            current_state = TIME;
        }
        else if (pressed[1]) {
            mo_offset++;
        }
        else if (pressed[2]) {
            current_state = SET_D;
        }
        default:;
    }
    
    for(byte i=0; i<3; i++)
    previousButtonValues[i] = values[i];
}

// ISR for display refresh timer
ISR(TIMER0_COMPA_vect) {
    for(byte i=0; i<4; i++) {
        SevenSegment::digits[i] = segments[i];
    }
    SevenSegment::points = segments[4];
    
    SevenSegment::show();
}