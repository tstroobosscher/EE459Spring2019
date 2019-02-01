#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "utils.h"

/*
 *	EE459 Spring 2019 Team 17 - Main
 */

int main() {

	initialize_pins();
	
	while(1)		
		for(int i = 0; i < ARRAY_SIZE(atmega328_pins); i++)
			if(atmega328_pins[i].port_reg != ATMEL_PORT_INVAL)
				pulse_pin(i);

	return(0);
}