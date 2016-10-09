#ifndef I2C_H_
#define I2C_H_

#include "types.h"
#include <avr/io.h>

#define TWSR_STAT_MASK 0xF8

#define TWI_STA_ACK 0x8
#define TWI_STR_ACK 0x10
#define TWI_SLWA_ACK 0x18
#define TWI_SLRA_ACK 0x40
#define TWI_SLW_ACK 0x28
#define TWI_SLR_A_ACK 0x50
#define TWI_SLR_N_ACK 0x58

class I2C {
    public:
    static void enable();
    static void disable();
    static bool start(bool repeated=false);
    static void stop();
    static bool address(byte address, bool read=false);
    static bool write(byte value);
    static byte read(bool keep_alive=false);
};

#endif /* I2C_H_ */