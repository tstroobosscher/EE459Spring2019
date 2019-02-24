/*
 *	USC EE459 Spring 2019 Team 17 - Serial (UART)
 */

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#include "uart.h"
#include "utils.h"

char uart_read_char() {
    while(!(UCSR0A & (1 << RXC0))) {}
    return UDR0;
}

void uart_write_char(char data) {
    while(!(UCSR0A & (1 << UDRE0))) {}
    UDR0=data;
}

void uart_write_str(char *buf) {
    while(*buf) {
        uart_write_char(*buf);
        buf++;
    }
}

void uart_write_hex(uint8_t n) {
    if(((n>>4) & 15) < 10)
        uart_write_char('0' + ((n>>4)&15));
    else
        uart_write_char('A' + ((n>>4)&15) - 10);
    n <<= 4;
    if(((n>>4) & 15) < 10)
        uart_write_char('0' + ((n>>4)&15));
    else
        uart_write_char('A' + ((n>>4)&15) - 10);
}

void initialize_uart(unsigned int ubrr_value) {
    /*
     * Set Baud rate
     */
    UBRR0H = (unsigned char)(ubrr_value >> 8);  
    UBRR0L = (unsigned char)(ubrr_value & 255);

    /*
     *	Frame Format: asynchronous, no parity, 1 stop bit, char size 8
     */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    /*
     *	Enable The receiver and transmitter
     */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    /* 
     *	flush output buffer, kind of a hack, should really be checking
     *	buffer status
     */
    uart_write_str("\n\r\n\r");
}

void uart_check_vowel_consonant(){
	uint8_t ch = uart_read_char();
	uint8_t buf[BUF_SIZE];

	if ((ch >= 'a') && (ch <= 'z')) {
		switch(ch) {
			case 'a':
				sprintf(buf,"You entered a vowel: a\n\r");
				break;
			case 'e':
				sprintf(buf,"You entered a vowel: e\n\r");
				break;
			case 'i':
				sprintf(buf,"You entered a vowel: i\n\r");
				break;
			case 'o':
				sprintf(buf,"You entered a vowel: o\n\r");
				break;
			case 'u':
				sprintf(buf,"You entered a vowel: u\n\r");
				break;
			default:
				sprintf(buf, "That was the consonant: %c\n\r", ch);
		}

		uart_write_str(buf);
	}
}
