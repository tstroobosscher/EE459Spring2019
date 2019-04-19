/*
 *	USC EE459 Spring 2019 Team 17 - FAT Routines
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "list.h"
#include "debug.h"
#include "fat.h"
#include "io.h"
#include "sd.h"
#include "uart.h"
#include "astdio.h"

#define FAT32_DBG_FILENAME(x) fat32_dump_filename(x)
#define FAT32_DBG_ADDRESS(x) fat32_dump_address(x)
#define LIST_DBG(x, y) list_dump(x, y)
#define FAT32_DBG_FILE_META(x) fat32_dump_file_meta(x)

static inline uint32_t
fat32_calc_first_cluster(uint16_t high, uint16_t low) {
  uint32_t shift_high = high;
  uint32_t shift_low = low;

  return (shift_high << 16) | (shift_low);
}

static inline uint32_t
fat32_calc_lba_from_cluster(uint32_t cluster_begin_lba,
                            uint32_t sectors_per_cluster,
                            uint32_t cluster_number) {
  return cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster;
}

static inline void
fat32_dump_filename(struct FAT32Entry *e) {
  uart_write_strn(UART_PORT_0, e->filename, 8);
  uart_write_str(UART_PORT_0, ".");
  uart_write_strn(UART_PORT_0, e->filename_ext, 3);
}

void fat32_dump_file_meta(struct fat32_file *file) {
  printf("file name: %s.%s\n", file->file_name, file->file_ext);
  printf("file size: 0x%lX\n", file->file_size);
  printf("file start byte: 0x%lX\n", file->start_byte_offset);
  printf("file current cluster: 0x%lX\n", file->current_cluster);
  printf("file sectors per cluster: 0x%lX\n", file->sectors_per_cluster);
  printf("file current sector: 0x%lX\n", file->current_sector);
  printf("file root directory offset: 0x%lX\n", file->root_dir_offset);
  // uart_write_str(UART_PORT_0, "\r\nfile name: ");
  // uart_write_strn(UART_PORT_0, file->file_name, 8);
  // uart_write_str(UART_PORT_0, ".");
  // uart_write_strn(UART_PORT_0, file->file_ext, 3);
  // uart_write_str(UART_PORT_0, "\r\nfile size: 0x");
  // uart_write_32(UART_PORT_0, file->file_size);
  // uart_write_str(UART_PORT_0, "\r\nfile position: 0x");
  // uart_write_32(UART_PORT_0, file->start_byte_offset);
  // uart_write_str(UART_PORT_0, "\r\ncurrent cluster: 0x");
  // uart_write_32(UART_PORT_0, file->current_cluster);
  // uart_write_str(UART_PORT_0, "\r\nsectors per cluster: 0x");
  // uart_write_32(UART_PORT_0, file->sectors_per_cluster);
  // uart_write_str(UART_PORT_0, "\r\ncurrent sector: 0x");
  // uart_write_32(UART_PORT_0, file->current_sector);
  // uart_write_str(UART_PORT_0, "\r\nlist pointer: 0x");
  // uart_write_32(UART_PORT_0, (uint32_t)file->fat_list);
  // uart_write_str(UART_PORT_0, "\r\nroot_dir_offset: 0x");
  // uart_write_32(UART_PORT_0, file->root_dir_offset);
  // uart_write_str(UART_PORT_0, "\r\n");
}

int8_t fat32_parse_entry(struct FAT32Entry *e) {
  switch (e->filename[0]) {
  case FILENAME_NEVER_USED:
    /* end of directory */
    return -1;
  case FILENAME_FILE_DELETED:
    DBG("fat32: deleted entry found: %s.%s\n", e->filename, e->filename_ext);
    return 0;
  case FILENAME_FIRST_CHAR_E5:
    /* Regular file, proceed normally */
    DBG("fat32: entry found, starting with 0xE5: %s.%s\n", e->filename, e->filename_ext);
    break;
  default:
    /* LFN's not yet implemented */
    if ((e->file_attr & 0x0F) == 0x0F) {
      DBG("fat32: entry found: LFN\n");
      DBG("    LFN's not yet implemented :(\n");
      return 0;
    } else {
      /* Regular file, proceed normally */
      DBG("fat32: entry found: %s.%s\n", e->filename, e->filename_ext);
      break;
    }
  }

  if (e->file_attr & FILE_ATTR_RO)
    DBG("    Attribute: READONLY\n");
  if (e->file_attr & FILE_ATTR_HIDDEN)
    DBG("    Attribute: HIDDEN\n");
  if (e->file_attr & FILE_ATTR_SYSTEM)
    DBG("    Attribute: SYSTEM\n");
  if (e->file_attr & FILE_ATTR_LABEL)
    DBG("    Attribute: LABEL\n");
  if (e->file_attr & FILE_ATTR_SUBDIR)
    DBG("    Attribute: SUBDIR\n");
  if (e->file_attr & FILE_ATTR_ARCHIVE)
    DBG("    Attribute: ARCHIVE\n");

  uint32_t cluster_addr = fat32_calc_first_cluster(e->first_cluster_addr_high,
                                                   e->first_cluster_addr_low) + 2;

  DBG("    Starting cluter address: 0x%lX\n", cluster_addr);

  return 0;
}

