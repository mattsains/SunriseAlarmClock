#ifndef I2C_H_
#define I2C_H_

#include "types.h"
#include <avr/io.h>

#define TWI_STA_ACK 0x8
#define TWI_STR_ACK 0x10
#define TWI_SLR_ACK 0x40
#define TWI_SLW_ACK 0x18

class I2C {
    public:
	static void enable();
    static void disable();
	static byte beginWrite(byte addr);
	static byte beginRead(byte addr);
	static byte write(byte data, byte expected, byte retries = 3);
	static byte read();
	static void end();

    //private:
    static byte handshake();
};

#endif /* I2C_H_ */