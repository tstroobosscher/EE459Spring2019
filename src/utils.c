/*
 *	USC EE459 Spring 2019 Team 17 - Utils
 */

#include <stdio.h>
#include <ctype.h>

#include "utils.h"
#include "debug.h"

/**
 * @brief      take 4 8 bit chars and return a 32 bit number of those bytes
 *
 * @param      arg0  bits 31 - 24
 * @param      arg1  bits 23 - 16
 * @param      arg2  bits 15 - 8
 * @param      arg3  bits 7 - 0
 *
 * @return     32 bit number corresponding to the combination of the arguments
 */
uint32_t bind_args(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3) {

  /* convert to 32 bit buffers */
  uint32_t args[4] = {arg0, arg1, arg2, arg3};

  return ((args[0] << 24) | (args[1] << 16) | (args[2] << 8) | (args[3]));
}

/**
 * @brief      performs a linear search on a buffer
 *
 * @param      byte  the byte to search for
 * @param      buf   the buffer to search through
 * @param      size  the size of the buffer
 *
 * @return     returns true if byte is located in the specified buffer
 */
int8_t byte_in_arr(uint8_t byte, void *buf, uint32_t size) {
  /*
   *	return pos of byte in arr, -1 if not
   */

  for (uint32_t i = 0; i < size; i++) {
    /* dereferencing a void pointer is undefined */
    uint8_t *ch = (uint8_t *)buf;
    if (byte == *ch)
      return i;
    buf++;
  }
  return -1;
}

/**
 * @brief      dump a series of bytes as hex characters 
 *
 * @param      port    the uart port to output to
 * @param      buf     the buffer to print
 * @param      nbytes  number of bytes to print
 */
void dump_nbytes(uint8_t port, void *buf, uint16_t nbytes) {
  uint8_t *ptr = (uint8_t *)buf;

  for (uint16_t i = 0; i < nbytes; i++) {
    uint8_t hex[6];
    snprintf(hex, 6, "0x%02X ", ptr[i]);
    uart_write_str(port, hex);
  }
  uart_write_str(port, "\n\r");
}


/**
 * @brief      performs a hexdump starting at the provided void pointer
 *             address and runs for nbytes long
 *
 * @param      port    the uart port to output to
 * @param      bin     the pointer to the address to be read
 * @param      nbytes  the number of bytes to read
 */
void dump_bin(uint8_t port, void *bin, uint16_t nbytes) {
  uint8_t *ptr = (uint8_t *)bin;

  const uint8_t buf_size = 18;

  int8_t ascii[buf_size];
  uint16_t i;

  uart_write_str(port, "HEXDATA\r\n\r\n");

  for (i = 0; i < nbytes; i++) {
    if (!(i % 16)) {
      if (i != 0)
        uart_write_str(port, "  ");
      uart_write_str(port, ascii);
      uart_write_str(port, "\r\n");

      /* output offset */
      uart_write_str(port, "  ");
      uart_write_hex(port, i >> 8);
      uart_write_hex(port, i);
      uart_write_str(port, "  ");
    }

    uart_write_str(port, " ");
    uart_write_hex(port, ptr[i]);

    if (!isprint(ptr[i]))
      ascii[i % 16] = '.';
    else
      ascii[i % 16] = ptr[i];

    ascii[(i % 16) + 1] = '\0';
  }

  while ((i % 16) != 0) {
    uart_write_str(port, " ");
    i++;
  }

  uart_write_str(port, "  ");
  uart_write_str(port, ascii);
  uart_write_str(port, "\r\n");
}
