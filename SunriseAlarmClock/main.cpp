#include "config.h"

#include <avr/io.h>
#include <util/delay.h>

#include "sevensegment.h"

void ledOn() {
    DDRD |= (1<<PD5);
    PORTD |= (1<<PD5);
}

void ledOff() {
    DDRD |= (1<<PD5);
    PORTD &= ~(1<<PD5);
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
    DISP_OE_PORT &= ~(1<<DISP_OE_PIN);
    //reset?
    DISP_DATA_PORT &= ~(1<<DISP_DATA_PIN);
    for(int i=0; i<16; i++) {
        DISP_CLK_PORT ^= 1<<DISP_CLK_PIN;
    }

    for(byte i=0; i<4; i++) {
        SevenSegment::digits[i] = SevenSegment::encodeDigit(i);
    }

    SevenSegment::show();

    blinkSlowForever();
}
