/*
 *	USC EE459 Spring 2019 Team 17 - SD Flash
 */

#include <stdint.h>

#include "sd.h"
#include "spi.h"
#include "pins.h"

void initialize_sd() {

	/* start off not selected (active low) */
#ifdef DEBUG_328
	if(pin_high(PIN_SS_SD) < 0)
		uart_write_str("spi: ERROR: unable to assert SD Slave Select\n\r");
	else
		uart_write_str("spi: succesfully asserted SPI SS\n\r");
#else
	pin_high(PIN_SS_SD);
#endif

	/*	
	 *	1. disable slave select
	 *	2. read 10 bytes (give card 80 clock ticks)
	 *	3. enable slave select
	 *	4. send CMD0 (0x40), SPI mde
	 *	5. send 4 zero bytes (args)
	 *	6. send 0x95 (CMD0 CRC)
	 *	7. read 8 bytes
	 */


}

void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t bytes, 
	uint8_t *buf) {
	/*
	 *	expecting a response of bytes length
	 *	buf must be at least bytes long
	 *	
	 *	SPI SD does not use crc, just default 0xFF once SPI is init'd
	 */

	spi_device_enable(PIN_SS_SD);

    spi_write_char(cmd);
    spi_write_char(arg>>24);
    spi_write_char(arg>>16);
    spi_write_char(arg>>8);
    spi_write_char(arg);
    spi_write_char(crc);
                
    for(uint8_t i = 0; i < bytes; i++)
        buf[i] = spi_write_char(0xFF);
                
    spi_device_disable(PIN_SS_SD);
}
