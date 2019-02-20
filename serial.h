/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART) header
 */

#ifndef SERIAL_H
#define SERIAL_H

#include "utils.h"
#include "debug.h"

/*
 *	UART Baud rate
 */
#define BAUD 19200

/*
 *	Value for UBRR0 register
 */
#define MYUBRR ((FOSC/16/BAUD)-1)

/*
 *	Static buffer allocation
 */
#define BUF_SIZE 32

void initialize_uart(unsigned int ubrr_value);
char uart_read_char();
void uart_write_char(char data);
void write_str(char *buf);
void uart_check_vowel_consonant();
#endif
