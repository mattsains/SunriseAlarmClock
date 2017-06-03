#include "buttons.h"

inline Button operator|(Button a, Button b) {
    return static_cast<Button>(static_cast<int>(a) | static_cast<int>(b));
}

Button Buttons::readButtons() {
    Button result = 
    (BTN_PIN & (1<<(0+BTN_0_PIN)) ? NONE : BUTTON_NEXT) | 
    (BTN_PIN & (1<<(1+BTN_0_PIN)) ? NONE : BUTTON_RESET) |
    (BTN_PIN & (1<<(2+BTN_0_PIN)) ? NONE : BUTTON_SET);
    return result;    
}

