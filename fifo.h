#ifndef FIFO_H
#define FIFO_H

/*
 *	    Fifo
 */

#define FIFO_SIZE 16

struct {
	char buf[FIFO_SIZE];
	unsigned char head;
	unsigned char tail;
} fifo_t;

/*
 *	set the struct pointers and size information for the fifo structure
 */
void fifo_init(struct fifo_t *fifo, char *buf, int size);

/*
 *	write a single byte to the fifo, increment the head
 *	if fifo is full, return -1
 *	return 0 on success
 *
 *	param: fifo is the fifo struct pointer
 *	param: byte is value-result argument
 */
char fifo_write_byte(struct fifo_t *fifo, unsigned char *byte);

/*
 *	read a single byte the fifo, increment the tail
 *	if fifo is empty, return -1
 *	return 0 on success, assign reference value the read byte
 *
 *	param: fifo is the fifo struct pointer
 *	param: byte is value-result argument
 */
char fifo_read_byte(struct fifo_t *fifo, unsigned char *byte);

#endif
