/*
 *	USC EE459 Spring 2019 Team 17 - Utils
 */

#include "utils.h"
#include "debug.h"

#ifdef DEBUG_86
#include <stdio.h>
#include <ctype.h>
#endif

#ifdef DEBUG_86

void dump_bin(void *buf, int size) {

	unsigned char *ptr = (unsigned char*) buf;
	unsigned char str[17];
	int i;

	printf("HEXDATA\n\n");

	for(i = 0; i < size; i++) {
		if(!(i % 16)) {
			if(i != 0)
				printf("  %s\n", str);
			/* output offset */
			printf("    %04X ", i);
		}

		printf(" %02X", ptr[i]);

		if(!isprint(ptr[i]))
			str[i % 16] = '.';
		else
			str[i % 16] = ptr[i];
		str[(i % 16) + 1] = '\0';
	}

	while((i % 16) != 0) {
		printf(" ");
		i++;
	}

	printf("  %s\n", str);
}

#endif

uint32_t bind_args(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3) {
	return (arg0 << 24) | (arg1 << 16) | (arg2 << 8) | (arg3);

}

int8_t byte_in_arr(uint8_t byte, void *buf, uint32_t size) {
	/*
	 *	return pos of byte in arr, 0 if not, logic is inconsistent but
	 *	its readable
	 */

	for(uint32_t i = 0; i < size; i++) {
		/* dereferencing a void pointer is undefined */
		uint8_t *ch = (uint8_t *) buf;
		if(byte == *ch)
			return 1;
		buf++;
	}
	return 0;
}

void dump_nbytes(uint8_t *buf, uint8_t nbytes) {
	for(uint8_t i = 0; i < nbytes; i++) {
		uint8_t hex[6];
		snprintf(hex, 6, "0x%02X ", buf[i]);
		uart_write_str(hex);
	}
	uart_write_str("\n\r");
}

void dump_byte(uint8_t byte) {
	uint8_t hex[6];
	snprintf(hex, 6, "0x%02X ", byte);
	uart_write_str(hex);
}

void trace() {
	uart_write_str("trace\r\n");
}

// void log(char *msg) {
// 	uart_write_str(msg);
// 	uart_write_str("\n\r");
// }

/* TODO: flush uart buffers */
