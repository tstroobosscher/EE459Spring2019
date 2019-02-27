/*
 *	USC EE459 Spring 2019 Team 17 - ELM327 Communication
 */

#include "elm.h"

/*
 *	The idea for this file is to provide a layer of abstraction between the 
 *	obd code and the physical chip. all this layer needs to do to provide
 *	byte level read and write to the obd code. This will make testing between 
 *	the device and the laptops easier since all we need to recode is the byte
 *	oriented reads and writes.
 *	
 *	All the obd code should be doing is passing in argument strings (ex. "0100")
 *	and reading the response bytes. The ELM code has to do all the formatting 
 *	to convert from UART ascii to actual bytes of data.
 */

int8_t initialize_elm() {

	

	return 0;
}
