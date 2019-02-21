/*
 *	USC EE459 Spring 2019 Team 17 - SD Flash
 */

#include <stdint.h>
#include <string.h>

#include "sd.h"
#include "spi.h"
#include "pins.h"
#include "utils.h"

void initialize_sd() {
	uint8_t buf[8];

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
#ifdef DEBUG_328
	if(spi_device_disable(SPI_SD_CARD) < 0)
		uart_write_str("sd: ERROR: unable to deassert SD slave select\n\r");
	else
		uart_write_str("sd: succesfully deasserted SD slave select\n\r");
#else
	spi_device_disable(SPI_SD_CARD);
#endif

	/* 80 clocks, init card */
	for(uint8_t i = 0; i < 10; i++)
		spi_write_char(0xFF);

	uart_write_str("trace\n\r");

	/* enable sd card */
#ifdef DEBUG_328
	if(spi_device_enable(SPI_SD_CARD) < 0)
		uart_write_str("spi: ERROR: unable to assert SD Slave Select\n\r");
	else
		uart_write_str("spi: succesfully asserted SPI SS\n\r");
#else
	spi_device_enable(SPI_SD_CARD);
#endif

	sd_command(CMD0, bind_args(NOARG, NOARG, NOARG, NOARG), CMD0_CRC, 8, buf);

	/* need to check if 0x01 is in the ret array */

#ifdef DEBUG_328
	if(byte_in_arr(0x01, buf, 8) < 0){

		uint8_t err[64];
		uart_write_str("sd: ERROR: sd initialization failed\n\r");
		snprintf(err, 64, "bytes received: 0x%02X, 0x%02X, 0x%02X, 0x%02X, "
			"0x%02X, 0x%02X, 0x%02X, 0x%02X\n\r", buf[0], buf[1], buf[2], 
			buf[3], buf[4], buf[5], buf[6], buf[7]);
		uart_write_str(err);
	}
	else
		uart_write_str("sd: succesfully initialized sd card\n\r");
#endif

}

void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t bytes, 
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
                
    for(uint8_t i = 0; i < bytes; i++)
        buf[i] = spi_write_char(0xFF);
                
    spi_device_disable(SPI_SD_CARD);
}
