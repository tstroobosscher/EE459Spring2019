/*
 *	USC EE459 Spring 2019 Team 17 - SD Flash
 */

#include <stdint.h>
#include <string.h>

#include "sd.h"
#include "spi.h"
#include "pins.h"
#include "utils.h"

int8_t initialize_sd() {
	uint8_t buf[CMD_RESP_BYTES];

	/*	
	 *	1. disable slave select
	 *	2. read 10 bytes (give card 80 clock ticks)
	 *	3. enable slave select
	 *	4. send CMD0 (0x40), SPI mde
	 *	5. send 4 zero bytes (args)
	 *	6. send 0x95 (CMD0 CRC)
	 *	7. read 8 bytes
	 */

	/* disable sd card */
	if(spi_device_disable(SPI_SD_CARD) < 0)

		/* sanity check */
		return -1;

	/* 80 clocks, init card */
	for(uint8_t i = 0; i < 10; i++)
		spi_write_char(0xFF);

	/* enable sd card */
	if(spi_device_enable(SPI_SD_CARD) < 0)

		/* sanity check */
		return -1;

	/* start the clock, must find the correct response in time */
	timein()

	do {
		/* send CMD0, get 8 bytes back */
		sd_command(CMD0, bind_args(NOARG, NOARG, NOARG, NOARG), 
		CMD0_CRC, CMD_RESP_BYTES, buf);

		/* too late */
		if(timeout())
			return -1;
	} while(!byte_in_arr(0x01, buf, CMD_RESP_BYTES));

	/* need to check if 0x01 is in the ret array */

	uint8_t j = 0;

cmd1:
	sd_command(CMD1, bind_args(NOARG, NOARG, NOARG, NOARG), NOCRC, 8, buf);

	if(byte_in_arr(0x00, buf, 8) < 0) {
		uart_write_str("sd: CMD1 returned incorrectly, retrying\n\r");
		dump_nbytes(buf, CMD_RESP_BYTES);
		j++;
		if(j==10)
			return;
		goto cmd1;
	} else
		uart_write_str("sd: CMD1 returned correctly\n\r");
}

void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t bytes, 
	uint8_t *buf) {
	/*
	 *	expecting a response of bytes length
	 *	buf must be at least bytes long
	 *	
	 *	SPI SD does not use crc, just default 0xFF once SPI is init'd
	 */

    spi_write_char(cmd);
    spi_write_char(arg>>24);
    spi_write_char(arg>>16);
    spi_write_char(arg>>8);
    spi_write_char(arg);
    spi_write_char(crc);
                
    for(uint8_t i = 0; i < bytes; i++)
        buf[i] = spi_write_char(0xFF);
                
    spi_device_disable(SPI_SD_CARD);
}

int8_t sd_set_block_len() {
	uint8_t buf[8];
	sd_command(CMD16, bind_args(0x00, 0x00, 0x02, 0x00), NOCRC, 8, buf);

}
