#include "rtc.h"
#include "util/delay.h"
#include "i2c.h"

byte RTC::readRegister(byte regAddr)
{
	_delay_us(10);
	
	I2C::beginWrite(RTC_ADDR);
	I2C::write(regAddr);
	I2C::end();
	
	_delay_us(10);
	
	I2C::beginRead(RTC_ADDR);
	byte result = I2C::read();
	I2C::end();

	return result;
}

void RTC::writeRegister(byte regAddr, byte val)
{
	_delay_us(10);
	
	I2C::beginWrite(RTC_ADDR);
	I2C::write(regAddr);
	I2C::write(val);
	I2C::end();
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
	byte secondReg = RTC::readRegister(SECONDS_ADDR);
	byte minuteReg = RTC::readRegister(MINUTES_ADDR);
	byte hourReg = RTC::readRegister(HOURS_ADDR);
	byte dayReg = RTC::readRegister(DAY_ADDR);
	byte dateReg = RTC::readRegister(DATE_ADDR);
	byte monthReg = RTC::readRegister(MONTH_ADDR);
	byte yearReg = RTC::readRegister(YEAR_ADDR);

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

	Time t;
	t.second = second;
	t.minute = minute;
	t.hour = hour;
	t.day = day;
	t.date = date;
	t.month = month;
	t.year = year;

	return t;
}

void RTC::setTime(Time t)
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

	RTC::writeRegister(SECONDS_ADDR, RTC::byteToBcd(t.second));
	RTC::writeRegister(MINUTES_ADDR, RTC::byteToBcd(t.minute));
	RTC::writeRegister(HOURS_ADDR, hour);
	RTC::writeRegister(DAY_ADDR, t.day);
	RTC::writeRegister(DATE_ADDR, RTC::byteToBcd(t.date));
	RTC::writeRegister(MONTH_ADDR, RTC::byteToBcd(t.month));
	RTC::writeRegister(YEAR_ADDR, RTC::byteToBcd(t.year));
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