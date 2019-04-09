/*
 *	USC EE459 Spring 2019 Team 17 - SPI Header
 */

#ifndef SPI_H
#define SPI_H

static const enum {
  SPI_SD_CARD,
  SPI_LCD,
}

struct spi_device {
  uint8_t dev_id;
  uint8_t spi_flags;
} spi_devices

void initialize_spi();
uint8_t spi_write_char(uint8_t ch);
uint8_t spi_read_char();
int8_t spi_device_enable(uint8_t dev);
int8_t spi_device_disable(uint8_t dev);

#define SPI_FOSC_OVER_4 (0 << SPR1) | (0 << SPR0)
#define SPI_FOSC_OVER_16 (0 << SPR1) | (1 << SPR0)
#define SPI_FOSC_OVER_64 (1 << SPR1) | (0 << SPR0)
#define SPI_FOSC_OVER_128 (1 << SPR1) | (1 << SPR0)

#endif
