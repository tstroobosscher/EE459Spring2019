/*
 *	USC EE459 Spring 2019 Team 17 - FAT Header
 */

#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#include "debug.h"
#include "io.h"
#include "sd.h"
#include "list.h"

/* always found at this address */
#define PARTITION_TABLE_OFFSET 0x1BE

/* always has 4 entries */
#define PARTITION_TABLE_ENTRIES 4

/* SD standard */
#define SECTOR_SIZE_BYTES 512

/* Number of bytes in an FAT entry */
#define FAT_ENTRY_SIZE 4

/*
 *	lets just get the relevant information from the fat structs, save to the
 *	context, and then discard everything we don't need (CHS, timestamps,
 *etc)
 */

/*
 *	Logical Block Addressing Note:
 *
 *	LBA for SSD's just numbers the sectors sequentially starting at 0
 *	So, for example, the MBR sector is just LBA = 0
 */

/*
 *	Partition table
 *
 *	Privides a mapping for every disk partition on the physical drive.
 *	Starts at absolute address: 0x1BE, contains 4 entries sequentially.
 *	Offset				Description
 *Size (Bytes) 0x00				0x80 if active, 0 otherwise
 *1 0x01				start address (CHS)
 *3 0x04				type, see below
 *1 0x05				end address (CHS)
 *3 0x80				relative offset (LBA)
 *4 0x0C				size (sectors)
 *4
 */

struct PartitionTable {
  uint8_t boot_byte;
  uint8_t start_chs[3];
  uint8_t partition_type;
  uint8_t end_chs[3];
  uint32_t start_sector; /* LBA begin */
  uint32_t length_sectors;
} __attribute((packed));

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
#define PARTITION_TYPE_FAT16_LBA 14
#define PARTITION_TYPE_EXTENDED_LBA 15

/*
 *	Boot sector
 *	Offset 			Description
 *Size (Bytes) 0x000			Intel 80x86 Jump
 *3 0x003			OEM Name
 *8 0x00B			Sector size (Bytes)
 *2 0x00D			Sectors per cluster
 *1 0x00E			Reserved sectors (boot incl)		2 0x010
 *Number of FATs						1 0x011
 *Number of root entries				2 0x013
 *Number of sectors (<= 32MiB)		2 0x015			Media
 *descriptor (removable?)		1 0x016			Number of
 *sectors per FAT (!32)		2 0x018			Number of
 *sectors per track (CHS)	2 0x01A			Number of heads (CHS)
 *2 0x01C			Number of hidden sectors
 *4 0x020			Total number of sectors (> 32MiB)	4 0x024
 *FAT32: Sectors per fat (32)			4 0x028
 *FAT32: Flags for FAT 				2 0x02A
 *FAT32: File system version number 	2 0x02C			FAT32:
 *Cluster number for root dir 	4
 *	0x030 			FAT32: Sector number for FSInfo 	2
 *	0x032 			FAT32: Sector number for backup 	2
 *	0x034 			FAT32: Reserved for future 12
 *	0x040			Drive number (0x024 FAT12/16)		1
 *	0x041 			Current head (0x025 FAT12/16)		1
 *	0x042 			Boot Signature (0x026 FAT12/16)		1
 *	0x043 			Volume ID (0x027 FAT12/16) 4
 *	0x047			Volume label (0x02B FAT12/16)		11
 *	0x052 			File system type (0x036 FAT12/16)	8
 *	0x08A			Boot code (0x03E FAT12/16)			372
 *(448 FAT12/16) 0x1FE 			Boot sector signature (0x55AA)
 *2
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
  uint16_t sectors_per_fat_lt32;
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
} __attribute((packed));

struct FAT32BootSector {
  uint8_t jmp[3];
  uint8_t oem[8];
  uint16_t sector_size;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t number_of_fats;
  uint16_t root_dir_entries;
  uint16_t total_sectors_16;
  uint8_t media_descriptor;
  uint16_t sectors_per_fat_lt32;
  uint16_t sectors_per_track;
  uint16_t number_of_heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_32;

  uint32_t sectors_per_fat_32;
  uint16_t fat_flags;
  uint16_t fs_version_number;
  uint32_t cluster_number_root_dir;
  uint16_t sector_number_fsinfo;
  uint16_t sector_number_backup;
  uint8_t reserved[12];

  uint8_t drive_number;
  uint8_t current_head;
  uint8_t boot_signature;
  uint32_t volume_id;
  uint8_t volume_label[11];
  uint8_t fs_type[8];
  uint8_t boot_code[420];
  uint16_t boot_sector_signature;
} __attribute((packed));

/*
 *	FAT16 Entry found in the root directory
 *	Offset 			Description 			Size (Bytes)
 *	0x00 			Filename 				8
 *	0x08 			Filename extension 		3
 *	0x0B 			File attributes 		1
 *	0x0C 			Reserved 				10
 *	0x16 			Time created/modified 	2
 *	0x18 			Date created/modified 	2
 *	0x1A 			Starting cluster number	2
 *	0x1C 			File size (Bytes) 		4
 */

