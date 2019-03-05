/*
 *	USC EE459 Spring 2019 Team 17 - Pins header
 */

#ifndef PINS_H
#define PINS_H

/* milliseconds */
#define PULSE_WIDTH_MS 100

//#include "debug.h"

#define ATMEGA2560

/* PORTX */
const enum {

#ifdef ATMEGA2560
  ATMEL_PORT_A,
#endif

#if defined ATMEGA2560 || defined ATMEGA328
  ATMEL_PORT_B,
  ATMEL_PORT_C,
  ATMEL_PORT_D,
#endif

#ifdef ATMEGA2560
  ATMEL_PORT_E,
  ATMEL_PORT_F,
  ATMEL_PORT_G,
  ATMEL_PORT_H,
  ATMEL_PORT_J,
  ATMEL_PORT_K,
  ATMEL_PORT_L,
#endif

  ATMEL_PORT_INVAL,
} amtel_328_port;

/* DDRX setting */
static const enum {
  ATMEL_INPUT,
  ATMEL_OUTPUT,
  ATMEL_DIR_INVAL,
} atmel_328_ddr_status;

/* Internal Pull-Ups */
static const enum {
  ATMEL_PULL_ENA,
  ATMEL_PULL_DIS,
  ATMEL_PULL_INVAL,
} atmel_328_pull_status;



/*
 *  ___ATMEGA328___
 *
 *	Pin assignments:
 *		1:	(Invalid) Reset
 *		2:	UART RX
 *		3:	UART TX
 *		4:
 *		5:
 *		6:
 *		7:	(Invalid) VCC
 *		8:	(Invalid) GND
 *		9:	(Invalid) XTAL
 *		10:
 *		11:
 *		12:
 *		13:
 *		14:
 *(B01)	15: LCD SPI Select
 *(D10)	16: SD Card SPI Select
 *(D11)	17: SPI MOSI
 *(D12)	18: SPI MISO
 *(D13)	19: SPI SCK
 *		20: (Invalid) AVCC
 *		21:	(Invalid) AREF
 *		22:	(Invalid) GND
 *		23:	(Analog)
 *		24:	(Analog)
 *		25:	(Analog)
 *		26:	(Analog)
 *		27:	(Analog)
 *		28:	(Analog)
 */

#if defined ATMEGA328

#define PIN_RX 2
#define PIN_TX 3
#define PIN_SS_SD 16
#define PIN_MOSI 17
#define PIN_MISO 18 /* input ! */
#define PIN_SCK 19

#elif defined ATMEGA2560

#define PIN_RX 2
#define PIN_TX 3
#define PIN_SS_SD 19
#define PIN_MOSI 21
#define PIN_MISO 22 /* input ! */
#define PIN_SCK 20

#endif

void initialize_pins();
int8_t pin_get(uint8_t pin);
int8_t pin_high(uint8_t pin);
int8_t pin_low(uint8_t pin);
void pulse_pin(uint8_t pin);
void cycle_pins();

#undef ATMEGA2560

#endif
