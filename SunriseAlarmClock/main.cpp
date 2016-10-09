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
    _delay_ms(1);
    for(;;){
        I2C::enable();
        _delay_ms(1);
        ledOn();
        //start bit:
        if (!I2C::start(false)) {
            error();
            goto end;
        }
        //setup for setting address register
        
        //setup for write
        //set slave addr
        if (!I2C::address(RTC_ADDR)) {
            error();
            goto end;
        }
        //write to slave:
        if (!I2C::write(0)) {
            error();
            goto end;
        }
        
        //setup for receive
        //send repeated start
        if(!I2C::start(true)) {
            error();
            goto end;
        }
        
        //set slave addr
        if (!I2C::address(RTC_ADDR, true)) {
            error();
            goto end;
        }
        
        // start slave transmission
        if (!I2C::read(true)) {
            error();
            goto end;
        }
        // start slave transmission
        if (!I2C::read(true)) {
            error();
            goto end;
        }
        // start last slave transmission
        if (!I2C::read(false)) {
            error();
            goto end;
        }
        
        //stop the transfer:
        I2C::stop();
        
        
        _delay_us(100);
        end:
        _delay_us(50);
        ledOff();
        I2C::disable();
    }
}
