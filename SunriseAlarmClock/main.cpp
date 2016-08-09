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

int main(void){
    SevenSegment::enable();
    I2C::enable();

    I2C::beginRead(RTC_ADDR);
    I2C::write(0, TWI_STR_ACK);
    byte result = I2C::read();
    I2C::end();

    SevenSegment::digits[3] = SevenSegment::encodeDigit(result%10);
    SevenSegment::digits[2] = SevenSegment::encodeDigit((result/10)%10);
    SevenSegment::digits[1] = SevenSegment::encodeDigit((result/100)%10);

    for(;;) {
        SevenSegment::show();
    }
    blinkSlowForever();
}
