#include "config.h"

#include <avr/io.h>
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
int main(void){
    I2C::enable();
    SevenSegment::enable();
    

    for(;;){
        ledOn();
        Time t = RTC::getTime();
        ledOff();
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
