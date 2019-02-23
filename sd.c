/*
 *	USC EE459 Spring 2019 Team 17 - SD Flash
 */

#include <stdint.h>
#include <string.h>
#include <avr/io.h>

#include "sd.h"
#include "spi.h"
#include "pins.h"
#include "utils.h"
#include "time.h"
#include "uart.h"
#include "debug.h"

static inline int8_t sd_wake_up() {
	/* enable sd card */
	if(spi_device_enable(SPI_SD_CARD) < 0)

		/* sanity check */
		return -1;

	/* 80 clocks, init card */
	for(uint8_t i = 0; i < 10; i++)
		spi_write_char(0xFF);

	/* disable sd card */
	if(spi_device_disable(SPI_SD_CARD) < 0)

		/* sanity check */
		return -1;

	return 0;
}

static void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t bytes, 
	uint8_t *buf) {
	/*
	 *	expecting a response of bytes length
	 *	buf must be at least bytes long
	 *	
	 *	SPI SD does not use crc, just default 0xFF once SPI is init'd
	 */

	spi_device_enable(SPI_SD_CARD);

    spi_write_char(cmd);
    spi_write_char(arg>>24);
    spi_write_char(arg>>16);
    spi_write_char(arg>>8);
    spi_write_char(arg);
    spi_write_char(crc);


    uart_write_str("sd: cmd sent: ");
    dump_byte(cmd);
    dump_byte(arg >> 24);
    dump_byte(arg >> 16);
    dump_byte(arg >> 8);
    dump_byte(arg);
    dump_byte(crc);
    uart_write_str("\r\n");
                
    for(uint8_t i = 0; i < bytes; i++)
        buf[i] = spi_write_char(0xFF);
                
    /* cs needs to be toggled between each command */
    spi_device_disable(SPI_SD_CARD);
}

static uint8_t sd_get_resp_byte(uint8_t *buf, uint8_t size) {
	for(uint8_t i = 0; i < size; i++)
		if(buf[i] & 0x80)
			continue;
		else
			return buf[i];
	return 0xFF;
}

static int8_t sd_find_resp_byte(uint8_t *buf, uint8_t size) {
	for(uint8_t i = 0; i < size; i++)
		if(buf[i] & 0x80)
			continue;
		else
			return i;
	return -1;
}

static int8_t sd_is_busy() {
	/* true/false logic */
	if(spi_write_char(0xFF) == 0xFF)
		return 0;
	else
		return 1;
}

int8_t initialize_sd(struct sd_ctx *sd) {
	uint8_t buf[CMD_RESP_BYTES];

	if(sd_wake_up() < 0)

		/* unhandled error */
		return -1;

	/* start the clock, must find the correct response in time */
	int trials = 0;

	do {
		/* send CMD0, get 8 bytes back */
		sd_command(CMD0, bind_args(NOARG, NOARG, NOARG, NOARG), 
		CMD0_CRC, CMD_RESP_BYTES, buf);

		/* too late */
		if(++trials > 10)
			goto failure;

	/* check if 0x01 is in the ret array */
	} while(!byte_in_arr(0x01, buf, CMD_RESP_BYTES));

	sd_command(CMD8, bind_args(NOARG, NOARG, 0x01, 0xAA), CMD8_CRC, 
		CMD_RESP_BYTES, buf);

	if(sd_get_resp_byte(buf, CMD_RESP_BYTES) & R1_ILLEGAL_COMMAND) {
		/* type 1 SD card */
		uart_write_str("sd: type 1 sd card\r\n");
		sd->sd_type = SD_TYPE_1;
	} else {
		if(!byte_in_arr(0x01, buf, CMD_RESP_BYTES))
			/* unhandled error */
			goto failure;
		if(!byte_in_arr(0xAA, buf, CMD_RESP_BYTES)) {
			/* unhandled error */
			goto failure;
		} else
			/* type 2 sd card */
			uart_write_str("sd: type 2 sd card\r\n");
			sd->sd_type = SD_TYPE_2;
	}	

	/* application command, lead with cmd55 */
	sd_command(CMD55, bind_args(NOARG, NOARG, NOARG, NOARG), NOCRC, 
		CMD_RESP_BYTES, buf);

	sd_command(ACMD41, sd->sd_type == SD_TYPE_2 ? 
		(bind_args(0x40, NOARG, NOARG, NOARG)) : 
		(bind_args(NOARG, NOARG, NOARG, NOARG)), 
		NOCRC, CMD_RESP_BYTES, buf);

	return 0;

failure:
	spi_device_disable(SPI_SD_CARD);
	return -1;

}
