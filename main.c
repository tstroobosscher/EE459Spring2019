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
#include "sd.h"
#include "io.h"

struct fifo_t uart_rx_fifo;

struct PartitionTable pt;
struct FAT32BootSector bs;
struct FAT32Entry e;

struct atmel_328_time timer;

struct sd_ctx sd;

struct io_ctx sd_io;

int main() {
	initialize_pins();
	initialize_uart(MYUBRR);
	initialize_spi();
	initialize_fifo(&uart_rx_fifo);
	if(initialize_sd(&sd) < 0)
		uart_write_str("main: unable to initialize SD\r\n");
	// initialize_timer(&timer, TIMER1);
	// TCCR1A |= (1 << WGM01);
	// TCCR1B |= (1 << CS12) | (1 << CS10);

	if(sd.sd_status == SD_ENABLED)
		if(sd_get_sector(&sd, 0x7E78, sd_io.sector_buf, SECTOR_SIZE) < 0)
			uart_write_str("main: unable to read sector\r\n");
		else
			dump_bin(sd_io.sector_buf, SECTOR_SIZE);

	while(1) {}

	return(0);
} 
