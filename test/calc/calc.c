/*
 *	USC EE459 Spring 2019 Team 17 - Calc test cases
 */
#include <stdint.h>
#include <stdio.h>

// #include "../../fat.h"
// #include "../../utils.h"

#define BUF_SIZE 32

void drop_whitespace(char *str) {
	int count = 0;

	for(int i = 0; i < strlen(str); i++) {
		if(!isspace(str[i]))
			str[count++] = str[i];
	}
		
	str[count] = '\0';
}

int main() {
	// uint16_t high = 0x1234;
	// uint16_t low = 0x5678;

	// uint32_t res = fat32_calc_first_cluster(high, low);

	// printf("Test Case: FAT32 Cluster Address Calc: ");
	// if(res != 0x12345678)
	// 	printf("Failure, res = %08X, exp = %08X\n", res, 0x12345678);
	// else
	// 	printf("Success, res = %08X\n", res);

	// uint8_t arg0 = 0x12;
	// uint8_t arg1 = 0x34;
	// uint8_t arg2 = 0x56;
	// uint8_t arg3 = 0x78;
	
	// res = bind_args(arg0, arg1, arg2, arg3);

	// printf("Test Case: SD SPI Long Arg Calc: ");
	// if(res != 0x12345678)
	// 	printf("Failure, res = %08X, exp = %08X\n", res, 0x12345678);
	// else
	// 	printf("Success, res = %08X\n", res);

	// uint8_t buf[BUF_SIZE] = {
	// 	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	// 	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	// 	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	// 	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	// };

	// res = (uint32_t) byte_in_arr(0x14, buf, BUF_SIZE);
	
	// printf("Test Case: Byte in Array: ");
	// if(res != 0x00000000)
	// 	printf("Failure, res = %08X, exp = %08X\n", res, 0x00000000);
	// else
	// 	printf("Success, res = %08X\n", res);

	// buf[0x14] = 0x15;

	// res = (uint32_t) byte_in_arr(0x14, buf, BUF_SIZE);
	
	// printf("Test Case: Byte not in Array: ");
	// if(res != -1)
	// 	printf("Failure, res = %08X, exp = %08X\n", res, -1);
	// else
	// 	printf("Success, res = %08X\n", res);

	char str[] = "this string has whitespace";
	char res_str[] = "thisstringhaswhitespace";
	printf("Test Case: Stripping Whitespace: ");
	drop_whitespace(str);
	if(strcmp(str, res_str))
		printf("Failure, res = \"%s\", exp = \"%s\"\n", str, res_str);
	else
		printf("Success, res = \"%s\"\n", str);


	return 0;
}
