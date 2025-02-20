/*
 *	USC EE459 Spring 2019 Team 17 - SPI Header
 */

#ifndef SPI_H
#define SPI_H

#define SPI_SD_CARD 1
#define SPI_LCD 2

void initialize_spi();
uint8_t spi_write_char(uint8_t ch);
uint8_t spi_read_char();
int8_t spi_device_enable(uint8_t dev);
int8_t spi_device_disable(uint8_t dev);

#define SPI_FOSC_OVER_4 (0 << SPI2X) | (0 << SPR1) | (0 << SPR0)
#define SPI_FOSC_OVER_16 (0 << SPI2X) | (0 << SPR1) | (1 << SPR0)
#define SPI_FOSC_OVER_64 (0 << SPI2X) | (1 << SPR1) | (0 << SPR0)
#define SPI_FOSC_OVER_128 (0 << SPI2X) | (1 << SPR1) | (1 << SPR0)
#define SPI_2X_FOSC_OVER_2 (1 << SPI2X) | (0 << SPR1) | (0 << SPR0)
#define SPI_2X_FOSC_OVER_8 (1 << SPI2X) | (0 << SPR1) | (1 << SPR0)
#define SPI_2X_FOSC_OVER_32 (1 << SPI2X) | (1 << SPR1) | (0 << SPR0)
#define SPI_2X_FOSC_OVER_64 (1 << SPI2X) | (1 << SPR1) | (1 << SPR0)

static const enum {
	FOSC_OVER_4,
	FOSC_OVER_16,
	FOSC_OVER_64,
	FOSC_OVER_128,
};

#endif
