/*
 *	USC EE459 Spring 2019 Team 17 - Utils
 */

#include <stdio.h>
#include <ctype.h>

#include "utils.h"
#include "debug.h"

/* gnu avr gcc does not like this function - very weak; write avr test case */
uint32_t bind_args(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3) {

  /* convert to 32 bit buffers */
  uint32_t args[4] = {arg0, arg1, arg2, arg3};

  return ((args[0] << 24) | (args[1] << 16) | (args[2] << 8) | (args[3]));
}

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

void dump_nbytes(uint8_t port, void *buf, uint16_t nbytes) {
  uint8_t *ptr = (uint8_t *)buf;

  for (uint16_t i = 0; i < nbytes; i++) {
    int8_t hex[6];
    snprintf((char *) hex, 6, "0x%02X ", ptr[i]);
    uart_write_str(port, hex);
  }
  uart_write_str(port, (int8_t *) "\n\r");
}

void dump_bin(uint8_t port, void *bin, uint16_t nbytes) {
  uint8_t *ptr = (uint8_t *) bin;

  const uint8_t buf_size = 18;

  int8_t ascii[buf_size];
  uint16_t i;

  uart_write_str(port, (int8_t *) "HEXDATA\r\n\r\n");

  for (i = 0; i < nbytes; i++) {
    if (!(i % 16)) {
      if (i != 0)
        uart_write_str(port, (int8_t *) "  ");
      uart_write_str(port, ascii);
      uart_write_str(port, (int8_t *) "\r\n");

      /* output offset */
      uart_write_str(port, (int8_t *) "  ");
      uart_write_hex(port, i >> 8);
      uart_write_hex(port, i);
      uart_write_str(port, (int8_t *) "  ");
    }

    uart_write_str(port, (int8_t *) " ");
    uart_write_hex(port, ptr[i]);

    if (!isprint(ptr[i]))
      ascii[i % 16] = '.';
    else
      ascii[i % 16] = ptr[i];

    ascii[(i % 16) + 1] = '\0';
  }

  while ((i % 16) != 0) {
    uart_write_str(port, (int8_t *) " ");
    i++;
  }

  uart_write_str(port, (int8_t *) "  ");
  uart_write_str(port, ascii);
  uart_write_str(port, (int8_t *) "\r\n");
}

void dump_byte(uint8_t port, uint8_t byte) {
  int8_t hex[6];
  snprintf((char *) hex, 6, "0x%02X ", byte);
  uart_write_str(port, hex);
}

void trace(uint8_t port) { uart_write_str(port, (int8_t *) "trace\r\n"); }

// void log(char *msg) {
// 	uart_write_str(msg);
// 	uart_write_str("\n\r");
// }

/* TODO: flush uart buffers */
