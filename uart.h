/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART) header
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

#include "debug.h"
#include "utils.h"

/*
 *	UART Baud rate
 */
#define BAUD 19200

/*
 *	Value for UBRR0 register
 */
#define MYUBRR ((FOSC / 16 / BAUD) - 1)

static const struct uart_port {
	uint16_t ubrr_value;
	volatile uint8_t *uart_reg;
	volatile uint8_t *ucsra;
	volatile uint8_t *ucsrb;
	volatile uint8_t *ucsrc;
	volatile uint8_t *ubrrl;
	volatile uint8_t *ubrrh;
	volatile uint8_t *udr;
}

void initialize_uart(unsigned long ubrr_value);
char uart_read_char();
void uart_write_hex(uint8_t n);
void uart_write_char(char data);
void uart_write_32(uint32_t val);
void uart_write_str(char *buf);
void uart_write_strn(uint8_t *buf, uint8_t n);
void uart_check_vowel_consonant();
#endif
