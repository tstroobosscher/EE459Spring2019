/*
 *	USC EE459 Spring 2019 Team 17 - SPI Header
 */

#ifndef SPI_H
#define SPI_H

static const enum {
	SPI_SD_CARD
};

void initialize_spi();
uint8_t spi_write_char(uint8_t ch);
int8_t spi_device_enable(uint8_t dev);
int8_t spi_device_disable(uint8_t dev);

#endif
