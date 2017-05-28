#include "lights.h"

void Lights::enable() {
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
}

void Lights::setWhite(unsigned int intensity) {
    intensity &= 0x3FF;
    byte high = intensity >> 8;
    byte low = intensity && 0xFF;
    OCR1BH = high;
    OCR1BL = low;
}

void Lights::setRed(unsigned int intensity) {
    intensity &= 0x3FF;
    byte high = intensity >> 8;
    byte low = intensity && 0xFF;
    OCR1AH = high;
    OCR1AL = low;
}

void Lights::setCombined(unsigned int intensity) {
    if (intensity < 0x3FF) {
        DDRB |= (1<<PB0);
        PORTB &= ~(1<<PB0);
        setRed(intensity);
        setWhite(0);
    } else {
        DDRB |= (1<<PB0);
        PORTB |= (1<<PB0);
        setRed(0x3f0);
        setWhite(intensity - 0x3FF);
    }
}