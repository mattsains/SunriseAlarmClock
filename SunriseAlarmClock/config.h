#ifndef CONFIG_H_
#define CONFIG_H_

#include <avr/io.h>

#define F_CPU 20000000L

#define RTC_ADDR 0b1101000

#define DISP_PORT PORTC
#define DISP_DDR DDRC
#define DISP_1 PC0
#define DISP_2 PC1
#define DISP_3 PC2
#define DISP_4 PC3

#define DISPX_PORT PORTD
#define DISPX_DDR DDRD
#define DISPX_PIN PD0

// For 75HC595
#define DISP_CLK_DDR DDRB
#define DISP_CLK_PORT PORTB
#define DISP_CLK_PIN PB3

#define DISP_LATCH_DDR DDRB
#define DISP_LATCH_PORT PORTB
#define DISP_LATCH_PIN PB6

#define DISP_OE_DDR DDRB
#define DISP_OE_PORT PORTB
#define DISP_OE_PIN PB5

#define DISP_DATA_DDR DDRB
#define DISP_DATA_PORT PORTB
#define DISP_DATA_PIN PB4

#endif /* CONFIG_H_ */