/*
 *	USC EE459 Spring 2019 Team 17 - FAT Header
 */

#ifndef FAT_H
#define FAT_H

#include <stdint.h>

/*
 *	Partition table
 *
 *	Privides a mapping for every disk partition on the physical drive.
 *	Starts at absolute address: 0x1BE, contains 4 entries sequentially.
 *	Offset				Description 					Size (Bytes)
 *	0x00				0x80 if active, 0 otherwise		1
 *	0x01				start address (CHS)				3
 *	0x04				type, see below					1
 *	0x05				end address (CHS)				3
 *	0x80				relative offset (LBA)			4
 *	0x0C				size (sectors)					4
 */

struct PartitionTable {
	uint8_t boot_byte;
	uint8_t start_chs[3];
	uint8_t partition_type;
	uint8_t end_chs[3];
	uint32_t start_sector;
	uint32_t length_sectors;
}__attribute((packed));


/*
 *	Partition types
 *	0		unused
 *	1		FAT12
 *	4		FAT16 <= 32MiB
 *	5		extended partitions
 *	6		FAT16 > 32MiB
 *	11		FAT32 <= 2GiB
 *	12		FAT32, LBA, no size constraints
 *	14		FAT16, LBA
 *	15		extended partition, LBA
 */

#define PARTITION_TYPE_UNUSED 0
#define PARTITION_TYPE_FAT12 1
#define PARTITION_TYPE_FAT16_LT32M 4
#define PARTITION_TYPE_EXTENDED 5
#define PARTITION_TYPE_FAT16_GT32M 6
#define PARTITION_TYPE_FAT32_LT2G 11
#define PARTITION_TYPE_FAT32_LBA 12
#define PARTITION_TYPE_FAT16_LBA 
#define PARTITION_TYPE_EXTENDED_LBA 15
 
/*
 *	Boot sector
 *	Offset 			Description 						Size (Bytes)
 *	0x000			Intel 80x86 Jump					3
 *	0x003			OEM Name							8
 *	0x00B			Sector size (Bytes)					2
 *	0x00D			Sectors per cluster					1
 *	0x00E			Reserved sectors (boot incl)		2
 *	0x010			Number of FATs						1
 *	0x011			Number of root entries				2
 *	0x013			Number of sectors (<= 32MiB)		2
 *	0x015			Media descriptor (removable?)		1
 *	0x016			Number of sectors for FAT 			2
 *	0x018			Number of sectors per track (CHS)	2
 *	0x01A			Number of heads (CHS)				2
 *	0x01C			Number of hidden sectors 			4
 *	0x020			Total number of sectors (> 32MiB)	4
 *	0x024			FAT32: Number of sectors 			4
 *	0x028			FAT32: Flags for FAT 				2
 *	0x02A			FAT32: File system version number 	2
 *	0x02C			FAT32: Cluster number for root dir 	4
 *	0x030 			FAT32: Sector number for FSInfo 	2
 *	0x032 			FAT32: Sector number for backup 	2
 *	0x034 			FAT32: Reserved for future			12
 *	0x040			Drive number (0x024 FAT12/16)		1
 *	0x041 			Current head (0x025 FAT12/16)		1
 *	0x042 			Boot Signature (0x026 FAT12/16)		1
 *	0x043 			Volume ID (0x027 FAT12/16)			4
 *	0x047			Volume label (0x02B FAT12/16)		11
 *	0x052 			File system type (0x036 FAT12/16)	8
 *	0x08A			Boot code (0x03E FAT12/16)			372 (448 FAT12/16)
 *	0x1FE 			Boot sector signature (0x55AA)		2
 */

struct FAT16BootSector {
	uint8_t jmp[3];
	uint8_t oem[8];
	uint16_t sector_size;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t number_of_fats;
	uint16_t root_dir_entries;
	uint16_t total_sectors_16;
	uint8_t media_descriptor;
	uint16_t fat_sectors;
	uint16_t sectors_per_track;
	uint16_t number_of_heads;
	uint32_t hidden_sectors;
	uint32_t total_sectors_32;

	uint8_t drive_number;
	uint8_t current_head;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fs_type[8];
	uint8_t boot_code[448];
	uint16_t boot_sector_signature;
}__attribute((packed));

#endif
