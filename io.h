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

	/* 
	 * 	same as in sd, but we need to layer the abstraction and we aren't 
	 * 	assuming the block size will always be 512 
	 */
	struct sd_ctx *sd;
};

uint8_t io_read_nbytes(struct io_ctx *io, void *buf, uint32_t offset, 
	uint32_t nbytes);

#endif
