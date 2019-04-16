/*
 *	USC EE459 Spring 2019 Team 17 - ELM327 Communication
 */

#include <stdint.h>
#include <string.h>
#include <stdint.h>

#include "elm.h"
#include "uart.h"

static char *OK = "OK";

static char *ELM_RESET = "ATZ\r";
static char *ECHO_OFF = "ATE0\r";
static char *SEARCH_BUS = "ATSP0\r";


/*
 *	The idea for this file is to provide a layer of abstraction between the 
 *	obd code and the physical chip. all this layer needs to do to provide
 *	byte level read and write to the obd code. This will make testing between 
 *	the device and the laptops easier since all we need to recode is the byte
 *	oriented reads and writes.
 *	
 *	All the obd code should be doing is passing in argument strings (ex. "0100")
 *	and reading the response bytes. The ELM code has to do all the formatting 
 *	to convert from UART ascii to actual bytes of data.
 */

static int elm_response_complete(char *buf) {
  /*
   *	console will send a '>' once it is ready for the next command
   */

  if (strstr(buf, ">"))
    return 1;

  return 0;
}

int elm_write(struct elm_ctx *elm, char *buf) {
  /*
   *	write n bytes from the buffer
   */

  uart_write_str(elm->uart_dev, buf);

  /* why was this here ? */
  // write(device, "\r", 1);

  return 0;
}

static int elm_read(struct elm_ctx *elm, char *buf, int len) {
  /*
   *	read n bytes and put into the buffer
   */
  
  memset(buf, 0, len);

  char *cur = buf;
  char ch;

  uint8_t ret = 0;

  while (len) {

    if (elm->fifo->f_status == FIFO_EMPTY)
      continue;

    fifo_read_byte(elm->fifo, &ch);

    *cur = ch;

    UART_DBG("elm: read buf: ");
    UART_DBG(buf);
    UART_DBG("\r\n");

    cur++;
    ret++;
    len--;

    *cur = '\0';

    if (elm_response_complete(buf))
      break;
  }

  return ret;
}

int elm_command(struct elm_ctx *elm, char *cmd, int len, char *buf, int size) {
  /*
   *	send the command and then get the raw bytes until the '>', or tout
   */
  DBG("elm: writing cmd: %s\n", cmd);
  if (elm_write(elm, cmd) < 0)
    return -1;

  return (elm_read(elm, buf, size));
}

int initialize_elm(struct fifo_t *device, struct elm_ctx *elm, uint8_t uart) {

  elm->fifo = device;
  elm->uart_dev = uart;

  char buf[ELM_BUF_SIZE];

  DBG("elm: resetting device\n");
  if (elm_command(elm, ELM_RESET, strlen(ELM_RESET), buf, BUF_SIZE) < 0)
    return -1;

  DBG("elm: reset successful\n");
  DBG("ATZ: %s\n", buf);

  DBG("elm: turning off echo\n");
  if (elm_command(elm, ECHO_OFF, strlen(ECHO_OFF), buf, BUF_SIZE) < 0)
    if (!strstr(buf, OK))
      return -1;
  DBG("elm: echo turned off\n");

  DBG("ATE0: %s\n", buf);

  if (elm_command(elm, SEARCH_BUS, strlen(SEARCH_BUS), buf, BUF_SIZE) < 0)
    if (!strstr(buf, OK))
      return -1;

  DBG("ATSP0: %s\n", buf);

  return 0;
}
