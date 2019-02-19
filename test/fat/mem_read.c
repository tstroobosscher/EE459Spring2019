#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../fat.h"

int main() {
	FILE *bin = fopen("test.img", "r");

	struct PartitionTable pt[PARTITION_TABLE_ENTRIES];
	struct FAT16BootSector bs;

	fseek(bin, PARTITION_TABLE_OFFSET, SEEK_SET);
	fread(pt, sizeof(struct PartitionTable), PARTITION_TABLE_ENTRIES, bin);

	for(int i=0; i < PARTITION_TABLE_ENTRIES; i++) {
		fat_dump_partition_table(&pt[i]);
		if((pt[i].partition_type == PARTITION_TYPE_FAT16_LT32M) || 
			(pt[i].partition_type == PARTITION_TYPE_FAT16_GT32M) ||
			(pt[i].partition_type == PARTITION_TYPE_FAT16_LBA)) {
				fseek(bin, SECTOR_SIZE_BYTES * pt[i].start_sector, SEEK_SET);
				fread(&bs, sizeof(struct FAT16BootSector), 1, bin);
				fat16_dump_boot_sector(&bs);
			}
	}

	fclose(bin);

	return 0;
}
