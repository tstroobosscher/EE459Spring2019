/*
 *	USC EE459 Spring 2019 Team 17 - FIFO Header
 */

#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>

#include "debug.h"

/** 
 * @brief      fifo is statically allocated 
 */
#define FIFO_SIZE 64

/**
 * @brief      fifo states
 */
static const enum {
  FIFO_FULL,
  FIFO_EMPTY,
  FIFO_READY,
} fifo_status;

/**
 * @brief      fifo structure
 */
struct fifo_t {
  uint8_t f_buf[FIFO_SIZE];
  uint8_t f_head;
  uint8_t f_tail;
  uint8_t f_status;
};

char fifo_write_byte(struct fifo_t *fifo, char *byte);
char fifo_read_byte(struct fifo_t *fifo, char *byte);
void fifo_dump_mem(struct fifo_t *fifo);
void initialize_fifo(struct fifo_t *fifo);

#endif
