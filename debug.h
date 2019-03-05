/*
 *	USC EE459 Spring 2019 Team 17 - Debug header
 */
#ifndef DEBUG_H
#define DEBUG_H

#include "uart.h"

/*
 *	Set for debug builds - Chip
 */
#define DEBUG

#define ARDUINO

#ifdef DEBUG

#define UART_DBG(x) uart_write_str(x)
#define UART_DBG_HEX(x) uart_write_hex(x)

#else

#define UART_DBG(x)
#define UART_DBG_HEX(x)

#endif

#endif
