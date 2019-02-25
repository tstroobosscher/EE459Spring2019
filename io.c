/*
 *	USC EE459 Spring 2019 Team 17 - Input/Output Abstraction Layer
 */

#include <stdint.h>

#include "io.h"
#include "sd.h"
#include "uart.h"

/*
 *	the strategy is to abstract the flash reading and writing to placing
 *	and getting bytes, everything else can be handled at the byte level
 */

#define UART_DBG(x)
#define UART_DBG_HEX(x)

uint8_t initialize_io(struct io_ctx *io, struct sd_ctx *sd) {
	/* get first sector, set sector pointer */

	if(sd_get_sector(sd, 0, io->sector_buf, sd->sd_sector_size) < 0)
		return -1;

	io->sector_addr = 0;
	io->byte_addr = 0;
	io->sd = sd;

	return 0;
}

static __attribute__((always inline)) 
	int8_t io_get_byte(struct io_ctx *io, uint32_t offset, uint8_t *buf) {

	/*
	 *	get the target argument, see if a new sector needs to be brought in
	 *	
	 *	place the requested byte into the reference buffer
	 *	
	 *	return success or failure
	 */

	UART_DBG("io: byte offset: 0x");
	UART_DBG_HEX(offset >> 24);
	UART_DBG_HEX(offset >> 16);
	UART_DBG_HEX(offset >> 8);
	UART_DBG_HEX(offset);
	UART_DBG("\r\n");

	/* sector address is the byte address divided by the sector size */
	uint32_t sector_addr = (offset >> 9);

	UART_DBG("io: sector address: 0x");
	UART_DBG_HEX(sector_addr >> 24);
	UART_DBG_HEX(sector_addr >> 16);
	UART_DBG_HEX(sector_addr >> 8);
	UART_DBG_HEX(sector_addr);
	UART_DBG("\r\n");

	/* intra sector address of the specified byte */
	uint32_t sector_offset = offset % io->sd->sd_sector_size;

	/* current sector is not the required sector, get the correct one */
	if(sector_addr != io->sector_addr) {
		if(sd_get_sector(io->sd, sector_addr, io->sector_buf, 
			io->sd->sd_sector_size) < 0 ) {
			return -1;
		}

		io->sector_addr = sector_addr;
	}

	*buf = io->sector_buf[sector_offset];

	io->byte_addr = sector_addr + sector_offset;

	UART_DBG("io: byte received: ");
	UART_DBG_HEX(*buf);
	UART_DBG("\r\n");

	return 0;
}

/* export one function, inline the subequent ones */
uint8_t io_read_nbytes(struct io_ctx *io, void *buf, uint32_t offset, 
	uint32_t nbytes) {

	uint8_t *ptr = (uint8_t *) buf;

	for(uint32_t i = 0; i < nbytes; i++) {
		if(io_get_byte(io, offset + i, &ptr[i]) < 0)
			return -1;
	}

	return 0;

}
