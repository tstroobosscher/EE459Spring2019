#ifndef PINS_H
#define PINS_H

/* milliseconds */
#define PULSE_WIDTH_MS 100

/*
 *	Pin assignments:
 *		1:	Reset
 *		2:	(UART) RX
 *		3:	(UART) TX
 *		4:
 *		5:
 *		6:
 *		7:	VCC
 *		8:	GND
 *		9:	XTAL
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
 *		20: AVCC
 *		21:	AREF
 *		22:	GND
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
int assert_pin(int pin);
int deassert_pin(int pin);
void pulse_pin(int pin);
void cycle_pins();

#endif