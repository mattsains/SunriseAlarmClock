#include "i2c.h"

void I2C::enable() {
	TWCR = (1<<TWINT) // Clear I2C interrupt flag
		|(1<<TWEN); // Enable I2C
}

void I2C::disable() {
	TWCR = 1<<TWINT; // Clear interrupt flag, clear enable bit.
}

byte I2C::beginWrite(byte addr) {
    handshake();
    return write((addr<<1), TWI_SLW_ACK);
}

byte I2C::beginRead(byte addr) {
    handshake();
    return write((addr<<1)|1, TWI_SLR_ACK);
}

byte I2C::handshake() {
    TWCR |= (1<<TWSTA);
    TWCR |= (1<<TWINT);
    while (!(TWCR & (1<<TWINT))) ; // Wait for interrupt flag to be set

    if ((TWSR & (0b11111000)) != TWI_STA_ACK) //Check for success of start bit.
        return false;

    return true;
}

byte I2C::write(byte data, byte expected, byte retries) {
    if (retries == 0)
        return false;

    TWDR = data;
    TWCR |= (1<<TWINT);

    while (!(TWCR & (1<<TWINT))) ; // Wait for interrupt flag to be set

    if ((TWSR & (0b11111000)) != expected) {
        // ACK not received
        // Send a (repeated) start bit and retry.
        
        return TWSR;//write(data - 1);
    }

    return true;
}

byte I2C::read() {
    TWCR |= (1<<TWEA);
    TWCR |= (1<<TWINT);
    while(!(TWCR & (1<<TWINT))) ; //Wait for data to come in
    
    byte data = TWDR;
    TWCR |= (1<<TWINT);
    return data;
}

void I2C::end() {
    // Send a stop bit
    TWCR |= (1<<TWSTO)|(1<<TWINT);
    while (!(TWCR & (1<<TWINT))) ; // Wait for interrupt flag to go low
}