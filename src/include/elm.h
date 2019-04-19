/*
 *	USC EE459 Spring 2019 Team 17 - ELM327 Header
 */

#ifndef ELM_H
#define ELM_H

#include "fifo.h"

#define ELM_BUF_SIZE 128

struct elm_ctx {
	struct fifo_t *fifo;
	uint8_t uart_dev;
};

int initialize_elm(struct fifo_t *fifo, struct elm_ctx *elm, uint8_t device);

#endif
