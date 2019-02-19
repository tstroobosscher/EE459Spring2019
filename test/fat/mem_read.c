#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../fat.h"

int main() {
	FILE *bin = fopen("test.img", "r");

	struct PartitionTable pt[PARTITION_TABLE_ENTRIES];

	fseek(bin, PARTITION_TABLE_OFFSET, SEEK_SET);
	fread(pt, sizeof(struct PartitionTable), PARTITION_TABLE_ENTRIES, bin);

	for(int i=0; i < PARTITION_TABLE_ENTRIES; i++) {
		fat_dump_partition_table(&pt[i]);
	}

	fclose(bin);

	return 0;
}
