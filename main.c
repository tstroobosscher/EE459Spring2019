/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */
 
 // the code is very simple
//hi its hanna
//hello it's paul


#include <avr/io.h>
#include <util/delay.h>
#include <string.h>


#include "pins.h"
#include "utils.h"
#include "fifo.h"
#include "fat16.h"
#include "serial.h"

struct fifo_t uart_rx_fifo;

struct PartitionTable pt;
struct FAT16BootSector bs;
struct FAT16Entry e;

int main() {

	initialize_fifo(&uart_rx_fifo);
	initialize_pins();
	serial_init();

	char buff[128];

	//const char* string = "christian";
	
	while(1) {

		unsigned char ch = serial_in();

		serial_out(ch);

		//DELAY_MS(10);

		//serial_out('i');


		/*if ((ch >= 'a') && (ch <= 'z')) {
			switch(ch) {
				case 'a':
					sprintf(buff,"You entered a vowel: a");
					break;
				case 'e':
					sprintf(buff,"You entered a vowel: e");
					break;
				case 'i':
					sprintf(buff,"You entered a vowel: i");
					break;
				case 'o':
					sprintf(buff,"You entered a vowel: o");
					break;
				case 'u':
					sprintf(buff,"You entered a vowel: u");
					break;
				default:
					sprintf(buff, "That was the consonant: %c",ch);
			}

			serial_out_str(buff,strlen(buff));


		} */

		//serial_out_str('christian',9);
		//serial_out('c');
	}

	return(0);
}
