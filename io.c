

/*
 *	USC EE459 Spring 2019 Team 17 - Input/Output Abstraction Layer
 */

#include <stdint.h>

#include "io.h"
#include "sd.h"
#include "spi.h"
#include "uart.h"

#define DBG(...)

/*
 *	the strategy is to abstract the flash reading and writing to placing
 *	and getting bytes, everything else can be handled at the byte level
 *
 *	the address space of the io module is the entire SD card
 */

int8_t initialize_io(struct io_ctx *io, struct sd_ctx *sd) {
  /* get first sector, set sector pointer */

  if (sd_get_sector(0, io->input_sector_buf, SECTOR_SIZE) < 0)
    return -1;

  if (sd_get_sector(0, io->output_sector_buf, SECTOR_SIZE) < 0)
    return -1;

  io->input_sector_addr = 0;
  io->input_byte_addr = 0;

  io->output_sector_addr = 0;
  io->output_byte_addr = 0;

  io->sd = sd;

  return 0;
}

static __attribute__((always inline)) int8_t
io_get_byte(struct io_ctx *io, uint32_t offset, uint8_t *buf) {

  /*
   *	get the target argument, see if a new sector needs to be brought in
   *
   *	place the requested byte into the reference buffer
   *
   *	return success or failure
   */

  // UART_DBG("io: byte offset: 0x");
  // UART_DBG_HEX(offset >> 24);
  // UART_DBG_HEX(offset >> 16);
  // UART_DBG_HEX(offset >> 8);
  // UART_DBG_HEX(offset);
  // UART_DBG("\r\n");

  /* sector address is the byte address divided by the sector size */
  uint32_t sector_addr = (offset >> 9);

  // UART_DBG("io: sector address: 0x");
  // UART_DBG_HEX(sector_addr >> 24);
  // UART_DBG_HEX(sector_addr >> 16);
  // UART_DBG_HEX(sector_addr >> 8);
  // UART_DBG_HEX(sector_addr);
  // UART_DBG("\r\n");

  /* intra sector address of the specified byte */
  uint32_t sector_offset = offset % SECTOR_SIZE;

  /* current sector is not the required sector, get the correct one */
  if (sector_addr != io->input_sector_addr) {
    if (sd_get_sector(sector_addr, io->input_sector_buf, SECTOR_SIZE) < 0) {
      return -1;
    }

    io->input_sector_addr = sector_addr;
  }

  *buf = io->input_sector_buf[sector_offset];

  io->input_byte_addr = sector_addr + sector_offset;

  return 0;
}

/* export one function, inline the previous ones */
int8_t io_read_nbytes(struct io_ctx *io, void *buf, uint32_t offset,
                      uint32_t nbytes) {

  uint8_t *ptr = (uint8_t *)buf;

  for (uint32_t i = 0; i < nbytes; i++) {
    if (io_get_byte(io, offset + i, &ptr[i]) < 0)
      return -1;
  }

  return 0;
}

int8_t io_flush_read_buffer(struct io_ctx *io) {
  /*
   * unconditional write out to the flash
   */
  return sd_get_sector(io->input_sector_addr, io->input_sector_buf,
                       SECTOR_SIZE);
}

int8_t io_flush_write_buffer(struct io_ctx *io) {
  /*
   * unconditional write out to the flash
   */
  return sd_put_sector(io->output_sector_addr, io->output_sector_buf,
                       SECTOR_SIZE);
}

int8_t io_put_byte(struct io_ctx *io, uint32_t offset, uint8_t *buf) {
  /*
   *  Writing is a little more complex. First we have to write out the
   *  previous block, then we have to bring in the block that was requested,
   *  then modify the content of that block, and either force it out,
   *  let the watchdog timer handle it, or let it write out when the blocks
   *  switch
   */

  /* sector address is the byte address divided by the sector size */
  uint32_t sector_addr = (offset / 512);

  DBG("io: offset: 0x%X\n", offset);

  DBG("io: sector addr: 0x%X\n", sector_addr);

  /* intra sector address of the specified byte */
  uint32_t sector_offset = offset % SECTOR_SIZE;

  /* current sector is not the required sector, get the correct one */
  if (sector_addr != io->output_sector_addr) {

    /* write back previous data unconditionally */
    if (io_flush_write_buffer(io) < 0) {
      DBG("io: unable to flush write buffer\n");
      return -1;
    }

    // UART_DBG_BIN(io->output_sector_buf, SECTOR_SIZE);

    if (sd_get_sector(sector_addr, io->output_sector_buf, 512) < 0) {

      DBG("io: unable to get write data, failing\n");
      return -1;
    }

    UART_DBG_BIN(io->output_sector_buf, SECTOR_SIZE);

    io->output_sector_addr = sector_addr;
  }

  io->output_sector_buf[sector_offset] = *buf;

  io->output_byte_addr = sector_addr + sector_offset;
}

int8_t io_write_nbytes(struct io_ctx *io, void *buf, uint32_t offset,
                      uint32_t nbytes) {

  uint8_t *ptr = (uint8_t *)buf;

  for (uint32_t i = 0; i < nbytes; i++) {
    if (io_put_byte(io, offset + i, &ptr[i]) < 0)
      return -1;
  }

  return 0;
}

