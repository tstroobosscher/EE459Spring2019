/*
 *	USC EE459 Spring 2019 Team 17 - Utils header
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <util/delay.h>

#include "debug.h"
#include "uart.h"

//#ifdef ARDUINO

/*
 * 16 Mhz Crystal (Arduino)
 */
//#define FOSC 16000000

//#else

/*
 * 7.3728 Mhz Crystal
 */
#define FOSC 7372800

//#endif

/*
 * Calculate number of arr elements
 */
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

/*
 * avr requires compile-time knowledge of delay times
 */
#define DELAY_MS(x) (_delay_ms(x))

/**
 * booleans
 */

#define true 1
#define false 0

/*
 *	Static buffer allocation
 */
#define BUF_SIZE 32

#ifdef DEBUG

#define UART_DBG(x) uart_write_str(UART_PORT_0, x)
#define UART_DBG_HEX(x) uart_write_hex(UART_PORT_0, x)
#define UART_DBG_32(x) uart_write_32(UART_PORT_0, x)
#define UART_DBG_BIN(x, y) dump_bin(UART_PORT_0, x, y)
#define UART_DBG_N(x, y) uart_write_strn(UART_PORT_0, x, y)

#else

#define UART_DBG(x)
#define UART_DBG_HEX(x)
#define UART_DBG_32(x)
#define UART_DBG_BIN(x, y)

#endif

uint32_t bind_args(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3);
int8_t byte_in_arr(uint8_t byte, void *buf, uint32_t size);
void dump_nbytes(uint8_t port, void *buf, uint16_t size);
void dump_bin(uint8_t port, void *buf, uint16_t nbytes);
void dump_byte(uint8_t port, uint8_t byte);
void trace(uint8_t port);

// void log(char *msg);

#endif
