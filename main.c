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
#include "pins.h"

const uint32_t macbeth_sect_addr = 0x00007E78;
const uint32_t macbeth_byte_addr = 0x00FCF000;

//struct fifo_t uart_rx_fifo;

struct PartitionTable pt[PARTITION_TABLE_ENTRIES];
struct FAT32BootSector bs;
//struct FAT32Entry e;

//struct atmel_328_time timer;

struct sd_ctx sd;

struct io_ctx sd_io;

int main() {
	initialize_pins();
	initialize_uart(MYUBRR);
	initialize_spi();
	//initialize_fifo(&uart_rx_fifo);
	if(initialize_sd(&sd) < 0)
		uart_write_str("main: unable to initialize SD\r\n");

	if(initialize_io(&sd_io, &sd))
		uart_write_str("main: unable to initialize io\r\n");
	// initialize_timer(&timer, TIMER1);
	// TCCR1A |= (1 << WGM01);
	// TCCR1B |= (1 << CS12) | (1 << CS10);

	// if(sd.sd_status == SD_ENABLED) {
	// 	if(sd_get_sector(&sd, macbeth_sect_addr, sd_io.sector_buf, 
	// 		sd.sd_sector_size) < 0)
	// 		uart_write_str("main: unable to read sector\r\n");
	// 	else
	// 		dump_bin(sd_io.sector_buf, sd.sd_sector_size);
	// }

	/* read the partition table */
	if(sd.sd_status == SD_ENABLED) {
		if(io_read_nbytes(&sd_io, &sd, &pt, PARTITION_TABLE_OFFSET, 
			sizeof(struct PartitionTable) * PARTITION_TABLE_ENTRIES) < 0) {
			uart_write_str("main: error reading partition table\r\n");
		}

		dump_bin(&pt, sizeof(struct PartitionTable) * PARTITION_TABLE_ENTRIES);

		/* parse the partition table */
		for(uint8_t i = 0; i < PARTITION_TABLE_ENTRIES; i++) {
			if((pt[i].partition_type == PARTITION_TYPE_FAT16_LT32M) || 
			(pt[i].partition_type == PARTITION_TYPE_FAT16_GT32M) ||
			(pt[i].partition_type == PARTITION_TYPE_FAT16_LBA)) {

				UART_DBG("main: FAT16\r\n");

			} else if((pt[i].partition_type == PARTITION_TYPE_FAT32_LT2G) || 
			(pt[i].partition_type == PARTITION_TYPE_FAT32_LBA)) {

				UART_DBG("main: FAT32\r\n");

				/* read the boot sector */
				if(io_read_nbytes(&sd_io, &sd, &bs, 
					sd.sd_sector_size * pt[i].start_sector, 
					sizeof(struct FAT32BootSector)) < 0) {
					uart_write_str("main: error FAT32 boot sector\r\n");
				}

				dump_bin(&bs, sizeof(struct FAT32BootSector));
			}
		}
	}

	while(1) {


	 // 	if(sd.sd_status == SD_ENABLED) {
		// 	for(uint16_t i = 0; i < 1000; i++) {
		// 		if(io_get_byte(&sd_io, &sd, macbeth_byte_addr + i, &byte) < 0) {
		// 			uart_write_str("main: byte read error\r\n");
		// 			byte = 0x00;
		// 		} 

		// 		if(byte == '\n')
		// 			uart_write_str("\r\n");
		// 		else {
		// 			if(isascii(byte)) {
		// 				uart_write_char(byte);
		// 			} else {
		// 				uart_write_char(".");
		// 			}
		// 		}

		// 		DELAY_MS(10);
		// 	}
		// }
	 
		// if(sd.sd_status == SD_ENABLED) {
		// 	if(io_read_nbytes(&sd_io, &sd, &pt, macbeth_byte_addr, 
		// 		sizeof(struct PartitionTable)) < 0) {
		// 		uart_write_str("main: error reading nbytes\r\n");
		// 	}

		// 	uart_write_str("data:\r\n");
		// 	dump_bin(&pt, sizeof(struct PartitionTable));
		// }

		// DELAY_MS(1000);
	}

	return(0);
} 
