/*
 *	USC EE459 Spring 2019 Team 17 - ELM327 Communication
 */

#include <stdint.h>

#include "elm.h"
#include "uart.h"

static const char *OK = "OK";

static const char *ELM_RESET = "ATZ\r";
static const char *ECHO_OFF = "ATE0\r";
static const char *SEARCH_BUS = "ATSP0\r";

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

static int elm_response_complete(const char *buf) {
  /*
   *	console will send a '>' once it is ready for the next command
   */

  if (strstr(buf, ">"))
    return 1;

  return 0;
}

int elm_write(uint8_t device, const char *buf) {
  /*
   *	write n bytes from the buffer
   */

  uart_write_str(device, buf);

  /* why was this here ? */
  // write(device, "\r", 1);

  return 0;
}

static int elm_read(int device, char *buf, int len) {
  /*
   *	read n bytes and put into the buffer
   */
  
  memset(buf, 0, len);

  char *cur = buf;
  int ret = 0;
  int n;

  while (len) {

    if (!uart_data_available(device))
      continue;

    *cur = uart_read_char(device);

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

int elm_command(int device, const char *cmd, int len, char *buf, int size) {
  /*
   *	send the command and then get the raw bytes until the '>', or tout
   */
  UART_DBG("elm: writing cmd: ");
  UART_DBG(cmd);
  UART_DBG("\r\n");
  if (elm_write(device, cmd) < 0)
    return -1;

  return (elm_read(device, buf, size));
}

int initialize_elm(int device) {

  char buf[BUF_SIZE];

  UART_DBG("elm: resetting device\r\n");
  if (elm_command(device, ELM_RESET, strlen(ELM_RESET), buf, BUF_SIZE) < 0)
    return -1;

  UART_DBG("elm reset successful\r\n");

  // //printf("ATZ: %s\n", buf);

  if (elm_command(device, ECHO_OFF, strlen(ECHO_OFF), buf, BUF_SIZE) < 0)
    if (!strstr(buf, OK))
      return -1;

  // //printf("ATE0: %s\n", buf);

  // if (elm_command(device, SEARCH_BUS, sizeof(SEARCH_BUS), buf, sizeof(buf)) < 0)
  //   if (!strstr(buf, OK))
  //     return -1;

  // //printf("ATSP0: %s\n", buf);

  return 0;
}
