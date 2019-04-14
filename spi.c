/*
 *	USC EE459 Spring 2019 Team 17 - SPI
 */

#include <avr/io.h>
#include <stdint.h>

#include "debug.h"
#include "pins.h"
#include "spi.h"
#include "uart.h"
#include "utils.h"
#include "astdio.h"

static __attribute__((always inline)) void
spi_initialize_slave_select(uint8_t pin) {
  /*
   *	Initialize slave select pin with active high
   */
  if (pin_high(pin) < 0)
    DBG("spi: unable to initialize slave select pin\n\r");
  else
    DBG("spi: succesfully initialized slave select pin\n\r");
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

  /*
   *	We will need to see how fast we can use SPI, the board might cause
   *	the signals to slow down and force us to lower our maximum transfer
   *	speed
   *
   *	No division results in a SPI speed of FOSC/4
   */
}

void spi_set_speed() {}

int8_t spi_device_enable(uint8_t dev) {
  /* clear pins first */
  pin_high(PIN_SS_SD);

  /* set device enable */
  switch (dev) {
  case SPI_SD_CARD:
    if (pin_low(PIN_SS_SD) < 0)
      return -1;
    DBG("spi: SD slave select enabled\r\n");
    return 0;
  default:
    return -1;
  }
}

int8_t spi_device_disable(uint8_t dev) {
  /* set device disable */
  switch (dev) {
  case SPI_SD_CARD:
    if (pin_high(PIN_SS_SD) < 0)
      return -1;
    DBG("spi: SD slave select disabled\r\n");
    return 0;
  default:
    return -1;
  }
}

uint8_t spi_write_char(uint8_t ch) {
  SPDR = ch;
  while (!(SPSR & (1 << SPIF))) {
  }
  return SPDR;
}

uint8_t spi_read_char() {
  SPDR = 0xFF;
  while (!(SPSR & (1 << SPIF))) {
  }
  return SPDR;
}
