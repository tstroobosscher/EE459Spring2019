#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../fat16.h"

int main() {
	FILE *bin = fopen("test.img", "r");

	struct PartitionTable pt[PARTITION_TABLE_ENTRIES];
	struct FAT16BootSector bs;
	struct FAT16Entry e;

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

				printf("\nNow at 0x%lX, sector size %d, FAT size %d sectors, "
					"%d FATs\n\n", ftell(bin), bs.sector_size, 
					bs.fat_sectors, bs.number_of_fats);

				fseek(bin, (bs.reserved_sectors - 1 + bs.fat_sectors * 
					bs.number_of_fats) * bs.sector_size, SEEK_CUR);

				for(int j = 0; j < bs.root_dir_entries; j++) {
					fread(&e, sizeof(struct FAT16Entry), 1, bin);
					fat16_dump_entry(&e);
				}
			}
			printf("\n");
	}

	printf("\nRoot directory read, now at 0x%lX\n", ftell(bin));
	fclose(bin);
	return 0;
}
