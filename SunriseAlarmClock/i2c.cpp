#include "i2c.h"

void I2C::enable() {
	TWCR = (1<<TWINT) // Clear I2C interrupt flag
		|(1<<TWEN); // Enable I2C
}

void I2C::disable() {
	TWCR = 1<<TWINT; // Clear interrupt flag, clear enable bit.
}