#define FILENAME_NEVER_USED 0x00
#define FILENAME_FILE_DELETED 0xE5
#define FILENAME_FIRST_CHAR_E5 0x05

/*
 * if second byte is also 0x2E, cluster number contains the address of the
 * parent directory, if the parent directory is the root directory, the
 * address: 0x0000 is specified here
 */
#define FILENAME_DIRECTORY 0x2E

#define FILE_ATTR_RO 0x01
#define FILE_ATTR_HIDDEN 0x02
#define FILE_ATTR_SYSTEM 0x04
#define FILE_ATTR_LABEL 0x08
#define FILE_ATTR_SUBDIR 0x10
#define FILE_ATTR_ARCHIVE 0x20

struct FAT16Entry {
  uint8_t filename[8];
  uint8_t filename_ext[3];
  uint8_t file_attr;
  uint8_t reserved[10];
  uint16_t time_last_modified;
  uint16_t date_last_modified;
  uint16_t cluster_start;
  uint32_t file_size;
} __attribute((packed));

/*
 *	FAT32 Entry found in the root directory
 *	Offset 			Description 			Size (Bytes)
 *	0x00 			Filename 				8
 *	0x08 			Filename extension 		3
 *	0x0B 			File attributes 		1
 *	0x0C 			Reserved 				1
 *	0x0D 			FAT creation date/time 	5
 *	0x12 			Accessed date 			2
 *	0x14 			High byte cluster addr 	2
 *	0x16 			Time created/modified 	2
 *	0x18 			Date created/modified 	2
 *	0x1A 			Low byte cluster addr	2
 *	0x1C 			File size (Bytes) 		4
 */

struct FAT32Entry {
  uint8_t filename[8];
  uint8_t filename_ext[3];
  uint8_t file_attr;
  uint8_t reserved;
  uint8_t date_time_creat[5];
  uint16_t accessed_date;
  uint16_t first_cluster_addr_high;
  uint16_t time_last_modified;
  uint16_t date_last_modified;
  uint16_t first_cluster_addr_low;
  uint32_t file_size;
} __attribute((packed));

struct fat32_ctx {
  /* partition info */
  uint8_t partition_type;
  uint32_t start_sector;
  uint32_t length_sectors;

  /* fat info */
  uint16_t reserved_sectors;
  uint8_t number_of_fats;
  uint32_t sectors_per_fat_32;
  uint8_t sectors_per_cluster;
  uint32_t cluster_number_root_dir;

  /* filesystem info */
  uint32_t cluster_begin_sector;
  uint32_t root_dir_sector;
  uint32_t fat_begin_sector;

  struct node *fat_list;
};

struct fat32_file {
  /*
   *  holds file size, pointer to linked list head of the fat map
   *  
   *  The number of list items times the sector size should give 
   *  the upper bound of the file size.
   */

  uint32_t file_size;
  uint32_t byte_offset;
  uint32_t current_cluster;
  uint32_t current_sector;
  uint32_t sectors_per_cluster;

  struct node *fat_list;
};

int8_t initialize_fat32(struct fat32_ctx *fat32, struct io_ctx *io,
                        struct sd_ctx *sd);

int8_t fat32_parse_entry(struct FAT32Entry *e);
void fat32_dump_file_meta(struct fat32_file *file);

#endif
