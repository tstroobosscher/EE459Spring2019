#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "utils.h"

/*
 *	EE459 Spring 2019 Team 17 - Main
 */

/*
 *	Pin assignments:
 *		1:
 *		2:
 *		3:
 *		4:
 *		5:
 *		6:
 *		7
 *		8
 *		9
 *		10
 *		11
 *		12
 *		13
 *		14
 *		15
 *		16
 *		17
 *		18
 *		19
 *		20
 *		21
 *		22
 *		23
 *		24
 *		25
 *		26
 *		27
 *		28
 */

int main() {

	initialize_pins();
	
	while(1)		
		cycle_pins();

	return(0);
}