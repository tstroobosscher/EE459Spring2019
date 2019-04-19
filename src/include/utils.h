/*
 *	USC EE459 Spring 2019 Team 17 - Utils header
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <util/delay.h>

#include "debug.h"
#include "uart.h"
#include "astdio.h"

#ifdef ARDUINO

/**
 * @brief      16 Mhz Crystal (Arduino)
 */
#define FOSC 16000000

#else

/*
 * @brief      7.3728 Mhz Oscillator
 */
#define FOSC 7372800

#endif

/*
 * @brief      Calculate number of array elements
 */
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

/**
 * @brief      AVR requires compile-time knowledge of delay times
 */
#define DELAY_MS(x) (_delay_ms(x))

/**
 * @brief      Boolean macro definitions
 */
#define true 1
#define false 0

/**
 * @brief      Static buffer allocation size
 */
#define BUF_SIZE 32

/**
 * @brief      Debug macros
 */
#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__)
#define DBG_BIN(x, y) dump_bin(UART_PORT_0, x, y)
#else
#define DBG(...)
#define DBG_BIN(x, y)
#endif

uint32_t bind_args(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3);
int8_t byte_in_arr(uint8_t byte, void *buf, uint32_t size);
void dump_nbytes(uint8_t port, void *buf, uint16_t size);
void dump_bin(uint8_t port, void *buf, uint16_t nbytes);

#endif
