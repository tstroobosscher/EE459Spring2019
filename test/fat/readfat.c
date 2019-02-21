/*
 *	USC EE459 Spring 2019 Team 17 - FAT16/32 Test code
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "../../fat.h"
#include "../../utils.h"

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
	if((bin = fopen(*argv, "rb")) == NULL) {
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
				bs16.sectors_per_fat_lt32, bs16.number_of_fats);

			fseek(bin, (bs16.reserved_sectors - 1 + bs16.sectors_per_fat_lt32 * 
				bs16.number_of_fats) * bs16.sector_size, SEEK_CUR);

			for(int j = 0; j < bs16.root_dir_entries; j++) {
				fread(&e16, sizeof(struct FAT16Entry), 1, bin);
				fat16_dump_entry(&e16);
			}
		} else if((pt[i].partition_type == PARTITION_TYPE_FAT32_LT2G) || 
			(pt[i].partition_type == PARTITION_TYPE_FAT32_LBA)) {

			fseek(bin, SECTOR_SIZE_BYTES * pt[i].start_sector, SEEK_SET);
			fread(&bs32, sizeof(struct FAT32BootSector), 1, bin);
			fat32_dump_boot_sector(&bs32);

			dump_sector_addr(&bs32, &pt[i]);

			uint32_t fat_begin_lba = pt[i].start_sector + 
				bs32.reserved_sectors;
			uint32_t cluster_begin_lba = pt[i].start_sector + 
				bs32.reserved_sectors + 
				(bs32.number_of_fats * bs32.sectors_per_fat_32);
			uint8_t sectors_per_cluster = bs32.sectors_per_cluster;
			uint32_t root_dir_first_cluster = bs32.cluster_number_root_dir;

			uint32_t addr = (cluster_begin_lba + ((root_dir_first_cluster - 2) * sectors_per_cluster)) * bs32.sector_size;

			uint32_t lba_root_dir = fat32_calc_lba_from_cluster(cluster_begin_lba, sectors_per_cluster, root_dir_first_cluster);

			printf("    addr: 0x%0X\n", lba_root_dir);
			printf("    byte offset: 0x%08X\n", lba_root_dir * bs32.sector_size);

			fseek(bin, addr, SEEK_SET);
			for(int j = 0; j < 16; j++) {
				fread(&e32, sizeof(struct FAT32Entry), 1, bin);
				uint32_t pos = ftell(bin);
				//dump_bin(&e32, sizeof(struct FAT32Entry));
				if(fat32_dump_entry(&e32) < 0)
					break;
				uint32_t file_cluster = fat32_calc_first_cluster(e32.first_cluster_addr_high, e32.first_cluster_addr_low);
				fseek(bin, bs32.sector_size * fat32_calc_lba_from_cluster(cluster_begin_lba, sectors_per_cluster, file_cluster), SEEK_SET);
				uint8_t buf[100];
				fread(&buf, sizeof(buf), 1, bin);
				dump_bin(&buf, sizeof(buf));
				fseek(bin, pos, SEEK_SET);

				uint32_t res;
				uint32_t position = ftell(bin);
				fseek(bin, (fat_begin_lba) * bs32.sector_size + 32*e32.file_cluster, SEEK_SET);
				uint32_t buf[100];
				fread(&buf, sizeof(buf), 1, bin);
				dump_bin(&buf, sizeof(buf));
				fseek(bin, position, SEEK_SET);
			}
		}
		printf("\n");
	}

	printf("\nRoot directory read, now at 0x%lX\n", ftell(bin));
	fclose(bin);
	return 0;
}
