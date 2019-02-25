/*
 *	USC EE459 Spring 2019 Team 17 - FIFO Routines
 */

#include "fifo.h"
#include "utils.h"
#include "debug.h"

void initialize_fifo(struct fifo_t *fifo) {
	/*
	 *	set the head, tail, and status for the fifo structure
	 *
	 *	export
	 */

	fifo->f_head = 0;
	fifo->f_tail = 0;
	fifo->f_status = FIFO_EMPTY;
	return;
}

static char fifo_write_byte(struct fifo_t *fifo, unsigned char *byte) {
	/*
	 *	write a single byte to the fifo, increment the head
	 *	if fifo is full, return -1
	 *	return 0 on success
	 *
	 *	param: fifo is the fifo struct pointer
	 *	param: byte is a reference argument
	 *
	 *	private function, should not be exported
	 */

	/* fifo ready or empty */
	if(fifo->f_status != FIFO_FULL) {

		/* head should be pointing to next available spot */
		fifo->f_buf[fifo->f_head] = *byte;

		/* increment, wrap around buf */
		fifo->f_head++;
		if(fifo->f_head == FIFO_SIZE)
			fifo->f_head = 0;

		/* check buf status */
		if(fifo->f_head == fifo->f_tail)
			fifo->f_status = FIFO_FULL;

		return 0;
	}

	/* fifo full */
	return -1;
}

static char fifo_read_byte(struct fifo_t *fifo, unsigned char *byte) {
	/*
	 *	read a single byte the fifo, increment the tail
	 *	if fifo is empty, return -1
	 *	return 0 on success, assign reference value the read byte
	 *
	 *	param: fifo is the fifo struct pointer
	 *	param: byte is value-result argument
	 *
	 *	private function, should not be exported
	 */

	/* fifo ready or full */
	if(fifo->f_status != FIFO_EMPTY) {

		/* tail should be pointing at next available byte to read */
		*byte = fifo->f_buf[fifo->f_tail];

		/* increment tail, wrap around */
		fifo->f_tail++;
		if(fifo->f_tail == FIFO_SIZE)
			fifo->f_tail = 0;

		/* check buf status */
		if(fifo->f_tail == fifo->f_head)
			fifo->f_status = FIFO_EMPTY;

		return 0;
	}

	/* fifo empty, nothing to read */
	return -1;
}

char fifo_writen(struct fifo_t *fifo, void *buf, int nbytes) {
	/*
	 *	write string of n bytes to the fifo
	 */

	uint8_t *ptr = (uint8_t *) buf;

	for(int i = 0; i < nbytes; i++)
		if(fifo_write_byte(fifo,  &ptr[i]) < 0)
			return -1;

	return 0;
}

char fifo_readn(struct fifo_t *fifo, void *buf, int nbytes) {
	/*
	 *	read a string of n bytes to the fifo
	 */

	uint8_t *ptr = (uint8_t *) buf;

	for(int i = 0; i < nbytes; i++)
		if(fifo_read_byte(fifo,  &ptr[i]) < 0)
			return -1;

	return 0;
}
