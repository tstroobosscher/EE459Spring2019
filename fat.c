/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include <stddef.h>
#include <stdint.h>

#include "debug.h"
#include "fat.h"
#include "io.h"
#include "sd.h"
#include "uart.h"

#ifdef DEBUG
#define FAT32_DBG_FILENAME(x) fat32_dump_filename(x)
#define FAT32_DBG_ADDRESS(x) fat32_dump_address(x)
#define LIST_DBG(x, y) list_dump(x, y)
#define FAT32_DBG_FILE_META(x) fat32_dump_file_meta(x)
#else
#define FAT32_DBG_FILENAME(x)
#define FAT32_DBG_ADDRESS(x) 
#define LIST_DBG(x, y)
#define FAT32_DBG_FILE_META(x)
#endif


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
    UART_DBG("fat32: deleted entry found: ");
    FAT32_DBG_FILENAME(e);
    UART_DBG("\r\n");
    return 0;
  case FILENAME_FIRST_CHAR_E5:
    /* Regular file, proceed normally */
    UART_DBG("fat32: entry found, starting with 0xE5: ");
    FAT32_DBG_FILENAME(e);
    UART_DBG("\r\n");
    break;
  default:
    /* LFN's not yet implemented */
    if ((e->file_attr & 0x0F) == 0x0F) {
      UART_DBG("fat32: entry found: LFN\r\n");
      UART_DBG("    LFN's not yet implemented :(\r\n");
      return 0;
    } else {
      /* Regular file, proceed normally */
      UART_DBG("fat32: entry found: ");
      FAT32_DBG_FILENAME(e);
      UART_DBG("\r\n");
      break;
    }
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

  UART_DBG("    Starting cluter address: 0x");
  UART_DBG_32(cluster_addr);
  UART_DBG("\r\n");

  return 0;
}

static __attribute__((always inline)) void
fat32_set_partition(struct fat32_ctx *fat32, struct PartitionTable pt[]) {
  /* lets just worry about the first partition */
  fat32->partition_type = pt[0].partition_type;
  fat32->start_sector = pt[0].start_sector;
  fat32->length_sectors = pt[0].length_sectors;
}

static __attribute__((always inline)) void
fat32_set_context(struct fat32_ctx *fat32, struct FAT32BootSector *bs) {

  /*
   *  Grab the relevant information from BS, then process, then recycle
   */

  fat32->reserved_sectors = bs->reserved_sectors;
  fat32->number_of_fats = bs->number_of_fats;
  fat32->sectors_per_fat_32 = bs->sectors_per_fat_32;
  fat32->sectors_per_cluster = bs->sectors_per_cluster;
  fat32->cluster_number_root_dir = bs->cluster_number_root_dir;

  fat32->cluster_begin_sector =
      fat32->start_sector + fat32->reserved_sectors +
      (fat32->number_of_fats * fat32->sectors_per_fat_32);

  fat32->root_dir_sector =
      (fat32->cluster_begin_sector +
       ((fat32->cluster_number_root_dir - 2) * fat32->sectors_per_cluster));

  fat32->fat_begin_sector = fat32->start_sector + fat32->reserved_sectors;

  fat32->root_dir_entries = NULL;
  //fat32->fat_list = NULL;
  //fat32->file_list = NULL;
}

static __attribute__((always inline)) void fat32_dump_address(uint32_t *dat) {
  uart_write_str("fat list item: ");
  uart_write_32(*dat);
  uart_write_str("\r\n");
}

static __attribute__((always inline)) int8_t
fat32_is_last_cluster(uint32_t dat) {
  if ((0x0FFFFFFF & dat) >= 0x0FFFFFF8)
    return true;
  else
    return false;
}

