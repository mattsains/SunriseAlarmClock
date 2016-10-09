#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "sevensegment.h"
#include "i2c.h"
#include "RTC.h"

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
    sei();

    while (time == 0) ; //wait for sync

    for(;;){
        Time t = *time;
        SevenSegment::digits[0] = SevenSegment::encodeDigit(t.hour/10);
        SevenSegment::digits[1] = SevenSegment::encodeDigit(t.hour%10);
        SevenSegment::digits[2] = SevenSegment::encodeDigit(t.minute/10);
        SevenSegment::digits[3] = SevenSegment::encodeDigit(t.minute%10);
        if (t.second % 2 == 0)
            SevenSegment::points = 1<<2 | 1<<3;
        else SevenSegment::points = 0;
        SevenSegment::show();
    }
}

ISR(INT1_vect) {
    Time t = RTC::getTime();
    if (time == &t1) {
        t2 = t;
        time = &t2;
    } else {
        t1 = t;
        time = &t1;
    }
}