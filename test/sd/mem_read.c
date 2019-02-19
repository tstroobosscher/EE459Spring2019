#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../fat.h"

int main() {
	FILE *bin = fopen("test.img", "r");

	struct PartitionTable pt[4];

	fseek(bin, 0x1BE, SEEK_SET);
	fread(pt, sizeof(struct PartitionTable), 4, bin);

	for(int i=0; i < 4; i++) {
		printf("Partition %d, type %02X\n", i, pt[i].partition_type);
		printf("	Start sector %08X, %d sectors long\n", pt[i].start_sector,
			pt[i].length_sectors);
	}

	fclose(bin);

	return 0;
}
