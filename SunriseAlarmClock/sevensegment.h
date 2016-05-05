#ifndef SEVENSEGMENT_H_
#define SEVENSEGMENT_H_

#include "config.h"
#include "types.h"
#include <util/delay.h>

class SevenSegment 
{
  public:
    static byte digits[4];

	  static void enable();
    static void show();

  //private:
    static byte encodeDigit(byte digit);
    static void writeShift(byte data);
};

#endif /* SEVENSEGMENT_H_ */