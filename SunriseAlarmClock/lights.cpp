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
    unsigned long x = pow(2.718281, intensity/5909.2) - 1;
    OCR1BH = x >> 8;
    OCR1BL = x & 0xFF;
}

void Lights::setRed(unsigned int intensity) {
    unsigned long x = pow(2.718281, intensity/5909.2) - 1;
    OCR1AH = x >> 8;
    OCR1AL = x & 0xFF;
}

void Lights::setCombined(unsigned long intensity) {
   if (intensity >= 131070) intensity=131070;
   setRed(intensity >= 0xffff ? 0xffff : intensity);
   setWhite(intensity < 0xffff ? 0 : (intensity-0xffff));
}