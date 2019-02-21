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

struct fifo_t uart_rx_fifo;

struct PartitionTable pt;
struct FAT16BootSector bs;
struct FAT16Entry e;

int main() {
	initialize_pins();
	initialize_uart(MYUBRR);
	initialize_spi();
	initialize_fifo(&uart_rx_fifo);

	while(1) {
		uart_check_vowel_consonant();
	}

	return(0);
} 
