/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART) header
 */

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdint.h>

#include "debug.h"
#include "utils.h"

/*
 *	UART Baud rate uart 0
 */
#define BUAD_UART_0 19200

/*
 *	UART Baud rate uart 1, ELM327 standard
 */
#define BUAD_UART_1 38400

/*
 *	preprocessor for UBRR register
 */
#define MYUBRR(x) ((FOSC / 16 / x))

const typedef enum {
  UART_PORT_0,
  UART_PORT_1,
} uart_port_index;

void initialize_uart(uint8_t port, unsigned long ubrr_value);
char uart_read_char(uint8_t port);
void uart_write_hex(uint8_t port, uint8_t n);
void uart_write_char(uint8_t port, char data);
void uart_write_32(uint8_t port, uint32_t val);
void uart_write_str(uint8_t port, char *buf);
void uart_write_strn(uint8_t port, uint8_t *buf, uint8_t n);
void uart_read_strn(uint8_t port, uint8_t *buf, uint8_t n);
char uart_data_available(port);
// void uart_check_vowel_consonant();
#endif
