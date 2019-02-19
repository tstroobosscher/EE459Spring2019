/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */
 
 // the code is very simple
//hi its hanna
//hello it's paul


#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "utils.h"
#include "fifo.h"
#include "fat.h"

struct fifo_t uart_rx_fifo;

struct PartitionTable pt;
struct FAT16BootSector bs;
struct FAT16Entry e;

int main() {

	initialize_fifo(&uart_rx_fifo);
	initialize_pins();
	
	while(1)
		cycle_pins();

	return(0);
}
