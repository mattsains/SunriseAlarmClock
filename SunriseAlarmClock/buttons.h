#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "types.h"
#include "config.h"
#include <avr/io.h>

enum Button {
    NONE = 0,
    BUTTON_SET = 1<<0,
    BUTTON_RESET = 1<<1,
    BUTTON_NEXT = 1<<2
};

class Buttons {
    public:
    static Button readButtons();
};

#endif /* BUTTONS_H_ */