static __attribute__((always inline)) int8_t
fat32_get_fat(struct fat32_ctx *ctx, struct io_ctx *io, uint32_t first_cluster,
              struct node **ptr) {
  /*
   *  traverse the fat, create a heap structure pointing to a linked list
   *  containing the values, return by reference
   */

  struct node *n = (struct node *)NULL;

  uint32_t curr_address = first_cluster;
  uint32_t next_address;

  do {

    if (io_read_nbytes(io, &next_address,
                       SECTOR_SIZE_BYTES * ctx->fat_begin_sector +
                           curr_address * 4,
                       sizeof(curr_address)) < 0) {
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

    /* we want to push the previous address onto the list if the next one isn't
     * the last */
    list_push_tail(&n, &curr_address, sizeof(curr_address));

    curr_address = next_address;

  } while (!fat32_is_last_cluster(next_address));

  (*ptr) = n;

  return 0;
}

int8_t fat32_open_file(struct fat32_ctx *ctx, struct FAT32Entry *e,
                       struct io_ctx *io, struct fat32_file *file) {
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
   *  IMPORTANT: keep in mind that in the FAT, the cluster number is the 32 bit
   * chunk offset from the fat begin address... whaaa
   *
   *  So we actually want the last cluster on the list, check a the end
   */

  /* lets build the 64 bit value and store in the context */
  if (fat32_get_fat(ctx, io,
                    fat32_calc_first_cluster(e->first_cluster_addr_high,
                                             e->first_cluster_addr_low),
                    &(file->fat_list)) < 0)
    return -1;

  file->file_size = e->file_size;
  file->byte_offset = 0;
  file->current_cluster = fat32_calc_first_cluster(e->first_cluster_addr_high,
                                                   e->first_cluster_addr_low);
  file->sectors_per_cluster = ctx->sectors_per_cluster;
  file->current_sector = 0;

  return 0;
}

static __attribute__((always inline)) int8_t
fat32_cache_root_dir(struct fat32_ctx *ctx, struct sd_ctx *sd,
                     struct io_ctx *io, struct FAT32Entry *e) {
  /*
   *  parse the root directory, cache the values of the entries that are
   *  actually files, their indexes, etc. the root directory in fat32 is
   *  slightly different than the older siblings. The root directory is
   *  essentially just another file, so its file table must be traversed
   *  and cached for complete analysis of its contents.
   *
   *  save the values: pointer to linked list concerning the root
   *  directory entries
   *
   *  So we can't use a lookup table, the fat can be huge and would be more
   *  challenging to make it fit. So a linked list and some extra work
   *  would be the best way to go.
   */

  /* 
   *  need an upper bound on the number of entries. sizeof(fat) / sizeof(entry) ?
   */

  /* first traverse the fat */

  struct fat32_file root_dir;
  
  if (fat32_get_fat(ctx, io, ctx->cluster_number_root_dir, &(root_dir.fat_list)) <
      0) {
    UART_DBG("fat32: unable to get FAT\r\n");
    return -1;
  }

  list_push_tail(&(ctx->root_dir_entries), &root_dir, sizeof(struct fat32_file));

  //list_dump(ctx->fat_list, fat32_dump_address);

  struct fat32_file file;

  /* 16 is arbitrary right now, directory entries are 0 delimited */
  for (int j = 0; j < 16; j++) {

    /* fat needs to linearize the directory space, can span clusters */
    if (io_read_nbytes(io, e,
                       (ctx->root_dir_sector * sd->sd_sector_size) +
                           (j * sizeof(struct FAT32Entry)),
                       sizeof(struct FAT32Entry)) < 0) {
      UART_DBG("main: error reading FAT32 root entry\r\n");
      break;
    }

    if (fat32_parse_entry(e) < 0)
      break;

    //UART_DBG_BIN(e, sizeof(struct FAT32Entry));

    if ((e->filename[0] == FILENAME_NEVER_USED) ||
        (e->filename[0] == FILENAME_FILE_DELETED) ||
        ((e->file_attr & 0x0F) == 0x0F) ||
	((e->file_attr & FILE_ATTR_LABEL)))
      continue;

    /* we should also keep a list to track directory entries, really no need to
     * open files here */

    fat32_open_file(ctx, e, io, &file);

    //list_dump((struct node *)file.fat_list, fat32_dump_address);

    list_push_tail(&(ctx->root_dir_entries), &file, sizeof(struct fat32_file));

    //fat32_dump_file_meta(&file);

    /* restate this! memory leak! */
    //fat32_close_file(ctx, &file);
  }

  return 0;
}

void fat32_close_root_dir(struct fat32_ctx *ctx) {
  //list_free(ctx->fat_list);
  //list_free(ctx->file_list);
}

int8_t initialize_fat32(struct fat32_ctx *fat32, struct io_ctx *io,
                        struct sd_ctx *sd) {
  /* read the partition table */

  struct FAT32Entry e;
  struct PartitionTable pt[PARTITION_TABLE_ENTRIES];
  struct FAT32BootSector bs;

  if (sd->sd_status == SD_DISABLED)
    return 0;

  if (io_read_nbytes(io, &pt, PARTITION_TABLE_OFFSET,
                     sizeof(struct PartitionTable) * PARTITION_TABLE_ENTRIES) <
      0) {
    UART_DBG("fat32: error reading partition table\r\n");
    return -1;
  }

  UART_DBG_BIN(&pt, sizeof(struct PartitionTable) * PARTITION_TABLE_ENTRIES);

  fat32_set_partition(fat32, &pt);

  UART_DBG("fat32: start sector: ");
  UART_DBG_32(fat32->start_sector);
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
    UART_DBG("fat32: error reading FAT32 boot sector\r\n");
    return -1;
  }

  UART_DBG_BIN(&bs, sizeof(struct FAT32BootSector));

  fat32_set_context(fat32, &bs);

  if (fat32_cache_root_dir(fat32, sd, io, &e) < 0) {
    UART_DBG("fat32: unable to cache root directory\r\n");
    return -1;
  }

  return 0;
}

