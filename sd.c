/*
 *	USC EE459 Spring 2019 Team 17 - SD Flash
 */

#include <stdint.h>

#include "sd.h"
#include "spi.h"
#include "pins.h"

void sd_select() {
	/* clear SPI SS bits, set SD (active low) */
}

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


