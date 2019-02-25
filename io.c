/*
 *	USC EE459 Spring 2019 Team 17 - Input/Output Abstraction Layer
 */

#include <stdint.h>

#include "io.h"
#include "sd.h"

/*
 *	here we need to implement the POSIX file io functions:
 *		open()
 *		read()
 *		write()
 *		seek()
 *		
 *		SYNOPSIS
 *
 *          int fseek(FILE *stream, long offset, int whence);
 */

int8_t io_seek_sector(struct io_ctx *io, struct sd_ctx *sd, uint64_t offset) {
	/*
	 *	get the target argument, see if a new sector needs to be brought in
	 */

	/* sector addrress is the byte address divided by the sector size */
	uint64_t sector_addr = offset / sd->sd_sector_size;

	/* current sector is not the required sector, get the correct one */
	if(sector_addr != io->sector_addr) {
		if(sd_get_sector(sd, sector_addr, io->sector_buf, 
			sd->sd_sector_size) < 0 ) {
			return -1;
		}
	}

	return 0;
}

