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
	uint16_t sector_addr;

	/* pointer to current byte */
	uint64_t byte_addr;

	/* sector buffer */
	uint8_t sector_buf[SECTOR_SIZE];

	/* sector checksum */
	uint16_t sector_crc;
};

int8_t io_get_byte(struct io_ctx *io, struct sd_ctx *sd, uint32_t offset,
	uint8_t *buf);
uint8_t io_read_nbytes(struct io_ctx *io, struct sd_ctx *sd, void *buf, 
	uint32_t offset, uint32_t nbytes);

#endif
