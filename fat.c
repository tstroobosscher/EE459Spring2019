/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include "fat.h"
#include "debug.h"

#ifdef DEBUG_86
#include <stdio.h>
#endif

static __attribute__((always inline)) 
	uint32_t fat32_calc_first_cluster(uint16_t high, uint16_t low) {
	return (high << 16) | (low);
}

static __attribute__((always inline)) 
	uint32_t fat32_calc_lba_from_cluster(uint32_t cluster_begin_lba, 
	uint32_t sectors_per_cluster, uint32_t cluster_number) {
	return cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster;
}

static __attribute__((always inline)) 
	void fat32_dump_filename(struct FAT32Entry *e) {
	uart_write_strn(e->filename, 8);
	uart_write_char(".");
	uart_write_strn(e->filename_ext, 3);
	uart_write_str("\r\n");
}

int8_t fat32_parse_entry(struct FAT32Entry *e) {
	switch(e->filename[0]) {
		case FILENAME_NEVER_USED :
			/* end of directory */
			return -1;
		case FILENAME_FILE_DELETED :
			uart_write_str("fat32: deleted entry found: ");
			fat32_dump_filename(e);
			return 0;
		case FILENAME_FIRST_CHAR_E5 :
			/* Regular file, proceed normally */
			uart_write_str("fat32: entry found, starting with 0xE5: ");
			fat32_dump_filename(e);
			break;
		default:
			/* LFN's not yet implemented */
			if((e->file_attr & 0x0F) == 0x0F) {
				uart_write_str("fat32: entry found: LFN\r\n");
				uart_write_str("    LFN's not yet implemented :(\r\n");
				return 0;
			}
			else
				/* Regular file, proceed normally */
				uart_write_str("fat32: entry found: ");
				fat32_dump_filename(e);
			break;
	}

	if(e->file_attr & FILE_ATTR_RO)
		uart_write_str("    Attribute: READONLY\r\n");
	if(e->file_attr & FILE_ATTR_HIDDEN)
		uart_write_str("    Attribute: HIDDEN\r\n");
	if(e->file_attr & FILE_ATTR_SYSTEM)
		uart_write_str("    Attribute: SYSTEM\r\n");
	if(e->file_attr & FILE_ATTR_LABEL)
		uart_write_str("    Attribute: LABEL\r\n");
	if(e->file_attr & FILE_ATTR_SUBDIR)
		uart_write_str("    Attribute: SUBDIR\r\n");
	if(e->file_attr & FILE_ATTR_ARCHIVE)
		uart_write_str("    Attribute: ARCHIVE\r\n");

	uint32_t cluster_addr = fat32_calc_first_cluster(
		e->first_cluster_addr_high, e->first_cluster_addr_low);

	uart_write_str("    Starting cluter address: 0x");
	uart_write_hex(cluster_addr >> 24);
	uart_write_hex(cluster_addr >> 16);
	uart_write_hex(cluster_addr >> 8);
	uart_write_hex(cluster_addr);
	uart_write_str("\r\n");

	return 0;
}
