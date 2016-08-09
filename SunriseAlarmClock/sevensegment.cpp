#include "sevensegment.h"

byte SevenSegment::digits[4];
byte SevenSegment::points;

void SevenSegment::enable() {
    DISP_1_DDR |= (1<<DISP_1);
    DISP_2_DDR |= (1<<DISP_2);
    DISP_3_DDR |= (1<<DISP_3);
    DISP_4_DDR |= (1<<DISP_4);

    DISP_1_PORT &= ~(1<<DISP_1);
    DISP_2_PORT &= ~(1<<DISP_2);
    DISP_3_PORT &= ~(1<<DISP_3);
    DISP_4_PORT &= ~(1<<DISP_4);

    DISPX_DDR |= (1<<DISPX_PIN);
    DISPX_PORT &= ~(1<<DISPX_PIN);

    DISP_OE_DDR |= (1<<DISP_OE_PIN);
    DISP_DATA_DDR |= (1<<DISP_DATA_PIN);
    DISP_CLK_DDR |= (1<<DISP_CLK_PIN);
    DISP_LATCH_DDR |= (1<<DISP_LATCH_PIN);

    DISP_OE_PORT |= (1<<DISP_OE_PIN); //disable output
    DISP_CLK_PORT &= ~(1<<DISP_CLK_PIN); //bring clock low

    //reset shift register
    DISP_DATA_PORT &= ~(1<<DISP_DATA_PIN);
    for(int i=0; i<16; i++) {
        DISP_CLK_PORT ^= 1<<DISP_CLK_PIN;
    }
    // clk is now low by maths
}

void SevenSegment::show() {
    const byte COLUMN_PINS[4] = { (1<<DISP_1), (1<<DISP_2), (1<<DISP_3), (1<<DISP_4) };
    volatile byte* COLUMN_PORTS[4] = { &DISP_1_PORT, &DISP_2_PORT, &DISP_3_PORT, &DISP_4_PORT };

    for(byte i = 0; i<4; i++) {
        DISP_OE_PORT |= (1<<DISP_OE_PIN);
        *(COLUMN_PORTS[i]) |= COLUMN_PINS[i];
        SevenSegment::writeShift(~SevenSegment::digits[i]);
        DISP_OE_PORT &= ~(1<<DISP_OE_PIN);
        _delay_us(500);
        DISP_OE_PORT |= (1<<DISP_OE_PIN);
        _delay_us(4);
        *(COLUMN_PORTS[i]) &= ~COLUMN_PINS[i];
    }

    DISP_OE_PORT |= (1<<DISP_OE_PIN);
    DISPX_PORT |= (1<<DISPX_PIN);
    SevenSegment::writeShift(~SevenSegment::points);
    DISP_OE_PORT &= ~(1<<DISP_OE_PIN);
    _delay_us(500);
    DISP_OE_PORT |= (1<<DISP_OE_PIN);
    _delay_us(4);
    DISPX_PORT &= ~(1<<DISPX_PIN);
}

byte SevenSegment::encodeDigit(byte digit) {
    const byte digits[10] = { 0b0111111, 0b0000110, 0b1011101, 0b1001111, 0b1100110, 0b1101011, 0b1111011, 0b0001110, 0b1111111, 0b1101111 };
    return digits[digit];
}

void SevenSegment::writeShift(byte data) {
    DISP_LATCH_PORT &= ~(1<<DISP_LATCH_PIN);
    for(byte i=0; i<8; i++) {
        if (data&(1<<i)) {
            DISP_DATA_PORT &= ~(1<<DISP_DATA_PIN);
            } else {
            DISP_DATA_PORT |= (1<<DISP_DATA_PIN);
        }
        DISP_CLK_PORT |= (1<<DISP_CLK_PIN);
        DISP_CLK_PORT &= ~(1<<DISP_CLK_PIN);
    }
    DISP_LATCH_PORT |= (1<<DISP_LATCH_PIN);
}