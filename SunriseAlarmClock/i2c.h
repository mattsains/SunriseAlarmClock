#ifndef I2C_H_
#define I2C_H_

#include "types.h"
#include <avr/io.h>

class I2C {
  public:
	static void enable();
	static void beginWrite(byte addr);
	static void beginRead(byte addr);
	static void write(byte data);
	static byte read();
	static void end();
	static void disable();
};

#endif /* I2C_H_ */