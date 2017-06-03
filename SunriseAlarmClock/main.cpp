#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "sevensegment.h"
#include "i2c.h"
#include "RTC.h"
#include "lights.h"
#include "buttons.h"

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

bool timeNeedsUpdate = true;

// An array containing display pixels
byte segments[5];
int main(void){
    I2C::enable();
    SevenSegment::enable();

    // Enable 1Hz signal from RTC and enable interrupt routine to set the time
    RTC::writeRegister(0xe, 0); //enable 1Hz signal from RTC

    EICRA = 0b1100; // enable interrupt on rising edge of 1Hz signal
    EIMSK = 0b10;
    
    Lights::enable();
    
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
    ledOff();
    
    unsigned long x = 4095;
    for(;;) {
        Time t;
        Lights::setCombined(x);
        x+=10;
        
        
        if (timeNeedsUpdate) {
            time = RTC::getTime();
            timeNeedsUpdate = false;
        }
        
        t = time;
        
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
        
        for(byte i=0; i<5; i++) {
            segments[i] = _segments[i];
        }
    }
}

// ISR for RTC 1Hz
ISR(INT1_vect) {
    timeNeedsUpdate = true;
}

// ISR for buttons
ISR(TIMER2_COMPA_vect) {
    if (Buttons::readButtons() == NONE) {
        ledOn();
    } else {
        ledOff();
    }
}

// ISR for display refresh timer
ISR(TIMER0_COMPA_vect) {
    for(byte i=0; i<4; i++) {
        SevenSegment::digits[i] = segments[i];
    }
    SevenSegment::points = segments[4];
    
    SevenSegment::show();
}