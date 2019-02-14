#ifndef FIFO_H
#define FIFO_H

/* for testing */
#define DEBUG

#include <stdio.h>

/*
 *	    fifo.h
 */

/* fifo is statically allocated */
#define FIFO_SIZE 16

static const enum {
	FIFO_FULL,
	FIFO_EMPTY,
	FIFO_READY,
};

struct fifo_t{
	char f_buf[FIFO_SIZE];
	unsigned char f_head;
	unsigned char f_tail;
	unsigned char f_status;
};

/*
 *	set the head and tail for the fifo structure
 */
void initialize_fifo(struct fifo_t *fifo);

#ifdef DEBUG
/*
 *
 */
void fifo_dump_buf(struct fifo_t *fifo, char *ret, int size);


/*
 *	debug
 */
void fifo_dump_mem(struct fifo_t *fifo, FILE *stream);

#endif

/*
 *	write string of n bytes to the fifo
 */
char fifo_writen(struct fifo_t *fifo, void *buf, int nbytes);

/*
 *	read a string of n bytes to the fifo
 */
char fifo_readn(struct fifo_t *fifo, void *buf, int nbytes);

#endif
