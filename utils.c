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
