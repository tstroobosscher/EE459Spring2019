/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART)
 */

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

#include "uart.h"
#include "utils.h"

static struct uart_port {
  volatile uint8_t *ucsra;
  volatile uint8_t *ucsrb;
  volatile uint8_t *ucsrc;
  volatile uint8_t *ubrrl;
  volatile uint8_t *ubrrh;
  volatile uint8_t *udr;
  volatile uint8_t ucsz0;
  volatile uint8_t uscz1;
  volatile uint8_t rxen;
  volatile uint8_t txen;
  volatile uint8_t rxc;
  volatile uint8_t udre;
} uart_ports[] = {
  /* serial port 0 */
  {
    &UCSR0A,
    &UCSR0B,
    &UCSR0C,
    &UBRR0H,
    &UBRR0L,
    &UDR0,
    UCSZ00,
    UCSZ01,
    RXEN0,
    TXEN0,
    RXC0,
    UDRE0,
  },
  /* serial port 1, mega1284 and mega2560 */
#if defined ATMEGA2560 || defined ATMEGA1284
  {
    &UCSR1A,
    &UCSR1B,
    &UCSR1C,
    &UBRR1H,
    &UBRR1L,
    &UDR1,
    UCSZ10,
    UCSZ11,
    RXEN1,
    TXEN1,
    RXC1,
    UDRE1,
  },
#endif
};

char uart_read_char(uint8_t port) {
  while (!(*uart_ports[port].ucsra & (1 << uart_ports[port].rxc))) {
  }
  return *uart_ports[port].udr;
}

void uart_write_char(uint8_t port, uint8_t data) {
  while (!(*uart_ports[port].ucsra & (1 << uart_ports[port].rxc))) {
  }
  *uart_ports[port].udr = data;
}

void uart_write_str(uint8_t port, int8_t *buf) {
  while (*buf) {
    uart_write_char(port, *buf);
    buf++;
  }
}

void uart_write_strn(uint8_t port, int8_t *buf, uint8_t n) {
  for (uint8_t i = 0; i < n; i++) {
    if (!isascii(*buf))
      continue;
    uart_write_char(port, *buf);
    buf++;
  }
}

void uart_write_hex(uint8_t port, uint8_t n) {
  if (((n >> 4) & 15) < 10)
    uart_write_char(port, '0' + ((n >> 4) & 15));
  else
    uart_write_char(port, 'A' + ((n >> 4) & 15) - 10);
  n <<= 4;
  if (((n >> 4) & 15) < 10)
    uart_write_char(port, '0' + ((n >> 4) & 15));
  else
    uart_write_char(port, 'A' + ((n >> 4) & 15) - 10);
}

void uart_write_32(uint8_t port, uint32_t val) {
  uart_write_hex(port, val >> 24);
  uart_write_hex(port, val >> 16);
  uart_write_hex(port, val >> 8);
  uart_write_hex(port, val);
}

int8_t initialize_uart(uint8_t port, unsigned long ubrr_value) {
  /*
   *  the uart structures are already enumerated, just need 
   *  to be indexed
   */

  (void) uart_ports;

  if(port >= ARRAY_SIZE(uart_ports))

    return -1;

  struct uart_port *uart = &uart_ports[port];

  /*
   * Set Baud rate
   */
  *(uart->ubrrh) = (unsigned char)(ubrr_value >> 8);
  *(uart->ubrrl) = (unsigned char)(ubrr_value & 255);

  /*
   *	Frame Format: asynchronous, no parity, 1 stop bit, char size 8
   */
  *(uart->ucsrc) = (1 << UCSZ01) | (1 << UCSZ00);

  /*
   *	Enable The receiver and transmitter
   */
  *(uart->ucsrb)= (1 << RXEN0) | (1 << TXEN0);

  /*
   *	flush output buffer, kind of a hack, should really be checking
   *	buffer status
   */
  uart_write_str(port, (int8_t *) "\n\r\n\r");

  return 0;
}

// void uart_check_vowel_consonant() {
//   uint8_t ch = uart_read_char();
//   uint8_t buf[BUF_SIZE];

//   if ((ch >= 'a') && (ch <= 'z')) {
//     switch (ch) {
//     case 'a':
//       sprintf((char *)buf, "You entered a vowel: a\n\r");
//       break;
//     case 'e':
//       sprintf((char *)buf, "You entered a vowel: e\n\r");
//       break;
//     case 'i':
//       sprintf((char *)buf, "You entered a vowel: i\n\r");
//       break;
//     case 'o':
//       sprintf((char *)buf, "You entered a vowel: o\n\r");
//       break;
//     case 'u':
//       sprintf((char *)buf, "You entered a vowel: u\n\r");
//       break;
//     default:
//       sprintf((char *)buf, "That was the consonant: %c\n\r", ch);
//     }

//     uart_write_str((char *)buf);
//   }
// }
