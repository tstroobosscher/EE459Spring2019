/*
 *	USC EE459 Spring 2019 Team 17 - Standard IO
 */

#include <stdio.h>

#include "astdio.h"
#include "uart.h"

/**
 *  { stdout callback prototype }
 */
static int stdio_putchar(char c, FILE *stream);

/**
 * { global FILE structure to buffer output chars }
 */
static FILE avr_stream =
    FDEV_SETUP_STREAM(stdio_putchar, NULL, _FDEV_SETUP_WRITE);

/**
 * @brief      { callback to write a byte to the AVR UART0 }
 *
 * @param      c       { char to be written }
 * @param      stream  { the stream buffer connected to UART0 }
 *
 * @return     { stream callback must return int }
 */
static int stdio_putchar(char c, FILE *stream) {

  /* raw terminals need to be carriage-returned upon newlines */
  if (c == '\n')
    stdio_putchar('\r', stream);

  uart_write_char(UART_PORT_0, c);
  return 0;
}
/**
 * @brief      { exported function to connect the buffer structure to stdout }
 *
 * @param      stream  The stream
 */
void initialize_stdio(FILE **stream) { *stream = &avr_stream; }
