/*
 *	USC EE459 Spring 2019 Team 17 - SPI
 */

#include <avr/io.h>
#include <stdint.h>

#include "spi.h"
#include "pins.h"
#include "debug.h"
#include "uart.h"

static void spi_initialize_slave_select(uint8_t pin) {
	/*
	 *	Initialize slave select pin with active high
	 */
#ifdef DEBUG_328
	if(pin_high(pin) < 0)
		uart_write_str("spi: unable to initialize slave select pin\n\r");
	else
		uart_write_str("spi: succesfully initialized slave select pin\n\r");
#else
	pin_high(pin);
#endif
}

void initialize_spi() {

	/*
	 *	must set CS before initializing SPI or the device will become a slave
	 */

	spi_initialize_slave_select(PIN_SS_SD);

	/* initialize SPI hardware,  */
	SPCR |= (1 << SPE);

	/* master device */
	SPCR |= (1 << MSTR); 

	/* div 16 */
	SPCR |= (1 << SPR1);
}

int8_t spi_device_enable(uint8_t dev) {
	/* clear pins first */
	pin_high(PIN_SS_SD);

	/* set device enable */
	switch(dev) {
		case SPI_SD_CARD :
			if(pin_low(PIN_SS_SD) < 0)
				return -1;
			return 0;
		default:
			return -1;
	}
}

int8_t spi_device_disable(uint8_t dev) {
	/* set device disable */
	switch(dev) {
		case SPI_SD_CARD :
			if(pin_high(PIN_SS_SD) < 0)
				return -1;
			return 0;
		default:
			return -1;
	}
}

uint8_t spi_write_char(uint8_t ch) {
    SPDR = ch;
    while(!(SPSR & (1 << SPIF))) {

#ifdef DEBUG_328
    	uart_write_str("spi: transfer not complete\n\r");
#endif
    	
    }
    return SPDR;
}
