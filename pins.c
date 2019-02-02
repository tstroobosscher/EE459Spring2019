/*
 *	EE459 Team 17 - Pins
 */

#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "utils.h"

/* PORTX */
static const enum {
	ATMEL_PORT_B,
	ATMEL_PORT_C,
	ATMEL_PORT_D,
	ATMEL_PORT_INVAL,
};

/* DDR */
static const enum {
	ATMEL_INPUT,
	ATMEL_OUTPUT,
	ATMEL_DIR_INVAL,
};


/* set pin behavior here (OUTPUT/INPUT) */
static const struct atmel_328_pin {
	unsigned char pin_num;
	unsigned char port_reg;
	unsigned char port_num;
	unsigned char pin_dir;
} atmega328_pins[] = {
	0,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	1,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	2,	ATMEL_PORT_D,		0,	ATMEL_OUTPUT,
	3,	ATMEL_PORT_D,		1,	ATMEL_OUTPUT,
	4,	ATMEL_PORT_D,		2,	ATMEL_OUTPUT,
	5,	ATMEL_PORT_D,		3,	ATMEL_OUTPUT,
	6,	ATMEL_PORT_D,		4,	ATMEL_OUTPUT,
	7,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	8,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	9,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	10,	ATMEL_PORT_B,		7,	ATMEL_OUTPUT,
	11,	ATMEL_PORT_D,		5,	ATMEL_OUTPUT,
	12,	ATMEL_PORT_D,		6,	ATMEL_OUTPUT,
	13,	ATMEL_PORT_D,		7,	ATMEL_OUTPUT,
	14,	ATMEL_PORT_B,		0,	ATMEL_OUTPUT,
	15,	ATMEL_PORT_B,		1,	ATMEL_OUTPUT,
	16,	ATMEL_PORT_B,		2,	ATMEL_OUTPUT,
	17,	ATMEL_PORT_B,		3,	ATMEL_OUTPUT,
	18,	ATMEL_PORT_B,		4,	ATMEL_OUTPUT,
	19,	ATMEL_PORT_B,		5,	ATMEL_OUTPUT,
	20,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	21,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	22,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,
	23,	ATMEL_PORT_C,		0,	ATMEL_OUTPUT,
	24,	ATMEL_PORT_C,		1,	ATMEL_OUTPUT,
	25,	ATMEL_PORT_C,		2,	ATMEL_OUTPUT, 
	26,	ATMEL_PORT_C,		3,	ATMEL_OUTPUT,
	27,	ATMEL_PORT_C,		4,	ATMEL_OUTPUT,
	28,	ATMEL_PORT_C,		5,	ATMEL_OUTPUT,
};

/* export */
void initialize_pins() {
	for(int i = 0; i < ARRAY_SIZE(atmega328_pins); i++) {
		if(atmega328_pins[i].port_reg != ATMEL_PORT_INVAL) {
			if(atmega328_pins[i].pin_dir == ATMEL_OUTPUT) {
				switch(atmega328_pins[i].port_reg) {
					case ATMEL_PORT_B	:
						DDRB |= (1 << atmega328_pins[i].port_num);
						break;
					case ATMEL_PORT_C	:
						DDRC |= (1 << atmega328_pins[i].port_num);
						break;
					case ATMEL_PORT_D	:
						DDRD |= (1 << atmega328_pins[i].port_num);
						break;
					default				:
						break;
				}
			} else if(atmega328_pins[i].pin_dir == ATMEL_INPUT) {
				switch(atmega328_pins[i].port_reg) {
					case ATMEL_PORT_B	:
						DDRB &= ~(1 << atmega328_pins[i].port_num);
						break;
					case ATMEL_PORT_C	:
						DDRC &= ~(1 << atmega328_pins[i].port_num);
						break;
					case ATMEL_PORT_D	:
						DDRD &= ~(1 << atmega328_pins[i].port_num);
						break;
					default				:
						break;
				}
			}
		}
	}
}

/* export */
char assert_pin(char pin) {
	/*
	 *	need to search through the pin database to map ports to physical pins
	 *	we can either iterate through the db and look for the pin_num,
	 *	which would cost us CPU cycles, or we could map the pin number to the
	 *	index of the db array. 0 is always invalid, no 0 pin
	 *	array size is thus the maximum pin number, plus one for the 0-index.
	 * 	therefore a pin request of ARRAY_SIZE is invalid
	 */
	if(pin < ARRAY_SIZE(atmega328_pins) && pin > 0)
		if(atmega328_pins[pin].port_reg != ATMEL_PORT_INVAL)
			if(atmega328_pins[pin].pin_dir == ATMEL_OUTPUT)
				switch(atmega328_pins[pin].port_reg) {
					case ATMEL_PORT_B	:
						PORTB |= (1 << atmega328_pins[pin].port_num);
						return 0;
					case ATMEL_PORT_C	:
						PORTC |= (1 << atmega328_pins[pin].port_num);
						return 0;
					case ATMEL_PORT_D	:
						PORTD |= (1 << atmega328_pins[pin].port_num);
						return 0;
					default				:
						return -1;
				}

	return -1;
}

/* export */
char deassert_pin(char pin) {
	if(pin < ARRAY_SIZE(atmega328_pins)  || pin > 0)
		if(atmega328_pins[pin].port_reg != ATMEL_PORT_INVAL)
			if(atmega328_pins[pin].pin_dir == ATMEL_OUTPUT)
				switch(atmega328_pins[pin].port_reg) {
					case ATMEL_PORT_B	:
						PORTB &= ~(1 << atmega328_pins[pin].port_num);
						return 0;
					case ATMEL_PORT_C	:
						PORTC &= ~(1 << atmega328_pins[pin].port_num);
						return 0;
					case ATMEL_PORT_D	:
						PORTD &= ~(1 << atmega328_pins[pin].port_num);
						return 0;
					default				:
						return -1;
				}

	return -1;
}

/* export */
void pulse_pin(char pin) {
	/*
	 *	assert the pin, wait, deassert
	 *	delay time must be known at compile time
	 */
	assert_pin(pin);
	DELAY_MS(PULSE_WIDTH_MS);
	deassert_pin(pin);
}

/* export */
void cycle_pins() {
	for(int i = 0; i < ARRAY_SIZE(atmega328_pins); i++)
		if(atmega328_pins[i].port_reg != ATMEL_PORT_INVAL)
			pulse_pin(i);
}