static void
fat32_set_partition(struct fat32_ctx *fat32, struct PartitionTable pt[]) {
  /* lets just worry about the first partition */
  fat32->partition_type = pt[0].partition_type;
  fat32->start_sector = pt[0].start_sector;
  fat32->length_sectors = pt[0].length_sectors;
}

static void
fat32_set_context(struct fat32_ctx *ctx, struct FAT32BootSector *bs) {

  /*
   *  Grab the relevant information from BS, then process, then recycle
   */

  ctx->reserved_sectors = bs->reserved_sectors;
  DBG("fat32: reserved_sectors = 0x%X\n", ctx->reserved_sectors);
  ctx->number_of_fats = bs->number_of_fats;
  DBG("fat32: number_of_fats = 0x%X\n", ctx->number_of_fats);
  ctx->sectors_per_fat_32 = bs->sectors_per_fat_32;
  DBG("fat32: sectors_per_fat_32 = 0x%X\n", ctx->sectors_per_fat_32);
  ctx->sectors_per_cluster = bs->sectors_per_cluster;
  DBG("fat32: sectors_per_cluster = 0x%X\n", ctx->sectors_per_cluster);
  ctx->cluster_number_root_dir = bs->cluster_number_root_dir;
  DBG("fat32: cluster_number_root_dir = 0x%X\n", ctx->cluster_number_root_dir);
  ctx->cluster_begin_sector =
      ctx->start_sector + ctx->reserved_sectors +
      (ctx->number_of_fats * ctx->sectors_per_fat_32);

  DBG("fat32: cluster_begin_sector = 0x%X\n", ctx->cluster_begin_sector);

  ctx->root_dir_sector =
      (ctx->cluster_begin_sector +
       ((ctx->cluster_number_root_dir - 2) * ctx->sectors_per_cluster));

  ctx->fat_begin_sector = ctx->start_sector + ctx->reserved_sectors;

  ctx->root_dir_entries = NULL;
}

static void fat32_dump_address(uint32_t *dat) {
  uart_write_str(UART_PORT_0, "fat list item: ");
  uart_write_32(UART_PORT_0, *dat);
  uart_write_str(UART_PORT_0, "\r\n");
}

static int8_t
fat32_is_last_cluster(uint32_t dat) {
  if ((0x0FFFFFFF & dat) >= 0x0FFFFFF8)
    return true;
  else
    return false;
}

