#include <stdio.h>

#include "uart.h"
#include "astdio.h"

static int stdio_putchar(char c, FILE *stream);

static FILE avr_stream = FDEV_SETUP_STREAM(stdio_putchar, NULL, _FDEV_SETUP_WRITE);

static int stdio_putchar(char c, FILE *stream) {
  if (c == '\n')
    stdio_putchar('\r', stream);
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = c;
  return 0;
}

void initialize_stdio(FILE **stream) {

  *stream = &avr_stream;

}
