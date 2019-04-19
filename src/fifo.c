/*
 *	USC EE459 Spring 2019 Team 17 - FIFO Routines
 */

#include "fifo.h"
#include "debug.h"
#include "utils.h"

/**
 * @brief      initialize the given fifo structure
 *
 * @param      fifo  pointer to the fifo
 */
void initialize_fifo(struct fifo_t *fifo) {
  /*
   *	set the head, tail, and status for the fifo structure
   */

  fifo->f_head = 0;
  fifo->f_tail = 0;
  fifo->f_status = FIFO_EMPTY;
  return;
}

<<<<<<< HEAD:fifo.c
/**
 * @brief      write a byte to the fifo, move the head up
 *
 * @param      fifo  pointer to the fifo
 * @param      byte  the byte to read from
 *
 * @return     return success or failure
 */

int8_t fifo_write_byte(struct fifo_t *fifo, unsigned char *byte) {
=======
char fifo_write_byte(struct fifo_t *fifo, char *byte) {
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
>>>>>>> master:src/fifo.c

  /* fifo ready or empty */
  if (fifo->f_status != FIFO_FULL) {

    /* head should be pointing to next available spot */
    fifo->f_buf[fifo->f_head] = *byte;

    /* increment, wrap around buf */
    fifo->f_head++;
    if (fifo->f_head == FIFO_SIZE)
      fifo->f_head = 0;

    /* check buf status */
    if (fifo->f_head == fifo->f_tail)
      fifo->f_status = FIFO_FULL;
    else
      fifo->f_status = FIFO_READY;

    return 0;
  }

  /* fifo full */
  return -1;
}

<<<<<<< HEAD:fifo.c
/**
 * @brief      read a single byte from the fifo, move the tail up
 *
 * @param      fifo  pointer to the fifo
 * @param      byte  the byte to read the data into
 *
 * @return     return success or failure
 */
int8_t fifo_read_byte(struct fifo_t *fifo, uint8_t *byte) {
=======
char fifo_read_byte(struct fifo_t *fifo, char *byte) {
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
>>>>>>> master:src/fifo.c

  /* fifo ready or full */
  if (fifo->f_status != FIFO_EMPTY) {

    /* tail should be pointing at next available byte to read */
    *byte = fifo->f_buf[fifo->f_tail];

    /* increment tail, wrap around */
    fifo->f_tail++;
    if (fifo->f_tail == FIFO_SIZE)
      fifo->f_tail = 0;

    /* check buf status */
    if (fifo->f_tail == fifo->f_head)
      fifo->f_status = FIFO_EMPTY;

    return 0;
  }

  /* fifo empty, nothing to read */
  return -1;
}

/**
 * @brief      dump the contents of the fifo
 *
 * @param      fifo  pointer to the fifo
 */
void fifo_dump_mem(struct fifo_t *fifo) {

  printf("fifo status: ");
  switch(fifo->f_status) {
    case FIFO_FULL  :
      printf("full\n");
      break;
    case FIFO_EMPTY :
      printf("empty\n");
      break;
    case FIFO_READY :
      printf("ready\n");
      break;
    default     :
      printf("unhandled status\n");
      break; 
  }

  for(int i = 0; i < ARRAY_SIZE(fifo->f_buf); i++){
<<<<<<< HEAD:fifo.c
    printf("fifo address: 0x%X data: 0x%X ", i, fifo->f_buf[i]);
    if(fifo->f_head == i)
      printf(" <- head");
    if(fifo->f_tail == i)
     printf(" <- tail");
=======
    printf("fifo address: %d data: %d ", i, fifo->f_buf[i]);
    if(fifo->f_head == i)
      printf(" <- head");
    if(fifo->f_tail == i)
      printf(" <- tail");
>>>>>>> master:src/fifo.c

    printf("\n");
  }

  return;
}
