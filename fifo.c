#include "fifo.h"
#include "utils.h"

void initialize_fifo(struct fifo_t *fifo) {
	/*
	 *	set the head, tail, and status for the fifo structure
	 *
	 *	export
	 */

	fifo->fifo_head = 0;
	fifo->fifo_tail = 0;
	fifo->fifo_status = FIFO_EMPTY;
	return;
}

#ifdef DEBUG

void fifo_dump_buf(struct fifo_t *fifo, char *ret, int size) {
	/*
	 *	get buf values by reference, wont move the tail pointer
	 */
	memcpy(ret, fifo->fifo_buf, size);
}

void fifo_dump_mem(struct fifo_t *fifo, FILE *stream) {
	/* 
	 * dump the fifo struct contents 
	 */

	fprintf(stream, "fifo status: ");
	switch(fifo->fifo_status) {
		case FIFO_FULL	:
			fprintf(stream, "full\n");
			break;
		case FIFO_EMPTY	:
			fprintf(stream, "empty\n");
			break;
		case FIFO_READY	:
			fprintf(stream, "ready\n");
			break;
		default			:
			fprintf(stream, "unhandled status\n");
			break; 
	}

	for(int i = 0; i < ARRAY_SIZE(fifo->fifo_buf); i++){
		fprintf(stream, "fifo address: %02d, data: 0x%02x", i, fifo->fifo_buf[i]);
		if(fifo->fifo_head == i)
			fprintf(stream, " <- head");
		if(fifo->fifo_tail == i)
			fprintf(stream, " <- tail");

		fprintf(stream, "\n");
	}


	return;
}

#endif

static char fifo_write_byte(struct fifo_t *fifo, unsigned char *byte) {
	/*
	 *	write a single byte to the fifo, increment the head
	 *	if fifo is full, return -1
	 *	return 0 on success
	 *
	 *	param: fifo is the fifo struct pointer
	 *	param: byte is value-result argument
	 *
	 *	private function, should not be exported
	 */

	/* fifo ready or empty */
	if(fifo->fifo_status != FIFO_FULL) {

		/* head should be pointing to next available spot */
		fifo->fifo_buf[fifo->fifo_head] = *byte;

		/* increment, wrap around buf */
		fifo->fifo_head++;
		if(fifo->fifo_head == FIFO_SIZE)
			fifo->fifo_head = 0;

		/* check buf status */
		if(fifo->fifo_head == fifo->fifo_tail)
			fifo->fifo_status = FIFO_FULL;

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
	if(fifo->fifo_status != FIFO_EMPTY) {

		/* tail should be pointing at next available byte to read */
		*byte = fifo->fifo_buf[fifo->fifo_tail];

		/* increment tail, wrap around */
		fifo->fifo_tail++;
		if(fifo->fifo_tail == FIFO_SIZE)
			fifo->fifo_tail = 0;

		/* check buf status */
		if(fifo->fifo_tail == fifo->fifo_head)
			fifo->fifo_status = FIFO_EMPTY;

		return 0;
	}

	/* fifo empty, nothing to read */
	return -1;
}

char fifo_writen(struct fifo_t *fifo, void *buf, int nbytes) {
	/*
	 *	write string of n bytes to the fifo
	 */

	for(int i = 0; i < nbytes; i++)
		if(fifo_write_byte(fifo, (unsigned char *) &buf[i]) < 0)
			return -1;

	return 0;
}

char fifo_readn(struct fifo_t *fifo, void *buf, int nbytes) {
	/*
	 *	read a string of n bytes to the fifo
	 */

	for(int i = 0; i < nbytes; i++)
		if(fifo_read_byte(fifo, (unsigned char *) &buf[i]) < 0)
			return -1;

	return 0;
}
