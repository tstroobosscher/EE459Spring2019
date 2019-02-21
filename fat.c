/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include "fat.h"
#include "debug.h"

#ifdef DEBUG_86
#include <stdio.h>
#endif

uint32_t fat32_calc_first_cluster(uint16_t high, uint16_t low) {
	return (high << 16) | (low);
}

uint32_t fat32_calc_lba_from_cluster(uint32_t cluster_begin_lba, 
	uint32_t sectors_per_cluster, uint32_t cluster_number) {
	return cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster;
}

#ifdef DEBUG_86

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
	printf("	Relative LBA address: 0x%08X, %u sectors long\n", 
		pt->start_sector, pt->length_sectors);
}

void fat16_dump_boot_sector(struct FAT16BootSector *bs) {
	printf("FAT16 boot sector found, jump code: %02X:%02X:%02X\n", bs->jmp[0], 
		bs->jmp[1], bs->jmp[2]);
	printf("	OEM code: [%.8s]\n", bs->oem);
	printf("	Sector size (Bytes): %u\n", bs->sector_size);
	printf("	Sectors per cluster: %u\n", bs->sectors_per_cluster);
	printf("	Number of reserved sectors: %u\n", bs->reserved_sectors);
	printf("	Number of FATs: %u\n", bs->number_of_fats);
	printf("	Number of root directory entries: %u\n", bs->root_dir_entries);
	printf("	Total number of sectors (16): %u\n", bs->total_sectors_16);
	printf("	Media descriptor: 0x%02X\n", bs->media_descriptor);
	printf("	Sectors per FAT (N.A. 32): %u\n", bs->sectors_per_fat_lt32);
	printf("	Number of sectors per track: %u\n", bs->sectors_per_track);
	printf("	Number of heads: %u\n", bs->number_of_heads);
	printf("	Number of hidden sectors: %u\n", bs->hidden_sectors);
	printf("	Total number of sectors (32): %u\n", bs->total_sectors_32);
	printf("	Drive number: 0x%02X\n", bs->drive_number);
	printf("	Current head: 0x%02X\n", bs->current_head);
	printf("	Boot signature: 0x%02X\n", bs->boot_signature);
	printf("	Volume ID: 0x%08X\n", bs->volume_id);
	printf("	Volume label: [%.11s]\n", bs->volume_label);
	printf("	Filesystem type: [%.8s]\n", bs->fs_type);
	printf("	Boot sector signature: 0x%04X\n", bs->boot_sector_signature);
}

void fat32_dump_boot_sector(struct FAT32BootSector *bs) {
	printf("FAT32 boot sector found, jump code: %02X:%02X:%02X\n", bs->jmp[0], 
		bs->jmp[1], bs->jmp[2]);
	printf("	OEM code: [%.8s]\n", bs->oem);
	printf("	Sector size (Bytes): %u\n", bs->sector_size);
	printf("	Sectors per cluster: %u\n", bs->sectors_per_cluster);
	printf("	Number of reserved sectors: %u\n", bs->reserved_sectors);
	printf("	Number of FATs: %u\n", bs->number_of_fats);
	printf("	Media descriptor: 0x%02X\n", bs->media_descriptor);
	printf("	Sectors per FAT (N.A. 32): %u\n", bs->sectors_per_fat_lt32);
	printf("	Number of sectors per track: %u\n", bs->sectors_per_track);
	printf("	Number of heads: %u\n", bs->number_of_heads);
	printf("	Number of hidden sectors: %u\n", bs->hidden_sectors);
	printf("	Total number of sectors (32): %u\n", bs->total_sectors_32);
	printf("	FAT32: Sectors per FAT: %u\n", bs->sectors_per_fat_32);
	printf("	FAT32: fat flags: %04X\n", bs->fat_flags);
	printf("	FAT32: fs version number: %u\n", bs->fs_version_number);
	printf("	FAT32: cluster address of root dir: %u\n", 
		bs->cluster_number_root_dir);
	printf("	FAT32: sector number of FSInfo: %04X\n", 
		bs->sector_number_fsinfo);
	printf("	FAT32: sector number of backup: %04X\n", 
		bs->sector_number_backup);
	printf("	Drive number: 0x%02X\n", bs->drive_number);
	printf("	Current head: 0x%02X\n", bs->current_head);
	printf("	Boot signature: 0x%02X\n", bs->boot_signature);
	printf("	Volume ID: 0x%08X\n", bs->volume_id);
	printf("	Volume label: [%.11s]\n", bs->volume_label);
	printf("	Filesystem type: [%.8s]\n", bs->fs_type);
	printf("	Boot sector signature: 0x%04X\n", bs->boot_sector_signature);
}

