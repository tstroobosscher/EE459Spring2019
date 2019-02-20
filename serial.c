/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART)
 */

#include <stdio.h>
#include "serial.h"
#include "utils.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/setbaud.h>


/*
 *	February 18, 2019
 *
 *	So it looks like you're pretty close, we just need to link against the 
 *	main.c file. Instead of defining a separate main function, we can just put
 *	the prototype code in the original main function, and then include "serial.h"
 *	at the top of that file. The Makefile is already looking for the
 *	serial object file, so once you include the header, you'll be able to call
 *	these functions from over there.
 *	
 *	Also, in order to use the macro values (FOSC, BUAD, etc), we should put
 *	them at the top of the serial header file so that the compiler
 *	doesn't have to search the compiled values when exporting these 
 *	functions.
 *	
 *	One more thing, the standard library functions like printf() and scanf()
 *	probably wont work correctly without some extra library initialization.
 *	This is because the AVR system has no stdin or stdout file because it has
 *	no operating system. So, we could redefine a hardware interface by
 *	assigning an address to 'extern FILE *stdin = &uart_stream;' but lets just
 *	try to get the hardware up and running first.
 *	
 *	I'm thinking that the best way to test the initialization function is just
 *	to send data out continuously and then scope it with the oscilloscope. Then
 *	we can hook up the serial monitor and loopback data. So the while loop will
 *	just pick up whatever char is sitting in the recieve register, and then
 *	send it out. That way we'll be able to see if any data comes back to the
 *	console and whether it is actually being processed by the device
 *	
 *	Oh also, FOSC is the frequency of the chip that weber gave us, it's like 
 *	7.12 something Megaherts
 *
 *
 *	Nice work we're getting there.
 *	
 */



void serial_init() {
	UBRR0 = MYUBRR; // Set baud rate

#ifdef USE_2X

	UCSR0A |= (1 << U2X0);

#else 
	UCSR0A &= ~(1 << U2X0);

#endif

	UCSR0B |= (1 << TXEN0 ) | (1 << RXEN0 ); // Turn on transmitter and receiver
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Set for async . operation , no parity ,
	// one stop bit , 8 data bits
}


// serial_out - Output a byte to the USART0 port


void serial_out(char ch) {

	while (( UCSR0A & (1 << UDRE0 )) == 0);
	UDR0 = ch;
} 


// serial_in - Read a byte from the USART0 and return it


char serial_in() {

	while ( !( UCSR0A & (1 << RXC0 )) );
	return UDR0;
}

