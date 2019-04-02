/*
 *	USC EE459 Spring 2019 Team 17 - ELM327 Communication
 */

#include "elm.h"

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

int elm_response_complete(const char *buf) {
  /*
   *	console will send a '>' once it is ready for the next command
   */

  if (strstr(buf, ">"))
    return 1;

  return 0;
}

int elm_write(int device, const char *buf, int len) {
  /*
   *	write n bytes from the buffer
   */

  int ret = 0;

  while (len > 0) {

    ret = write(device, buf, len);
    if (ret < 0)
      return -1;

    len -= ret;
  }

  /* why was this here ? */
  // write(device, "\r", 1);

  return 0;
}

int elm_read(int device, char *buf, int len) {
  /*
   *	read n bytes and put into the buffer
   */

  char *cur = buf;
  int ret = 0;
  int n;

  while (len) {
    n = read(device, cur, 1);

    cur += n;
    ret += n;
    len -= n;

    *cur = '\0';

    if (elm_response_complete(buf))
      break;
  }

  return 0;
}

int elm_command(int device, const char *cmd, int len, char *buf, int size) {
  /*
   *	send the command and then get the raw bytes until the '>', or tout
   */

  if (elm_write(device, cmd, len) < 0)
    return -1;

  return (elm_read(device, buf, size));
}

int initialize_elm(int device) {

  char buf[BUF_SIZE];

  if (elm_command(device, ELM_RESET, sizeof(ELM_RESET), buf, sizeof(buf)) < 0)
    return -1;

  printf("ATZ: %s\n", buf);

  if (elm_command(device, ECHO_OFF, sizeof(ECHO_OFF), buf, sizeof(buf)) < 0)
    if (!strstr(buf, OK))
      return -1;

  printf("ATE0: %s\n", buf);

  if (elm_command(device, SEARCH_BUS, sizeof(SEARCH_BUS), buf, sizeof(buf)) < 0)
    if (!strstr(buf, OK))
      return -1;

  printf("ATSP0: %s\n", buf);

  return 0;
}
