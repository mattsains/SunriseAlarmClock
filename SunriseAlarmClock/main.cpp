#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

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

// Pointer to the latest time fetched from the RTC
Time* volatile time;
// Swapping variables for setting time pointer.
Time t1, t2;

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
    
    sei(); // Enable interrupts so we can get the time
    
    while (time == 0) ; // Wait for time sync before enabling display
    
    TIMSK0 = (1<<OCIE0A); // Enable interrupt on timer 0A
    
    
    for(;;);
    ledOff();
}

// ISR for RTC 1Hz
ISR(INT1_vect) {
    Time t = RTC::getTime();
    if (time == &t1) {
        t2 = t;
        time = &t2;
    }
    else {
        t1 = t;
        time = &t1;
    }
}

bool previousButtonValues[3];

// ISR for buttons
ISR(TIMER2_COMPA_vect) {
    bool values[3];
    bool pressed[3];
    for(byte i=0; i<3; i++) {
        values[i] = !(BTN_PIN&(1<<(BTN_0_PIN+i)));
        pressed[i] = values[i] && !previousButtonValues[i];
    }
    
    if (pressed[0]) {
        PORTB ^= (1<<PB0);
    }
    
    for(byte i=0; i<3; i++)
    previousButtonValues[i] = values[i];
}

// ISR for display refresh timer
ISR(TIMER0_COMPA_vect) {
    Time t = *time;
    SevenSegment::digits[0] = SevenSegment::encodeDigit(t.hour/10);
    SevenSegment::digits[1] = SevenSegment::encodeDigit(t.hour%10);
    SevenSegment::digits[2] = SevenSegment::encodeDigit(t.minute/10);
    SevenSegment::digits[3] = SevenSegment::encodeDigit(t.minute%10);
    if (t.second % 2 == 0) {
        SevenSegment::points = 1<<2 | 1<<3;
    }
    else {
        SevenSegment::points = 0;
    }
    
    SevenSegment::show();
}