/*
 *	USC EE459 Spring 2019 Team 17 - Calc test cases
 */
#include <stdint.h>

#include "../../fat.h"


int main() {
	uint16_t high = 0x1234;
	uint16_t low = 0x5678;

	uint32_t res = fat32_calc_first_cluster(high, low);

	printf("Test Case: FAT32 Cluster Address Calc: ");
	if(res != 0x12345678)
		printf("Failure, res = %08X, exp = %08X\n", res, 0x12345678);
	else
		printf("Success, res = %08X\n", res);

	return 0;
}
