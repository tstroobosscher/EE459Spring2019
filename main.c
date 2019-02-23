/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */
 
 // the code is very simple
//hi its hanna
//hello it's paul


#include <avr/io.h>

#include "utils.h"
#include "uart.h"
#include "fat.h"
#include "fifo.h"
#include "debug.h"
#include "spi.h"
#include "time.h"

struct fifo_t uart_rx_fifo;

struct PartitionTable pt;
struct FAT16BootSector bs;
struct FAT16Entry e;

struct atmel_328_time timer;

int main() {
	initialize_pins();
	initialize_uart(MYUBRR);
	initialize_spi();
	initialize_fifo(&uart_rx_fifo);
	if(initialize_sd() < 0)
		uart_write_str("main: unable to initialise SD\r\n");
	// initialize_timer(&timer, TIMER1);
	// TCCR1A |= (1 << WGM01);
	// TCCR1B |= (1 << CS12) | (1 << CS10);

	while(1) {
		dump_time(&timer);
		DELAY_MS(100);
	}

	return(0);
} 