static int8_t
fat32_get_fat(struct fat32_ctx *ctx, uint32_t first_cluster,
              struct node **ptr) {
  /*
   *  traverse the fat, create a heap structure pointing to a linked list
   *  containing the values, return by reference
   */

  struct node *n = NULL;

  uint32_t curr_address = first_cluster;
  uint32_t next_address;

  do {

    if (io_read_nbytes(ctx->io, &next_address,
                       SECTOR_SIZE_BYTES * ctx->fat_begin_sector +
                           curr_address * 4,
                       sizeof(curr_address)) < 0) {
      return -1;
    }

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
  if (fat32_get_fat(ctx,
                    fat32_calc_first_cluster(e->first_cluster_addr_high,
                                             e->first_cluster_addr_low) + 2,
                    &(file->fat_list)) < 0)
    return -1;

  file->file_size = e->file_size;
  file->start_byte_offset = 0;
  file->current_byte_offset = 0;

  strncpy(file->file_name, e->filename, 8);
  strncpy(file->file_ext, e->filename_ext, 3);

  file->current_cluster = fat32_calc_first_cluster(e->first_cluster_addr_high,
                                                   e->first_cluster_addr_low) + 2;
  file->sectors_per_cluster = ctx->sectors_per_cluster;
  file->current_sector = 0;

  return 0;
}

static int8_t
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
   *  need an upper bound on the number of entries. sizeof(fat) / sizeof(entry)
   * ?
   */

  struct fat32_file file;

  /* 16 is arbitrary right now, directory entries are 0 delimited */
  for (uint32_t j = 0; j < 16; j++) {

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

    if ((e->filename[0] == FILENAME_NEVER_USED) ||
        (e->filename[0] == FILENAME_FILE_DELETED) ||
        ((e->file_attr & 0x0F) == 0x0F))
      continue;

    /* we should also keep a list to track directory entries, really no need to
     * open files here */

    fat32_open_file(ctx, e, io, &file);

    file.root_dir_offset = j;

    UART_DBG("fat32: caching file entry at root dir offset 0x");
    UART_DBG_32(file.root_dir_offset);
    UART_DBG("\r\n");

    // uart_write_32(file.root_dir_offset);

    // list_dump((struct node *)file.fat_list, fat32_dump_address);

    list_push_tail(&ctx->root_dir_entries, &file, sizeof(struct fat32_file));

    // fat32_dump_file_meta(&file);

    /* restate this! memory leak! */
    // fat32_close_file(ctx, &file);
  }

  return 0;
}

void fat32_close_root_dir(struct fat32_ctx *ctx) {
  // list_free(ctx->fat_list);
  // list_free(ctx->file_list);
}

