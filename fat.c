/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include <stdint.h>
#include <stddef.h>

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
  uart_write_str(".");
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

  fat32->fat_list = NULL;

  return 0;
}

void fat32_dump_address(uint32_t *dat) {
  uart_write_str("fat list item: ");
  uart_write_32(*dat);
  uart_write_str("\r\n");
}

int8_t fat32_is_last_cluster(uint32_t dat) {
  if((0x0FFFFFFF & dat)  >= 0x0FFFFFF8)
    return true;
  else
    return false;
}

int8_t fat32_get_fat(struct fat32_ctx *ctx, struct io_ctx *io, uint32_t first_cluster) {
  
  struct node *n = NULL;

  uint32_t curr_address = first_cluster;
  uint32_t next_address;

  do {

    if(io_read_nbytes(io, &next_address, SECTOR_SIZE_BYTES * ctx->fat_begin_sector + curr_address * 4, sizeof(curr_address)) < 0) {
      return -1;
    }

    UART_DBG("pointer: ");
    UART_DBG_32(curr_address);
    UART_DBG(" value: ");
    UART_DBG_32(next_address);
    UART_DBG("\r\n");

    /*
     *  Lets cache the fat to speed up the look-up
     */

    /* we want to push the previous address onto the list if the next one isn't the last */
    list_push_tail(&n, &curr_address, sizeof(curr_address));

    curr_address = next_address;

  } while(!fat32_is_last_cluster(next_address));

  ctx->fat_list = n;

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
   *  
   *  IMPORTANT: keep in mind that in the FAT, the cluster number is the 32 bit chunk offset
   *  from the fat begin address... whaaa
   *  
   *  So we actually want the last cluster on the list, check a the end
   */

  /* lets build the 64 bit value and store in the context */
  if(fat32_get_fat(ctx, io, fat32_calc_first_cluster(e->first_cluster_addr_high, e->first_cluster_addr_low)) < 0)
    return -1;


  file->file_size = e->file_size;
  file->byte_offset = 0;
  file->current_cluster = fat32_calc_first_cluster(e->first_cluster_addr_high, e->first_cluster_addr_low);
  file->sectors_per_cluster = ctx->sectors_per_cluster;
  file->current_sector = 0;

  list_dump(ctx->fat_list, fat32_dump_address);
}

// int8_t fat32_cache_root_dir(struct fat32_ctx *ctx, struct sd_ctx) {
//   /*
//    *  parse the root directory, cache the values of the entries that are
//    *  actually files, their indexes, etc. the root directory in fat32 is
//    *  slightly different than the older siblings. The root directory is
//    *  essentially just another file, so its file table must be traversed 
//    *  and cached for complete analysis of its contents.
//    *  
//    *  save the values: pointer to linked list concerning the root
//    *  directory entries
//    */

//   /* need an upper bound on the number of entries. sizeof(fat) / sizeof(entry) ? */

//   /* first traverse the fat */

//   for (int j = 0; j < 16; j++) {
//     if (io_read_nbytes(&io, &e,
//                        (fat32.root_dir_sector * sd.sd_sector_size) +
//                            (j * sizeof(struct FAT32Entry)),
//                        sizeof(struct FAT32Entry)) < 0) {
//       UART_DBG("main: error reading FAT32 root entry\r\n");
//       break;
//     }

//     if (fat32_parse_entry(&e) < 0)
//       break;
//     dump_bin(&e, sizeof(struct FAT32Entry));

//     if((e.file_attr == FILENAME_NEVER_USED) || (e.file_attr == FILENAME_FILE_DELETED) || ((e.file_attr & 0x0F) == 0x0F))
//       continue;

//     uart_write_str("main: file\r\n");
//     fat32_open_file(&fat32, &e, &io, &file);
//     fat32_dump_file_meta(&file);

//     fat32_close_file(&fat32, &file);
//   }
// }

void fat32_close_root_dir(struct fat32_ctx *ctx) {

}

void fat32_dump_file_meta(struct fat32_file *file) {
  uart_write_str("\r\nfile size: 0x");
  uart_write_32(file->file_size);
  uart_write_str("\r\nfile position: 0x");
  uart_write_32(file->byte_offset);
  uart_write_str("\r\ncurrent cluster: 0x");
  uart_write_32(file->current_cluster);
  uart_write_str("\r\nsectors per cluster: 0x");
  uart_write_32(file->sectors_per_cluster);
  uart_write_str("\r\ncurrent sector: 0x");
  uart_write_32(file->current_sector);
  uart_write_str("\r\n");
}

void fat32_close_file(struct fat32_ctx *ctx, struct fat32_file *file) {
  list_free(ctx->fat_list);
}

// int8_t fat_32_read_file_byte() {

// }

// int8_t fat32_read_file_nbytes() {

// }

// int8_t fat32_write_file_byte() {

// }

// int8_t fat32_write_file_nbytes() {
  
// }
