#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "../../fat.h"

extern int errno;

void usage() {
	printf("readfat: usage: readfat <image file>\n");
}

int main(int argc, const char **argv) {

	const char *pname = *argv;
	argv++;

	if(argc != 2) {
		printf("readfat: incorrect number of arguments\n");
		usage();
		return -1;
	}

	FILE *bin;
	if((bin = fopen(*argv, "r")) == NULL) {
		perror(pname);
		return -1;
	}

	struct PartitionTable pt[PARTITION_TABLE_ENTRIES];
	struct FAT16BootSector bs16;
	struct FAT32BootSector bs32;
	struct FAT16Entry e16;
	struct FAT32Entry e32;

	fseek(bin, PARTITION_TABLE_OFFSET, SEEK_SET);
	fread(pt, sizeof(struct PartitionTable), PARTITION_TABLE_ENTRIES, bin);

	fat_dump_sizes();

	for(int i=0; i < PARTITION_TABLE_ENTRIES; i++) {
		fat_dump_partition_table(&pt[i]);
		if((pt[i].partition_type == PARTITION_TYPE_FAT16_LT32M) || 
			(pt[i].partition_type == PARTITION_TYPE_FAT16_GT32M) ||
			(pt[i].partition_type == PARTITION_TYPE_FAT16_LBA)) {
		
			fseek(bin, SECTOR_SIZE_BYTES * pt[i].start_sector, SEEK_SET);
			fread(&bs16, sizeof(struct FAT16BootSector), 1, bin);
			fat16_dump_boot_sector(&bs16);

			printf("\nNow at 0x%lX, sector size %d, FAT size %d sectors, "
				"%d FATs\n\n", ftell(bin), bs16.sector_size, 
				bs16.fat_sectors, bs16.number_of_fats);

			fseek(bin, (bs16.reserved_sectors - 1 + bs16.fat_sectors * 
				bs16.number_of_fats) * bs16.sector_size, SEEK_CUR);

			for(int j = 0; j < bs16.root_dir_entries; j++) {
				fread(&e16, sizeof(struct FAT16Entry), 1, bin);
				fat16_dump_entry(&e16);
			}
		} else if((pt[i].partition_type == PARTITION_TYPE_FAT32_LT2G) || 
			(pt[i].partition_type == PARTITION_TYPE_FAT32_LBA)) {

			fseek(bin, SECTOR_SIZE_BYTES * pt[i].start_sector, SEEK_SET);
			fread(&bs32, sizeof(struct FAT32Entry), 1, bin);
		}
		printf("\n");
	}

	printf("\nRoot directory read, now at 0x%lX\n", ftell(bin));
	fclose(bin);
	return 0;
}
