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
		cycle_pins();

	return(0);
}