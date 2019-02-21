/*
 *	USC EE459 Spring 2019 Team 17 - Pins header
 */

#ifndef PINS_H
#define PINS_H

/* milliseconds */
#define PULSE_WIDTH_MS 100

/* PORTX */
const enum {
	ATMEL_PORT_B,
	ATMEL_PORT_C,
	ATMEL_PORT_D,
	ATMEL_PORT_INVAL,
};

/* DDRX */
const enum {
	ATMEL_INPUT,
	ATMEL_OUTPUT,
	ATMEL_DIR_INVAL,
};

/* Internal Pull-Ups */
const enum {
	ATMEL_PULL_ENA,
	ATMEL_PULL_DIS,
	ATMEL_PULL_INVAL,
};

/*
 *	Pin assignments:
 *		1:	(Invalid) Reset
 *		2:	(UART) RX
 *		3:	(UART) TX
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
 *		15:
 *(D10)	16: (SPI /CS) (Only for slave communication)
 *(D11)	17: (SPI MOSI)
 *(D12)	18: (SPI MISO)
 *(D13)	19: (SPI SCK)
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

#define PIN_RX 2
#define PIN_TX 3
#define PIN_CS 16
#define PIN_MOSI 17
#define PIN_MISO 18 /* input ! */
#define PIN_SCK 19

void initialize_pins();
char get_pin(unsigned char pin);
char assert_pin(unsigned char pin);
char deassert_pin(unsigned char pin);
void pulse_pin(unsigned char pin);
void cycle_pins();

#endif