int8_t initialize_fat32(struct fat32_ctx *fat32, struct io_ctx *io,
                        struct sd_ctx *sd) {
  /* read the partition table */

  struct FAT32Entry e;
  struct PartitionTable pt[PARTITION_TABLE_ENTRIES];
  struct FAT32BootSector bs;

  fat32->io = io;

  if (sd->sd_status == SD_DISABLED)
    return 0;

  if (io_read_nbytes(fat32->io, &pt, PARTITION_TABLE_OFFSET,
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
  if (io_read_nbytes(fat32->io, &bs, SECTOR_SIZE_BYTES * pt[0].start_sector,
                     sizeof(struct FAT32BootSector)) < 0) {
    UART_DBG("fat32: error reading FAT32 boot sector\r\n");
    return -1;
  }

  UART_DBG_BIN(&bs, sizeof(struct FAT32BootSector));

  fat32_set_context(fat32, &bs);

  if (fat32_cache_root_dir(fat32, sd, fat32->io, &e) < 0) {
    UART_DBG("fat32: unable to cache root directory\r\n");
    return -1;
  }

  struct node *n;

  for(n = fat32->root_dir_entries; n != NULL; n = n->next) {
    UART_DBG("fat32: cached file entry: \r\n");
    fat32_dump_file_meta((struct fat32_file *)n->data);
  }

  return 0;
}

int8_t fat32_address_in_dir(struct fat32_ctx *ctx, uint32_t addr) {

  struct node *n = ctx->root_dir_entries;

  while (n != NULL) {

    struct fat32_file *f = n->data;
    struct node *t = f->fat_list;

    while (t != NULL) {
      uint32_t *ptr = (uint32_t *)t->data;

      if (*ptr == addr)
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

  while (fat32_address_in_dir(ctx, cluster))

    cluster++;

  return cluster;
}

int8_t fat32_root_entry_found(struct fat32_ctx *ctx, uint32_t loc) {
  /*
   *  search for each location. I think this calculation is like O(n^2),
   *  would be better to organize the linked list first then perform ordered
   *  calculations. but whatever, its not like it needs to be superfast right
   *  now anyways
   */
  for (struct node *n = ctx->root_dir_entries; n != NULL; n = n->next) {
    struct fat32_file *file = (struct fat32_file *)n->data;
    if (file->root_dir_offset == loc) {
      UART_DBG("fat32: root entry found!\r\n");
      return true;
    } else {
      UART_DBG("fat32: root entry not found\r\n");
    }
  }

  return false;
}

uint32_t fat32_get_next_root_dir_loc(struct fat32_ctx *ctx) {

  /* 
   * file entries are stored in a linked list, start at the bottom and search
   * through the list to find the lowest valued, however, we can't really assume
   * that list items are in order, so we must linear search each index
   */

  uint32_t root_dir_index = 0;

  while (fat32_root_entry_found(ctx, root_dir_index)) {
    root_dir_index++;
  }

  return root_dir_index;
}

struct fat32_file *fat32_file_exists(struct fat32_ctx *ctx, struct fat32_file *file) {
  /*
   *  Needs valid filename
   */

  /* search the cached root directory entries and linear search the names */
  for (struct node *n = ctx->root_dir_entries; n != NULL; n = n->next) {

    struct fat32_file *cached_file = (struct fat32_file *)n->data;

    /* not a null terminated string */
    // UART_DBG("fat32: cached file_name = ");
    // uart_write_strn(0, cached_file->file_name, 8);
    // UART_DBG("\r\n");
    // UART_DBG("fat32: cached file_ext = ");
    // uart_write_strn(0, cached_file->file_ext, 3);
    // UART_DBG("\r\n");

    // UART_DBG("fat32: new file_name = ");
    // uart_write_strn(0, file->file_name, 8);
    // UART_DBG("\r\n");
    // UART_DBG("fat32: new file_ext = ");
    // uart_write_strn(0, file->file_ext, 3);
    // UART_DBG("\r\n");

    // UART_DBG("fat32: cached file root dir offset = ");
    // UART_DBG_32(file->root_dir_offset);
    // UART_DBG("\r\n");

    if (!strncmp(cached_file->file_name, file->file_name, 8) &&
        !strncmp(cached_file->file_ext, file->file_ext, 3)) {
      return cached_file;
    }
  }

  return NULL;
}

int8_t fat32_update_directory_entry(struct fat32_ctx *ctx,
                                    struct FAT32Entry *e) {

  return 0;
}

int8_t fat32_update_fat(struct fat32_ctx *ctx, struct fat32_file *file, uint32_t cluster) {
  /* 
   * updating the fat: first find the new cluster, then push the cluster data 
   * to the file fat list, then write the next cluster address to the previous
   * fat entry, then write the delimiter to the next fat entry
   */

  struct node *n = file->fat_list;
  uint32_t next = cluster;
  uint32_t delimiter = 0x0FFFFFFF;

  /* if fat_list is empty, then first cluster so dont write the next entry to the fat */
  if(n != NULL) {
    /* list is not empty */

    uint32_t prev;
    while(n != NULL) {
      prev = n->data;
      n = n->next;
    }

    UART_DBG("fat32: previous tail: 0x");
    UART_DBG_32(prev);
    UART_DBG("\r\n");

    /* update the previous tail, if there is one */
    io_write_nbytes(ctx->io, &next, ctx->fat_begin_sector * SECTOR_SIZE + 4 * prev, sizeof(prev));
  }

  /* push cluster to fat_list */
  list_push_tail(&file->fat_list, &next, sizeof(next));

  UART_DBG("fat32: updating delimiter at address: 0x");
  UART_DBG_32(next);
  UART_DBG("\r\n");

  /* write the delimiter to the fat */
  io_write_nbytes(ctx->io, &delimiter, ctx->fat_begin_sector * SECTOR_SIZE + 4 * next, sizeof(delimiter));
  return 0;
}

void fat32_dump_entry(struct FAT32Entry *e) {
  uart_write_str(UART_PORT_0, "file name: ");
  uart_write_strn(UART_PORT_0, e->filename, 8);
  uart_write_str(UART_PORT_0, ".");
  uart_write_strn(UART_PORT_0, e->filename_ext, 3);
  uart_write_str(UART_PORT_0, "\r\nfile size: 0x");
  uart_write_32(UART_PORT_0, e->file_size);
  uart_write_str(UART_PORT_0, "\r\nfirst cluster address: 0x");
  uart_write_32(UART_PORT_0, fat32_calc_first_cluster(e->first_cluster_addr_high, e->first_cluster_addr_low));
  uart_write_str(UART_PORT_0, "\r\n");
}

int8_t fat32_update_file_size(struct fat32_ctx *ctx, struct fat32_file *file, uint32_t size) {

  struct FAT32Entry e;

  if (io_read_nbytes(ctx->io, &e,
                       (ctx->root_dir_sector * SECTOR_SIZE) +
                           (file->root_dir_offset * sizeof(struct FAT32Entry)),
                       sizeof(struct FAT32Entry)) < 0) {
      UART_DBG("main: error reading FAT32 root entry\r\n");
  }

  e.file_size += size;

  UART_DBG("fat32: updating file size at root dir offset 0x");
  UART_DBG_32(file->root_dir_offset);
  UART_DBG("\r\n");

  fat32_dump_entry(&e);

  io_write_nbytes(ctx->io, &e, ctx->root_dir_sector * SECTOR_SIZE + sizeof(struct FAT32Entry) * file->root_dir_offset, sizeof(struct FAT32Entry));

  return 0;
}

int8_t fat32_creat_file(struct fat32_ctx *ctx, struct fat32_file *file) {
  /*
   *  file name needs to be filled
   */

  struct fat32_file *f;

  if((f = fat32_file_exists(ctx, file)) != NULL) {
    /* file already exists */
    UART_DBG("fat32: file already exists\r\n");
    /* just return the cached file information */
    file->root_dir_offset = f->root_dir_offset;
    file->file_size = f->file_size;
    file->fat_list = f->fat_list;
    file->current_cluster = f->current_cluster;

  } else {

    /* only write to the root directory if the file isn't there */
    uint32_t first_cluster_addr;
    struct FAT32Entry e;
    memset(&e, 0, sizeof(struct FAT32Entry));

    UART_DBG("fat32: creating new file entry\r\n");

    file->root_dir_offset = fat32_get_next_root_dir_loc(ctx);

    UART_DBG("fat32: creat: root dir offset: 0x");
    UART_DBG_32(file->root_dir_offset);
    UART_DBG("\r\n");

    first_cluster_addr = fat32_get_next_cluster(ctx);

    UART_DBG("fat32: next cluster address: 0x");
    UART_DBG_32(first_cluster_addr);
    UART_DBG("\r\n");

    e.first_cluster_addr_high = (first_cluster_addr >> 16);
    e.first_cluster_addr_low = (first_cluster_addr);

    strncpy(e.filename, file->file_name, 8);
    strncpy(e.filename_ext, file->file_ext, 3);

    e.file_size = 0;
    file->file_size = 0;

    /* may need to be flushed! */
    io_write_nbytes(ctx->io, &e, ctx->root_dir_sector * SECTOR_SIZE + 32 * file->root_dir_offset, sizeof(struct FAT32Entry));

    // fat32_update_fat(ctx, file, first_cluster_addr);
    file->fat_list = NULL;

    file->current_cluster = first_cluster_addr;
  }

  file->start_byte_offset = ctx->cluster_begin_sector * SECTOR_SIZE + (file->current_cluster - 2) * ctx->sectors_per_cluster * SECTOR_SIZE;
  file->current_byte_offset = file->start_byte_offset;

  return 0;
}

void fat32_close_file(struct fat32_ctx *ctx, struct fat32_file *file) {
  // list_free(ctx->fat_list);
}

int8_t fat32_write_file_nbytes(struct fat32_ctx *ctx, struct fat32_file *file,
                             uint8_t *buf, uint32_t nbytes) {
  /*
   *  write byte to the next position recorded by the file position, increment
   */

  /* jesus christ the 2 unit translation is frustrating */
  uint64_t addr =  + file->current_byte_offset;

  UART_DBG("main: fat32 writing to byte address 0x");
  UART_DBG_32(addr >> 32);
  UART_DBG_32(addr);
  UART_DBG("\r\n");

  UART_DBG("main: buf size: 0x");
  UART_DBG_32(nbytes);
  UART_DBG("\r\n");

  io_write_nbytes(ctx->io, buf, addr, nbytes);

  io_flush_read_buffer(ctx->io);
  io_flush_write_buffer(ctx->io);

  fat32_update_file_size(ctx, file, nbytes);

  file->current_byte_offset += nbytes;

  return 0;
}
