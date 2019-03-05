/*
 *	USC EE459 Spring 2019 Team 17 - Utils
 */

#include "utils.h"
#include "debug.h"

/* gnu avr gcc does not like this function - very weak; write avr est case */
uint32_t bind_args(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3) {

  /* convert to 32 bit buffers */
  uint32_t args[4] = {arg0, arg1, arg2, arg3};

  return ((args[0] << 24) | (args[1] << 16) | (args[2] << 8) | (args[3]));
}

int8_t byte_in_arr(uint8_t byte, void *buf, uint32_t size) {
  /*
   *	return pos of byte in arr, 0 if not, logic is inconsistent but
   *	its readable
   */

  for (uint32_t i = 0; i < size; i++) {
    /* dereferencing a void pointer is undefined */
    uint8_t *ch = (uint8_t *)buf;
    if (byte == *ch)
      return 1;
    buf++;
  }
  return 0;
}

void dump_nbytes(void *buf, uint16_t nbytes) {
  uint8_t *ptr = (uint8_t *)buf;

  for (uint16_t i = 0; i < nbytes; i++) {
    uint8_t hex[6];
    snprintf(hex, 6, "0x%02X ", ptr[i]);
    uart_write_str(hex);
  }
  uart_write_str("\n\r");
}

void dump_bin(void *bin, uint16_t nbytes) {
  uint8_t *ptr = (uint8_t *)bin;

  const uint8_t buf_size = 18;

  uint8_t ascii[buf_size];
  uint16_t i;

  uart_write_str("HEXDATA\r\n\r\n");

  for (i = 0; i < nbytes; i++) {
    if (!(i % 16)) {
      if (i != 0)
        uart_write_str("  ");
      uart_write_str(ascii);
      uart_write_str("\r\n");

      /* output offset */
      uart_write_str("  ");
      uart_write_hex(i >> 8);
      uart_write_hex(i);
      uart_write_str("  ");
    }

    uart_write_str(" ");
    uart_write_hex(ptr[i]);

    if (!isprint(ptr[i]))
      ascii[i % 16] = '.';
    else
      ascii[i % 16] = ptr[i];

    ascii[(i % 16) + 1] = '\0';
  }

  while ((i % 16) != 0) {
    uart_write_str(" ");
    i++;
  }

  uart_write_str("  ");
  uart_write_str(ascii);
  uart_write_str("\r\n");
}

void dump_byte(uint8_t byte) {
  uint8_t hex[6];
  snprintf(hex, 6, "0x%02X ", byte);
  uart_write_str(hex);
}

void trace() { uart_write_str("trace\r\n"); }

  // void log(char *msg) {
  // 	uart_write_str(msg);
  // 	uart_write_str("\n\r");
  // }

  /* TODO: flush uart buffers */
