/*
 *	USC EE459 Spring 2019 Team 17 - IO Header
 */

#ifndef IO_H
#define IO_H

#include "sd.h"

/*
 *	io buffer structures
 *
 *	the io ctx holds the current buffered data and the the address of the
 *	buffered data
 */

struct io_ctx {

  /* pointer to current sector */
  uint16_t input_sector_addr;

  /* pointer to current byte */
  uint64_t input_byte_addr;

  /* sector buffer */
  uint8_t input_sector_buf[SECTOR_SIZE];

  /* sector checksum */
  uint16_t input_sector_crc;

  /* pointer to current sector */
  uint16_t output_sector_addr;

  /* pointer to current byte */
  uint64_t output_byte_addr;

  /* sector buffer */
  uint8_t output_sector_buf[SECTOR_SIZE];

  /* sector checksum */
  uint16_t output_sector_crc;

  /*
   * 	same as in sd, but we need to layer the abstraction and we aren't
   * 	assuming the block size will always be 512
   */
  struct sd_ctx *sd;
};

int8_t initialize_io(struct io_ctx *io, struct sd_ctx *sd);

int8_t io_read_nbytes(struct io_ctx *io, void *buf, uint32_t offset,
                      uint32_t nbytes);
int8_t io_put_byte(struct io_ctx *io, uint32_t offset, uint8_t *buf);
int8_t io_flush_write_buffer(struct io_ctx *io);

#endif
