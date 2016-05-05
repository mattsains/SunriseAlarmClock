#include "sevensegment.h"

byte SevenSegment::digits[4];

void SevenSegment::enable() {
    DISP_DDR |= (1<<DISP_1)
    | (1<<DISP_2)
    | (1<<DISP_3)
    | (1<<DISP_4);

    DISP_PORT &= ~((1<<DISP_1)
    | (1<<DISP_2)
    | (1<<DISP_3)
    | (1<<DISP_4));

    DISPX_DDR |= (1<<DISPX_PIN);
    DISPX_PORT &= ~(1<<DISPX_PIN);

    DISP_OE_DDR |= (1<<DISP_OE_PIN);
    DISP_DATA_DDR |= (1<<DISP_DATA_PIN);
    DISP_CLK_DDR |= (1<<DISP_CLK_PIN);
    DISP_LATCH_DDR |= (1<<DISP_LATCH_PIN);

    DISP_OE_PORT |= (1<<DISP_OE_PIN); //disable output
    DISP_CLK_PORT &= ~(1<<DISP_CLK_PIN); //bring clock low
}

void SevenSegment::show() {
    const byte COLUMNS[4] = { (1<<DISP_1), (1<<DISP_2), (1<<DISP_3), (1<<DISP_4) };

    for(byte i = 0; i<4; i++) {
        DISP_OE_PORT |= (1<<DISP_OE_PIN);
        DISP_PORT |= COLUMNS[i];
        SevenSegment::writeShift(~SevenSegment::digits[i]);
        DISP_OE_PORT &= ~(1<<DISP_OE_PIN);
        _delay_ms(1000);
        DISP_PORT &= ~COLUMNS[i];
    }
}

byte SevenSegment::encodeDigit(byte digit) {
    const byte digits[10] = { 0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110, 0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111 };
    return digits[digit];
}

void SevenSegment::writeShift(byte data) {
    DISP_LATCH_PORT &= ~(1<<DISP_LATCH_PIN);
    for(byte i=7; i!=255; i--) {
        if (data&(1<<i)) {
            DISP_DATA_PORT |= (1<<DISP_DATA_PIN);
            } else {
            DISP_DATA_PORT &= ~(1<<DISP_DATA_PIN);
        }
        DISP_CLK_PORT |= (1<<DISP_CLK_PIN);
        DISP_CLK_PORT &= ~(1<<DISP_CLK_PIN);
    }
    DISP_LATCH_PORT |= (1<<DISP_LATCH_PIN);
}