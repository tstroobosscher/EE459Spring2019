/*
 *	USC EE459 Spring 2019 Team 17 - FIFO Header
 */

#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>

#include "debug.h"

/* fifo is statically allocated */
#define FIFO_SIZE 32

static const enum {
  FIFO_FULL,
  FIFO_EMPTY,
  FIFO_READY,
} fifo_status;

struct fifo_t {
  uint8_t f_buf[FIFO_SIZE];
  uint8_t f_head;
  uint8_t f_tail;
  uint8_t f_status;
};

char fifo_write_byte(struct fifo_t *fifo, unsigned char *byte);
char fifo_read_byte(struct fifo_t *fifo, unsigned char *byte);
void fifo_dump_mem(struct fifo_t *fifo);

/*
 *	set the head and tail for the fifo structure
 */
void initialize_fifo(struct fifo_t *fifo);

/*
 *	write string of n bytes to the fifo
 */
char fifo_writen(struct fifo_t *fifo, void *buf, int nbytes);

/*
 *	read a string of n bytes to the fifo
 */
char fifo_readn(struct fifo_t *fifo, void *buf, int nbytes);

#endif
