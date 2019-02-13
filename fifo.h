#ifndef FIFO_H
#define FIFO_H

struct {
	char *buf;
	unsigned char size;
	unsigned char head;
	unsigned char tail;
} fifo;

#endif
