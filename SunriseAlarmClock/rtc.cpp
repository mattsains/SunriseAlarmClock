#include "rtc.h"
#include "util/delay.h"
#include "i2c.h"

byte RTC::readRegister(byte regAddr)
{
    if(!I2C::start()) return 255;
    if(!I2C::address(RTC_ADDR)) return 255;
    if(!I2C::write(regAddr)) return 255;
    
    if(!I2C::start(true)) return 255;
    if(!I2C::address(RTC_ADDR, true)) return 255;
    byte result = I2C::read();
    I2C::stop();

    return result;
}

bool RTC::readRegisters(byte startAddr, byte count, byte* buffer) {
    if(!I2C::start()) return false;
    if(!I2C::address(RTC_ADDR)) return false;
    if(!I2C::write(startAddr)) return false;
    
    if(!I2C::start(true)) return false;
    if(!I2C::address(RTC_ADDR, true)) return false;

    for(byte i = 0; i < count; i++) {
        buffer[i] = I2C::read(i < count);
    }

    I2C::stop();
    return true;
}

bool RTC::writeRegister(byte regAddr, byte val)
{
    if(!I2C::start()) return false;
    if(!I2C::address(RTC_ADDR)) return false;
    if(!I2C::write(regAddr)) return false;
    if(!I2C::write(val)) return false;
    I2C::stop();
    return true;
}

bool RTC::writeRegisters(byte startAddr, byte count, byte* vals)
{
    if(!I2C::start()) return false;
    if(!I2C::address(RTC_ADDR)) return false;
    if(!I2C::write(startAddr)) return false;

    for(byte i=0; i < count; i++) {
        if(!I2C::write(vals[i])) return false;
    }

    I2C::stop();
    return true;
}

byte RTC::bcdToByte(byte tens, byte units)
{
    return tens*10+units;
}

byte RTC::byteToBcd(byte n)
{
    byte tens = n/10;
    byte units = n%10;
    return (tens<<4)|units;
}

Time RTC::getTime()
{
    Time t;
    byte buffer[7];
    if (!RTC::readRegisters(0, 7, buffer)) return t;

    byte secondReg = buffer[0];
    byte minuteReg = buffer[1];
    byte hourReg = buffer[2];
    byte dayReg = buffer[3];
    byte dateReg = buffer[4];
    byte monthReg = buffer[5];
    byte yearReg = buffer[6];

    bool isAMPM = hourReg & (1<<6);
    byte hour;
    if (isAMPM) {
        hour = RTC::bcdToByte((hourReg & (1<<4))>>4, hourReg & 0b1111);
        byte isAM = hourReg & (1<<5);
        if (isAM) {
            if (hour == 12) {
                hour = 0;
            }
            } else {
            if (hour != 12) {
                hour += 12;
            }
        }
        } else {
        hour = RTC::bcdToByte(hourReg>>4, hourReg&0b1111);
    }
    
    byte second = RTC::bcdToByte(secondReg>>4, secondReg&0b1111);
    byte minute = RTC::bcdToByte(minuteReg>>4, minuteReg&0b1111);
    byte day = dayReg;
    byte date = RTC::bcdToByte((dateReg&(1<<4))>>4, dateReg&0x1111);
    byte month = RTC::bcdToByte((monthReg&(1<<4))>>4, monthReg&0b1111);
    byte year = RTC::bcdToByte(yearReg>>4, hourReg&0b1111);

    t.second = second;
    t.minute = minute;
    t.hour = hour;
    t.day = day;
    t.date = date;
    t.month = month;
    t.year = year;

    return t;
}

bool RTC::setTime(Time t)
{
    byte hourReg = RTC::readRegister(HOURS_ADDR);
    bool isAMPM = hourReg & (1<<6);

    byte hour;
    if (isAMPM) {
        hour = RTC::byteToBcd(t.hourAsAMPM());
        hour |= (t.isPM()) << 5;
        } else {
        hour = RTC::byteToBcd(t.hour);
    }

    byte buffer[7];
    buffer[0] = RTC::byteToBcd(t.second);
    buffer[1] = RTC::byteToBcd(t.minute);
    buffer[2] = hour;
    buffer[3] = t.day;
    buffer[4] = RTC::byteToBcd(t.date);
    buffer[5] = RTC::byteToBcd(t.month);
    buffer[6] = RTC::byteToBcd(t.year);

    return RTC::writeRegisters(0, 7, buffer);
}

bool Time::isPM()
{
    return this->hour > 11;
}

byte Time::hourAsAMPM()
{
    if (this->isPM()) {
        return this->hour == 12? 12 : this->hour - 12;
        } else {
        return this->hour == 0? 12 : this->hour;
    }
}