#ifndef RTC_H_
#define RTC_H_

#include "config.h"
#include "types.h"


#define SECONDS_ADDR 0x0
#define MINUTES_ADDR 0x1
#define HOURS_ADDR 0x2
#define DAY_ADDR 0x3
#define DATE_ADDR 0x4
#define MONTH_ADDR 0x5
#define YEAR_ADDR 0x6
#define A1_SECONDS_ADDR 0x7
#define A1_MINUTES_ADDR 0x8
#define A1_HOURS_ADDR 0x9
#define A1_DAYDATE_ADDR 0xa
#define A2_MINUTES_ADDR 0xb
#define A2_HOURS_ADDR 0xc
#define A2_DAYDATE_ADDR 0xd

struct Time {
  byte second;
  byte minute;
  byte hour;
  byte day;
  byte date;
  byte month;
  byte year;

  bool isPM();
  byte hourAsAMPM();
};

class RTC {
  public:
    static byte readRegister(byte regAddr);
    static void writeRegister(byte regAddr, byte val);
    static byte bcdToByte(byte tens, byte units);
    static byte byteToBcd(byte n);
    static Time getTime();
    static void setTime(Time t);
};

#endif /* RTC_H_ */