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

/* turn off SPI dbg messages */
#define UART_DBG(x)
#define UART_DBG_HEX(x)

struct spi_device {
  uint8_t dev_id;
  uint8_t clock_div;
  uint64_t select_pin;
  const char *dev_name;
} spi_devices[] = {

  /* default speed has been working fine */
  {
    SPI_SD_CARD,
    SPI_FOSC_OVER_4,
    PIN_SS_SD,
    "SD Card",
  },

  /* LCD needs the slowest possible clock */
  {
    SPI_LCD,
    SPI_FOSC_OVER_128,
    PIN_SS_LCD,
    "LCD",
  }
};


static __attribute__((always inline)) void
spi_initialize_slave_select(uint8_t pin) {
  /*
   *	Initialize slave select pin with active high
   */
  if (pin_high(pin) < 0)
    UART_DBG("spi: unable to initialize slave select pin\n\r");
  else
    UART_DBG("spi: succesfully initialized slave select pin\n\r");
}

void spi_set_speed(uint8_t flags) {
  /* clear lower SPR1, SPR0 */
  SPCR &= ~(0x03);
  SPCR |= (flags);
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

int8_t spi_device_enable(uint8_t dev) {
  /* clear pins first */
  for(uint8_t i = 0; i < ARRAY_SIZE(spi_devices); i++) {
    if(pin_high(spi_devices[i].select_pin) < 0) {
      UART_DBG("spi: unable to clear select pin for device: ");
      UART_DBG(spi_devices[i].dev_name);
      UART_DBG("\r\n");

      return -1;
    }
  }

  for(uint8_t i = 0; i < ARRAY_SIZE(spi_devices); i++) {
    if(spi_devices[i].dev_id == dev)
      if (pin_low(spi_devices[i].select_pin) < 0)
        return -1;

      UART_DBG("spi: SD slave select enabled for device: ");
      UART_DBG(spi_devices[i].dev_name);
      UART_DBG("\r\n");
  }
}

int8_t spi_device_disable(uint8_t dev) {
  /* set device disable */
   for(uint8_t i = 0; i < ARRAY_SIZE(spi_devices); i++) {
    if(spi_devices[i].dev_id == dev)
      if (pin_high(spi_devices[i].select_pin) < 0)
        return -1;

      UART_DBG("spi: SD slave select disabled for device: ");
      UART_DBG(spi_devices[i].dev_name);
      UART_DBG("\r\n");
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
