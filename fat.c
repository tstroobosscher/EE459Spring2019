/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include <stdint.h>

#include "debug.h"
#include "fat.h"
#include "io.h"
#include "sd.h"
#include "uart.h"

static __attribute__((always inline)) uint32_t
fat32_calc_first_cluster(uint16_t high, uint16_t low) {
  uint32_t shift_high = high;
  uint32_t shift_low = low;

  return (shift_high << 16) | (shift_low);
}

static __attribute__((always inline)) uint32_t
fat32_calc_lba_from_cluster(uint32_t cluster_begin_lba,
                            uint32_t sectors_per_cluster,
                            uint32_t cluster_number) {
  return cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster;
}

static __attribute__((always inline)) void
fat32_dump_filename(struct FAT32Entry *e) {
  uart_write_strn(e->filename, 8);
  uart_write_char(".");
  uart_write_strn(e->filename_ext, 3);
}

int8_t fat32_parse_entry(struct FAT32Entry *e) {
  switch (e->filename[0]) {
  case FILENAME_NEVER_USED:
    /* end of directory */
    return -1;
  case FILENAME_FILE_DELETED:
    uart_write_str("fat32: deleted entry found: ");
    fat32_dump_filename(e);
    uart_write_str("\r\n");
    return 0;
  case FILENAME_FIRST_CHAR_E5:
    /* Regular file, proceed normally */
    uart_write_str("fat32: entry found, starting with 0xE5: ");
    fat32_dump_filename(e);
    uart_write_str("\r\n");
    break;
  default:
    /* LFN's not yet implemented */
    if ((e->file_attr & 0x0F) == 0x0F) {
      uart_write_str("fat32: entry found: LFN\r\n");
      uart_write_str("    LFN's not yet implemented :(\r\n");
      return 0;
    } else
      /* Regular file, proceed normally */
      uart_write_str("fat32: entry found: ");
    fat32_dump_filename(e);
    uart_write_str("\r\n");
    break;
  }

  if (e->file_attr & FILE_ATTR_RO)
    UART_DBG("    Attribute: READONLY\r\n");
  if (e->file_attr & FILE_ATTR_HIDDEN)
    UART_DBG("    Attribute: HIDDEN\r\n");
  if (e->file_attr & FILE_ATTR_SYSTEM)
    UART_DBG("    Attribute: SYSTEM\r\n");
  if (e->file_attr & FILE_ATTR_LABEL)
    UART_DBG("    Attribute: LABEL\r\n");
  if (e->file_attr & FILE_ATTR_SUBDIR)
    UART_DBG("    Attribute: SUBDIR\r\n");
  if (e->file_attr & FILE_ATTR_ARCHIVE)
    UART_DBG("    Attribute: ARCHIVE\r\n");

  uint32_t cluster_addr = fat32_calc_first_cluster(e->first_cluster_addr_high,
                                                   e->first_cluster_addr_low);

  uart_write_str("    Starting cluter address: 0x");
  uart_write_32(cluster_addr);
  uart_write_str("\r\n");

  return 0;
}

int8_t initialize_fat32(struct fat32_ctx *fat32, struct io_ctx *io,
                        struct sd_ctx *sd) {
  /* read the partition table */

  struct PartitionTable pt[PARTITION_TABLE_ENTRIES];
  memset(&pt, 0, sizeof(struct PartitionTable) * PARTITION_TABLE_ENTRIES);
  struct FAT32BootSector bs;
  memset(&bs, 0, sizeof(struct FAT32BootSector));

  if (sd->sd_status == SD_DISABLED)
    return 0;

  if (io_read_nbytes(io, &pt, PARTITION_TABLE_OFFSET,
                     sizeof(struct PartitionTable) * PARTITION_TABLE_ENTRIES) <
      0) {
    UART_DBG("fat32: error reading partition table\r\n");
    return -1;
  }

  dump_bin(&pt, sizeof(struct PartitionTable) * PARTITION_TABLE_ENTRIES);

  /* lets just worry about the first partition */
  fat32->partition_type = pt[0].partition_type;
  fat32->start_sector = pt[0].start_sector;
  fat32->length_sectors = pt[0].length_sectors;

  UART_DBG("fat32: start sector: ");
  uart_write_32(fat32->start_sector);
  UART_DBG("\r\n");

  if ((fat32->partition_type == PARTITION_TYPE_FAT32_LT2G) ||
      (fat32->partition_type == PARTITION_TYPE_FAT32_LBA)) {

    UART_DBG("fat32: FAT32 partition found\r\n");
  } else {

    /* partition 0 is not FAT32 */
    UART_DBG("fat32: partition 0 is not FAT32\r\n");
    return -1;
  }

  /* read the boot sector */
  if (io_read_nbytes(io, &bs, SECTOR_SIZE_BYTES * pt[0].start_sector,
                     sizeof(struct FAT32BootSector)) < 0) {
    uart_write_str("fat32: error reading FAT32 boot sector\r\n");
    return -1;
  }

  dump_bin(&bs, sizeof(struct FAT32BootSector));

  fat32->reserved_sectors = bs.reserved_sectors;
  fat32->number_of_fats = bs.number_of_fats;
  fat32->sectors_per_fat_32 = bs.sectors_per_fat_32;
  fat32->sectors_per_cluster = bs.sectors_per_cluster;
  fat32->cluster_number_root_dir = bs.cluster_number_root_dir;

  fat32->cluster_begin_sector =
      fat32->start_sector + fat32->reserved_sectors +
      (fat32->number_of_fats * fat32->sectors_per_fat_32);

  fat32->root_dir_sector =
      (fat32->cluster_begin_sector +
       ((fat32->cluster_number_root_dir - 2) * fat32->sectors_per_cluster));

  fat32->fat_begin_sector = fat32->start_sector + fat32->reserved_sectors;

  return 0;
}

int8_t fat32_open_file(struct fat32_ctx *ctx, struct FAT32Entry *e, struct io_ctx *io, struct fat32_file *file) {
  /*
   *  create a linked list that holds the fat table entries, size of the file
   *  it's not necessary to buffer the entire file, but perform the 
   *  calculations necessary to find the particular block corresponding to a
   *  requested byte.
   *  
   *  First need the cluster address of the entry. That indexes the 32 bit 
   *  (FAT32) number in the fat which is at the block offset of 
   *  fat32->fat_begin_sector For every byte in the fat that isnt the end int:
   *  0xFFFFFFFF. 
   *  
   *  for each list item, we just need to save the index of the fat table that
   *  item stays in.
   *  
   *  for each byte we need to check if it sits outside the bounds of the 
   *  buffer, so it makes sense to first implement the byte reading function,
   *  and then write the nbytes function 
   */

    uint32_t fat_address;

    do {
      if(io_read_nbytes(io, &fat_address, ctx->fat_begin_sector + e->cluster_begin_lba, sizeof(fat32_ctx) < 0)
        return -1;

      push_head(/* file_ctx pointer memer, the uint 32 (dynamic) */);

    } while(fat_address != 0xFFFFFFFF /* && not outside the boundary */);

}