int8_t fat32_address_in_dir(struct fat32_ctx *ctx, uint32_t addr) {
  
  struct node *n = ctx->root_dir_entries;
  
  while(n != NULL) {
    
    struct fat32_file *f = n->data;
    struct node *t = f->fat_list;
    
    while(t != NULL) {
      uint32_t *ptr = (uint32_t *) t->data;

      uart_write_str("fat32: ptr: 0x");
      uart_write_32(*ptr);
      uart_write_str(" addr: 0x");
      uart_write_32(addr);
      uart_write_str("\r\n");
      if(*ptr == addr)
	return true;
      
      t = t->next;
    }
    
    n = n->next;
  }

  return false;
}

int32_t fat32_get_next_cluster(struct fat32_ctx *ctx) {
  /*
   *  The fs cluster info is stored as a list of lists, the first list
   *  describes the root directory entries, of which, each entry
   *  contains a list of cluster addresses that are dedicated to that
   *  file. The idea is to locate the lowest numerically valued cluster
   *  address to allocate to the file being created
   * 
   *  root directory is always at cluster addr 2 (0 and 1 are 
   *  dedicated to fs info), can be an arbitrary size
   *
   *  the list may not be in order, I dont think entry order necessarily
   *  corresponds to a monotonic increase in address order. If we leave
   *  it out of order this algorithm is going to be super expensive. 
   *  essentially we have to search the entire list for every iteration
   *  of the loop, over the entire size of the list. crazy
   */

  /* cluster addressing starts at 2 */
  uint32_t cluster = 2;

  while(fat32_address_in_dir(ctx, cluster))

    cluster++;
  
  return cluster;
}

uint8_t fat32_creat_file(struct fat32_ctx *ctx, struct fat32_file *file) {

  uint32_t res = fat32_get_next_cluster(ctx);

  struct fat32_file f;
  
  return 0;
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
  uart_write_str("\r\nlist pointer: 0x");
  uart_write_32(file->fat_list);
  uart_write_str("\r\n");
}

void fat32_close_file(struct fat32_ctx *ctx, struct fat32_file *file) {
  //list_free(ctx->fat_list);
}

int8_t fat_32_read_file_byte() {}

// int8_t fat32_read_file_nbytes() {

// }

// int8_t fat32_write_file_byte() {

// }

// int8_t fat32_write_file_nbytes() {

// }
