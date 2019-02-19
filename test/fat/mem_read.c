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
		fat_dump_partition_table(&pt[i]);
	}

	fclose(bin);

	return 0;
}
