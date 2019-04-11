#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int
uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                         _FDEV_SETUP_WRITE);

int
uart_putchar(char c, FILE *stream)
{
  if (c == '\n')
    uart_putchar('\r', stream);
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = c;
  return 0;
}

#define FOSC 16000000
/*
 *  UART Baud rate
 */
#define BAUD 38400

/*
 *  Value for UBRR0 register
 */
#define MYUBRR ((FOSC / 16 / BAUD) - 1)

char USARTReadChar() {
  while (!(UCSR0A & (1 << RXC0))) {
  }
  return UDR0;
}

void USARTWriteChar(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = data;
}

void uwrite_str(char *buf) {
  while (*buf) {
    USARTWriteChar(*buf);
    buf++;
  }
}

void uart_write_strn(uint8_t *buf, uint8_t n) {
  for (char i = 0; i < n; i++) {
    USARTWriteChar(*buf);
    buf++;
  }
}

void uart_write_hex(uint8_t n) {
  if (((n >> 4) & 15) < 10)
    USARTWriteChar('0' + ((n >> 4) & 15));
  else
    USARTWriteChar('A' + ((n >> 4) & 15) - 10);
  n <<= 4;
  if (((n >> 4) & 15) < 10)
    USARTWriteChar('0' + ((n >> 4) & 15));
  else
    USARTWriteChar('A' + ((n >> 4) & 15) - 10);
}

void uart_write_32(uint32_t val) {
  uart_write_hex(val >> 24);
  uart_write_hex(val >> 16);
  uart_write_hex(val >> 8);
  uart_write_hex(val);
}

extern FILE *stdout;

int main() {

  UBRR0=MYUBRR;
 
  UCSR0B|=(1<<TXEN0)|(1<<RXEN0);

  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);

  stdout = &mystdout;

  printf("Hello World!\n");
  printf("Goodbye World\n");

  while(1) {
  }

  return 0;
}