void fat16_dump_entry(struct FAT16Entry *e) {
	switch(e->filename[0]) {
		case FILENAME_NEVER_USED :
			/* unused entry */
			return;
		case FILENAME_FILE_DELETED :
			printf("FAT16 Entry found, Deleted file: [?%.7s.%.3s]\n", 
				e->filename + 1, e->filename_ext);
			return;
		case FILENAME_FIRST_CHAR_E5 :
			printf("FAT16 Entry found, File starting with 0xE5: "
				"[%c%.7s.%.3s]\n", 0xE5, 
				e->filename + 1, e->filename_ext);
			break;
		case FILENAME_DIRECTORY :
			printf("FAT16 Entry found, Directory: [%.8s.%.3s]\n", e->filename, 
				e->filename_ext);
			break;
		default:
			printf("FAT16 Entry found, File: [%.8s.%.3s]\n", e->filename, 
				e->filename_ext);
			break;
	}

	printf("    Modified: %04d-%02d-%02d %02d:%02d:%02d\n", 
		(1980 + (e->date_last_modified >> 9)), 
		((e->date_last_modified >> 5) & 0xF), 
		(e->date_last_modified & 0x1F), 
		(e->time_last_modified >> 11), 
		((e->time_last_modified >> 5) & 0x3F), 
		(e->time_last_modified & 0x1F));
	printf("    Start: [%04X]", e->cluster_start);
	printf("    Size: %u\n", e->file_size);
}

char fat32_dump_entry(struct FAT32Entry *e) {
	switch(e->filename[0]) {
		case FILENAME_NEVER_USED :
			/* end of directory */
			return -1;
		case FILENAME_FILE_DELETED :
			printf("Deleted FAT32 Entry found: [?%.7s.%.3s]\n", 
				e->filename + 1, e->filename_ext);
			return 0;
		case FILENAME_FIRST_CHAR_E5 :
			/* Regular file, proceed normally */
			printf("FAT32 Entry found, Starting with 0xE5: [%c%.7s.%.3s]\n", 
				0xE5, e->filename + 1, e->filename_ext);
			break;
		default:
			/* LFN's not yet implemented */
			if((e->file_attr & 0x0F) == 0x0F) {
				printf("FAT32 Entry found: LFN\n");
				printf("    LFN's not yet implemented :(\n");
				return 0;
			}
			else
				/* Regular file, proceed normally */
				printf("FAT32 Entry found: [%.8s.%.3s]\n", e->filename, 
					e->filename_ext);
			break;
	}

	if(e->file_attr & FILE_ATTR_RO)
		printf("    Attribute: READONLY\n");
	if(e->file_attr & FILE_ATTR_HIDDEN)
		printf("    Attribute: HIDDEN\n");
	if(e->file_attr & FILE_ATTR_SYSTEM)
		printf("    Attribute: SYSTEM\n");
	if(e->file_attr & FILE_ATTR_LABEL)
		printf("    Attribute: LABEL\n");
	if(e->file_attr & FILE_ATTR_SUBDIR)
		printf("    Attribute: SUBDIR\n");
	if(e->file_attr & FILE_ATTR_ARCHIVE)
		printf("    Attribute: ARCHIVE\n");

	printf("    Starting cluter address: 0x%08X\n", 
		fat32_calc_first_cluster(e->first_cluster_addr_high, 
			e->first_cluster_addr_low));
	printf("    File size: %u Bytes\n", e->file_size);

	return 0;
}

void dump_sector_addr(struct FAT32BootSector *bs, struct PartitionTable *pt) {
	uint32_t fat_begin_lba = pt->start_sector + 
		bs->reserved_sectors;
	uint32_t cluster_begin_lba = pt->start_sector + 
		bs->reserved_sectors + 
		(bs->number_of_fats * bs->sectors_per_fat_32);
	uint8_t sectors_per_cluster = bs->sectors_per_cluster;
	uint32_t root_dir_first_cluster = bs->cluster_number_root_dir;

	printf("    fat_begin_lba: 0x%08X, byte offset: %0X\n", fat_begin_lba, 
		fat_begin_lba * bs->sector_size	);
	printf("    cluster_begin_lba: 0x%08X, byte offset: %0X\n", 
		cluster_begin_lba, cluster_begin_lba * bs->sector_size	);
	printf("    sectors_per_cluster: %u\n", sectors_per_cluster);
	printf("    root_dir_first_cluster: 0x%08X\n", root_dir_first_cluster);
}

void fat_dump_sizes() {
	printf("fat16 boot sector size : %u\n", 
		(int) sizeof(struct FAT16BootSector));
	printf("fat32 boot sector size : %u\n", 
		(int) sizeof(struct FAT32BootSector));
	printf("fat16 entry size : %u\n", (int) sizeof(struct FAT16Entry));
	printf("fat32 entry size : %u\n", (int) sizeof(struct FAT32Entry));
}

#endif
