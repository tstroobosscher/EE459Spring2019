/*
 *	USC EE459 Spring 2019 Team 17 - Timer header
 */

#ifndef TIMER_H
#define TIMER_H

#include "debug.h"

/*
 *	Timer control pins:
 *      CS02    CS01    CS00
 *        0       0       0      No clock
 *        0       0       1      FOSC
 *        0       1       0      FOSC/8  
 *        0       1       1      FOSC/64
 *        1       0       0      FOSC/256
 *        1       0       1      FOSC/1024
 *        1       1       0      External, T0 pin, falling edge
 *        1       1       1      External, T0 pin, rising edge 
 */

/*
 *	Timer count = ( Required Delay / Clock Period ) - 1
 *	            = ( Required Delay * FOSC ) - 1
 *	
 *	Timer count (ms) = (Delay (ms) * FOSC / 1000) - 1
 */

#define TIMER_RES_1_MS FOSC/1000 - 1

#define TIMER_NO_CLK 0b000
#define TIMER_FOSC 0b001
#define TIMER_FOSC_DIV_8 0b010
#define TIMER_FOSC_DIV_64 0b011
#define TIMER_FOSC_DIV_256 0b100
#define TIMER_FOSC_DIV_1024 0b101

static const enum {
	TIMER0,
	TIMER1,
	TIMER2,
} atmel_timer;

struct atmel_328_time {
	uint16_t timer_value;
	uint8_t timer_reg;
	/* clock division (Ardino and the board)*/
	uint8_t timer_ctl;
};

int8_t timeout(struct atmel_328_time *t, uint16_t timeout_val);
void timein(struct atmel_328_time *t);
void dump_time(struct atmel_328_time *t);
void initialize_timer(struct atmel_328_time *t, uint8_t timer_reg);

#endif
