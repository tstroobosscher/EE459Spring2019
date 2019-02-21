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
	if(assert_pin(PIN_SS_SD) < 0)
		uart_write_str("spi: ERROR: unable to assert SD Slave Select\n\r");
	else
		uart_write_str("spi: succesfully asserted SPI SS\n\r");
#else
	pin_high(PIN_SS_SD);
#endif
	//spi_write_char();
}

void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t bytes, 
	uint8_t *buf) {
	/*
	 *	expecting a response of bytes length
	 *	buf must be at least bytes long
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
                
    sd_disable();
}
