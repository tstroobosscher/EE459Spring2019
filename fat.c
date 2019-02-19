/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include "fat.h"

#ifdef DEBUG

void fat_dump_partition_table(struct PartitionTable *pt) {
	printf("Partition entry found, first byte: %02X\n", pt->boot_byte);
	printf("	Start address in CHS: %02X:%02X:%02X\n", pt->start_chs[0], 
		pt->start_chs[1], pt->start_chs[2]);
	uint8_t start_chs[3];
	uint8_t partition_type;
	uint8_t end_chs[3];
	uint32_t start_sector;
	uint32_t length_sectors;
}

#endif
