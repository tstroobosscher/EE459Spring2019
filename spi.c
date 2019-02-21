/*
 *	USC EE459 Spring 2019 Team 17 - SPI
 */

#include <avr/io.h>
#include <stdint.h>

#include "spi.h"
#include "pins.h"
#include "debug.h"
#include "uart.h"

void initialize_spi() {


	/* start off not selected (active low) */
#ifdef DEBUG_328
	if(assert_pin(PIN_CS) < 0)
		write_str("spi: ERROR: unable to assert SPI SS\n\r");
	else
		write_str("spi: succesfully asserted SPI SS\n\r");
#else
	assert_pin(PIN_CS);
#endif

	/* initialize SPI hardware,  */
	SPCR |= (1 << SPE);

	/* master device */
	SPCR |= (1 << MSTR); 

	/* div 16 */
	SPCR |= (1 << SPR1);
}

uint8_t spi_write_char(uint8_t ch) {
    SPDR = ch;
    while(!(SPSR & (1 << SPIF))) {}       
    return SPDR;
}
