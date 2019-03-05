/*
 *	USC EE459 Spring 2019 Team 17 - Utils header
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <util/delay.h>


#include "debug.h"

#define ATMEGA2560

#ifdef ARDUINO

/*
 * 16 Mhz Crystal (Arduino)
 */
#define FOSC 16000000

#else

/*
 * 7.3728 Mhz Crystal
 */
#define FOSC 7372800

#endif

/*
 * Calculate number of arr elements
 */
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

/*
 * avr requires compile-time knowledge of delay times
 */
#define DELAY_MS(x) (_delay_ms(x))

#ifdef DEBUG_86
void dump_bin(void *buf, int size);
#endif

/**
 * booleans
 */

#define true 1
#define false 0

/*
 *	Static buffer allocation
 */
#define BUF_SIZE 32

#ifndef DEBUG_86
uint32_t bind_args(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3);
int8_t byte_in_arr(uint8_t byte, void *buf, uint32_t size);
void dump_nbytes(void *buf, uint16_t size);
void dump_bin(void *buf, uint16_t nbytes);
void dump_byte(uint8_t byte);
void trace();
#endif

// void log(char *msg);

#endif
