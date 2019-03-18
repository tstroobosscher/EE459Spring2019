/*
 *  USC EE459 Spring 2019 Team 17 - Debug header
 */

#ifndef DEBUG_H
#define DEBUG_H

/*
 *  Do not #include headers that depend on debug.h ! these circular
 *  dependencies can corrupt the macro values inside debug and screw with
 *  the rest of the code
 */

/* Set for debug builds */
#define DEBUG

#define ATMEGA2560
#define ARDUINO

#ifdef DEBUG

#define UART_DBG(x) uart_write_str(x)
#define UART_DBG_HEX(x) uart_write_hex(x)
#define UART_DBG_32(x) uart_write_32(x)
#define UART_DBG_BIN(x, y) dump_bin(x, y)

#else

#define UART_DBG(x)
#define UART_DBG_HEX(x)
#define UART_DBG_32(x)
#define UART_DBG_BIN(x, y)


#endif

#endif
