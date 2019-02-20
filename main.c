/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */
 
 // the code is very simple
//hi its hanna
//hello it's paul


#include <avr/io.h>

#include "utils.h"
#include "serial.h"
#include "fat16.h"
#include "fifo.h"

struct fifo_t uart_rx_fifo;

struct PartitionTable pt;
struct FAT16BootSector bs;

int main() {
	initialize_pins();
	initialize_uart(MYUBRR);

	char buff[128];

	while(1) {

		unsigned char ch = uart_read_char();

		if ((ch >= 'a') && (ch <= 'z')) {
			switch(ch) {
				case 'a':
					sprintf(buff,"You entered a vowel: a\n\r");
					break;
				case 'e':
					sprintf(buff,"You entered a vowel: e\n\r");
					break;
				case 'i':
					sprintf(buff,"You entered a vowel: i\n\r");
					break;
				case 'o':
					sprintf(buff,"You entered a vowel: o\n\r");
					break;
				case 'u':
					sprintf(buff,"You entered a vowel: u\n\r");
					break;
				default:
					sprintf(buff, "That was the consonant: %c\n\r",ch);
			}

			write_str(buff);
		}
	}

	return(0);
} 
