/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include "fat.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#ifdef DEBUG

void fat_dump_partition_table(struct PartitionTable *pt) {
	printf("Partition entry found, first byte: %02X\n", pt->boot_byte);
	printf("	Start address in CHS: %02X:%02X:%02X\n", pt->start_chs[0], 
		pt->start_chs[1], pt->start_chs[2]);
	printf("	Partition type: ");
	switch(pt->partition_type) {
		case PARTITION_TYPE_UNUSED :
			printf("unused\n");
			break;
		case PARTITION_TYPE_FAT12 :
			printf("FAT12\n");
			break;
		case PARTITION_TYPE_FAT16_LT32M :
			printf("FAT16, less than 32 MiB\n");
			break;
		case PARTITION_TYPE_EXTENDED :
			printf("extended\n");
			break;
		case PARTITION_TYPE_FAT16_GT32M :
			printf("FAT16, greater than 32 MiB\n");
			break;
		case PARTITION_TYPE_FAT32_LT2G :
			printf("FAT32, less than 2 GiB\n");
			break;
		case PARTITION_TYPE_FAT32_LBA :
			printf("FAT32, LBA\n");
			break;
		case PARTITION_TYPE_FAT16_LBA :
			printf("FAT16, LBA\n");
			break;
		case PARTITION_TYPE_EXTENDED_LBA :
			printf("extended, LBA\n");
			break;
		default:
			printf("unknown partition type\n");
			break;
	}
	printf("	End address in CHS: %02X:%02X:%02X\n", pt->end_chs[0], 
		pt->end_chs[1], pt->end_chs[2]);
	printf("	Relative LBA address: %08X, %d sectors long\n", 
		pt->start_sector, pt->length_sectors);
}

void fat_dump_boot_sector(struct PartitionTable *pt) {
	
}

#endif
