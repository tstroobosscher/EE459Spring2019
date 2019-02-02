/*
 *	EE459 Team 17 - Pins header
 */

#ifndef PINS_H
#define PINS_H

/* milliseconds */
#define PULSE_WIDTH_MS 100

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
 *		16:
 *		17:
 *		18:
 *		19:
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

void initialize_pins();
char assert_pin(char pin);
char deassert_pin(char pin);
void pulse_pin(char pin);
void cycle_pins();

#endif