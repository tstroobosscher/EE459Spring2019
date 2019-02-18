/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART) header
 */

#ifndef SERIAL_H
#define SERIAL_H

/*
 *	short is a C++ thing, we should really be using width-specific typedefs
 *	like "uint8_t" defined in <stdint.h>
 */

void serial_init(unsigned short); 
void serial_out(char ch);
char serial_in();

#endif
