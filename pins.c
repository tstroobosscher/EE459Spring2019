/*
 *	USC EE459 Spring 2019 Team 17 - Pins
 */

#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "utils.h"

/* set pin behavior here (OUTPUT/INPUT), ENA/DIS internal pull ups */
static const struct atmel_328_pin {
	unsigned char pin_num;
	unsigned char port_reg;
	unsigned char port_num;
	unsigned char pin_dir;
	unsigned char pull_up;
} atmega328_pins[] = {
	0,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL, 	ATMEL_PULL_INVAL,
	1,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL, 	ATMEL_PULL_INVAL,
	2,	ATMEL_PORT_D,		0,	ATMEL_OUTPUT, 		ATMEL_PULL_DIS,
	3,	ATMEL_PORT_D,		1,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	4,	ATMEL_PORT_D,		2,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	5,	ATMEL_PORT_D,		3,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	6,	ATMEL_PORT_D,		4,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	7,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,	ATMEL_PULL_INVAL,
	8,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,	ATMEL_PULL_INVAL,
	9,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,	ATMEL_PULL_INVAL,
	10,	ATMEL_PORT_B,		7,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	11,	ATMEL_PORT_D,		5,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	12,	ATMEL_PORT_D,		6,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	13,	ATMEL_PORT_D,		7,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	14,	ATMEL_PORT_B,		0,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	15,	ATMEL_PORT_B,		1,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	16,	ATMEL_PORT_B,		2,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	17,	ATMEL_PORT_B,		3,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	18,	ATMEL_PORT_B,		4,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	19,	ATMEL_PORT_B,		5,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	20,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,	ATMEL_PULL_INVAL,
	21,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,	ATMEL_PULL_INVAL,
	22,	ATMEL_PORT_INVAL,	0,	ATMEL_DIR_INVAL,	ATMEL_PULL_INVAL,
	23,	ATMEL_PORT_C,		0,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	24,	ATMEL_PORT_C,		1,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	25,	ATMEL_PORT_C,		2,	ATMEL_OUTPUT, 		ATMEL_PULL_DIS,
	26,	ATMEL_PORT_C,		3,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	27,	ATMEL_PORT_C,		4,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
	28,	ATMEL_PORT_C,		5,	ATMEL_OUTPUT,		ATMEL_PULL_DIS,
};

/* export */
void initialize_pins() {
	/*
	 *	Here we are assuming that internal pull ups are off by default
	 */
	for(unsigned char i = 0; i < ARRAY_SIZE(atmega328_pins); i++) {
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
				/* set internal pull-ups if input pin */
				if(atmega328_pins[i].pull_up == ATMEL_PULL_ENA) {
					switch(atmega328_pins[i].port_reg) {
						case ATMEL_PORT_B	:
							PORTB |= (1 << atmega328_pins[i].port_num);
							break;
						case ATMEL_PORT_C	:
							PORTC |= (1 << atmega328_pins[i].port_num);
							break;
						case ATMEL_PORT_D	:
							PORTD |= (1 << atmega328_pins[i].port_num);
							break;
						default				:
							break;
					}
				}
			}
		}
	}
}

/* export */
char get_pin(unsigned char pin) {
	/*
	 *  return 1 if pin 1, 0 if 0, and -1 if error
	 *	########### Not tested yet ############
	 */
	if(pin < ARRAY_SIZE(atmega328_pins) && pin > 0)
		if(atmega328_pins[pin].port_reg != ATMEL_PORT_INVAL)
			if(atmega328_pins[pin].pin_dir == ATMEL_INPUT)
				switch(atmega328_pins[pin].port_reg) {
					case ATMEL_PORT_B	:
						return(
							(PINB & ~(1 << atmega328_pins[pin].port_num)) > 0);
					case ATMEL_PORT_C	:
						return(
							(PINC & ~(1 << atmega328_pins[pin].port_num)) > 0);
					case ATMEL_PORT_D	:
						return(
							(PIND & ~(1 << atmega328_pins[pin].port_num)) > 0);
					default				:
						return -1;
				}

	return -1;
}

/* export */
char assert_pin(unsigned char pin) {
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
char deassert_pin(unsigned char pin) {
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
void pulse_pin(unsigned char pin) {
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
	for(unsigned char i = 0; i < ARRAY_SIZE(atmega328_pins); i++)
		if(atmega328_pins[i].port_reg != ATMEL_PORT_INVAL)
			pulse_pin(i);
}
