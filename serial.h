/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART) header
 */

#ifndef SERIAL_H
#define SERIAL_H

#include "utils.h"

#define BAUD 19200               // Baud rate used
#define MYUBRR ((FOSC/16/BAUD)-1) // Value for UBRR0 register


/*
 *	short is a C++ thing, we should really be using width-specific typedefs
 *	like "uint8_t" defined in <stdint.h>
 */

// void serial_init(); 
// void serial_out(unsigned char ch);
// unsigned char serial_in();
// void serial_out_str(char *buf);

void initialize_uart(unsigned int ubrr_value);
char uart_read_char();
void uart_write_char(char data);
void write_str(char *buf);
#endif
