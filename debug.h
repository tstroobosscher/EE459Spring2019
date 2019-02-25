/*
 *	USC EE459 Spring 2019 Team 17 - Debug header
 */
#ifndef DEBUG_H
#define DEBUG_H

#include "uart.h"

/*
 *	Set for debug builds - Desktop
 */
//#define DEBUG_86

/*
 *	Set for debug builds - Chip
 */
//#define DEBUG_328

/* 
 * Changes clock speed to 16MHz 
 */
#define ARDUINO

#ifdef DEBUG_328

#define UART_DBG(x) uart_write_str(x)
#define UART_DBG_HEX(x) uart_write_hex(x)

#else

#define UART_DBG(x)
#define UART_DBG_HEX(x)

#endif

#endif
