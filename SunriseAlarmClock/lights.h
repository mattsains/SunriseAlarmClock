#ifndef LIGHTS_H_
#define LIGHTS_H_

#include "types.h"
#include <avr/io.h>
#include <math.h>

class Lights {
    public:
    static void enable();
    static void setWhite(unsigned int intensity);
    static void setRed(unsigned int intensity);
    static void setCombined(unsigned long intensity);
};

#endif /* LIGHTS_H_ */