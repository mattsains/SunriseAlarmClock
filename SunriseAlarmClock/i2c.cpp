#include "i2c.h"

void I2C::enable() {
    TWCR = (1<<TWINT) // Clear I2C interrupt flag
          |(1<<TWEN); // Enable I2C
}

void I2C::disable() {
    TWCR = 1<<TWINT; // Clear interrupt flag, clear enable bit.
}

bool I2C::start(bool repeated) {
    TWCR |= (1<<TWSTA);
    TWCR |= (1<<TWINT);
    
    while (!(TWCR & (1<<TWINT))) ; // Wait for interrupt flag to be set

    const byte expected = repeated? TWI_STR_ACK : TWI_STA_ACK;
    
    return (TWSR & TWSR_STAT_MASK) == expected; //Check for success of start bit.
}

void I2C::stop() {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

bool I2C::address(byte address, bool read) {
    const byte rw_flag = read? 1 : 0;
    
    TWDR = (address<<1)|rw_flag;
    TWCR = (1<<TWINT) | (1<<TWEN); //clearing TWINT starts address transmission
    
    while (!(TWCR & (1<<TWINT))) ; // Wait for interrupt flag to be set
    
    const byte expected = read ? TWI_SLRA_ACK : TWI_SLWA_ACK;
    
    return (TWSR & TWSR_STAT_MASK) == expected;
}

bool I2C::write(byte value) {
    TWDR = value;
    TWCR = (1<<TWINT) | (1<<TWEN); //clearing TWINT starts transmission
    
    while (!(TWCR & (1<<TWINT))) ; // Wait for interrupt flag to be set
    
    return (TWSR & TWSR_STAT_MASK) == TWI_SLW_ACK;
}

byte I2C::read(bool keep_alive) {
    const byte enable_ack_flag = keep_alive ? (1<<TWEA) : 0;
    TWCR = (1<<TWINT) | (1<<TWEN) | enable_ack_flag; //clearing TWINT starts address transmission
    
    while (!(TWCR & (1<<TWINT))) ; // Wait for interrupt flag to be set
    
    const byte expected = keep_alive ? TWI_SLR_A_ACK : TWI_SLR_N_ACK;
    if ((TWSR & TWSR_STAT_MASK) == expected) {
        return TWDR;
     } else {
        return 255;
    }
}