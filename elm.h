/*
 *	USC EE459 Spring 2019 Team 17 - ELM327 Header
 */

#ifndef ELM_H
#define ELM_H

#include "fifo.h"

#define BUF_SIZE 128

struct elm_ctx {
	struct fifo_t *fifo;
	uint8_t uart_dev;
};

int initialize_elm(struct fifo_t *fifo, struct elm_ctx *elm, uint8_t device);
// int elm_write(uint8_t device, const char *buf);
// int elm_command(int device, const char *cmd, int len, char *buf, int size);

#endif
