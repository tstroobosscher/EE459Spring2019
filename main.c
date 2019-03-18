/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */

// the code is very simple
// hi its hanna
// hello it's paul

#include <avr/io.h>
#include <stdint.h>

#include "debug.h"
#include "fat.h"
#include "fifo.h"
#include "io.h"
#include "pins.h"
#include "sd.h"
#include "spi.h"
#include "time.h"
#include "utils.h"
#include "uart.h"

/*
 *  okay, so multiplication on 8 bit architectures works a bit differently
 *  than you would expect. a buffer is assigned to the value being
 *  operated on, and the size that buffer may be inadiquate if it is not
 *  stated explicitely.
 */

/*
 *	write debug strings in the header that can be placed in program
 *	memory and then referenced by macro
 */

struct FAT32Entry e;

/*
 *  Can we push these valeus into successive data structures so we aren't
 *  keeping stale data in the dataspace? What do we actually need?
 */

struct sd_ctx sd;

/*
 *  the io level should also be global because a single io structure
 *  can operate on more than one instance of a file
 */
struct io_ctx io;

/*
 *  fat32 context should be global because we can associate more than
 *  one file with a single file system
 */
struct fat32_ctx fat32;

struct fat32_file file;

int main() {

  char ch = 'X';

  char str[] =
      "1606\n"
      "THE TRAGEDY OF MACBETH\n"
      "\n"
      "\n"
      "by William Shakespeare\n"
      "\n"
      "\n"
      "\n"
      "Dramatis Personae\n"
      "\n"
      "  DUNCAN, King of Scotland\n"
      "  MACBETH, Thane of Glamis and Cawdor, a general in the King's\n"
      "army\n"
      "  LADY MACBETH, his wife\n"
      "  MACDUFF, Thane of Fife, a nobleman of Scotland\n"
      "  LADY MACDUFF, his wife\n"
      "  MALCOLM, elder son of Duncan\n"
      "  DONALBAIN, younger son of Duncan\n"
      "  BANQUO, Thane of Lochaber, a general in the King's army\n"
      "  FLEANCE, his son\n"
      "  LENNOX, nobleman of Scotland\n"
      "  ROSS, nobleman of Scotland\n"
      "  MENTEITH nobleman of Scotland\n"
      "  ANGUS,";

  /* atmel hardware */
  initialize_pins();

  UART_DBG("main: initialized pins\r\n");

  initialize_uart(MYUBRR);

  UART_DBG("main: initialized uart\r\n");

  initialize_spi();

  UART_DBG("main: initialized spi\r\n");

  /* software resources */
  if (initialize_sd(&sd) < 0)
    UART_DBG("main: unable to initialize sd\r\n");
  else
    UART_DBG("main: initialized sd\r\n");

  if (initialize_io(&io, &sd) < 0)
    UART_DBG("main: unable to initialize io\r\n");
  else
    UART_DBG("main: initialized io\r\n");

  if (initialize_fat32(&fat32, &io, &sd) < 0)
    UART_DBG("main: unable to initialize fat32\r\n");
  else
    UART_DBG("main: initialized fat32\r\n");

  /* main routines */

  struct fat32_ctx file;

  uart_write_str("Hello World!\r\n");
  //fat32_creat_file(&fat32, &file);

  uint32_t res = 3;

  struct FAT32Entry e;
  memset(&e, 0, sizeof(struct FAT32Entry));

  strncpy(e.filename, "logfile1", 8);
  strncpy(e.filename_ext, "txt", 3);

  e.first_cluster_addr_high = (res >> 16);
  e.first_cluster_addr_low = (res);
  
  e.file_size = 0;

  uint32_t ret = 1;

  io_write_nbytes(fat32.io, &e, fat32.root_dir_sector * SECTOR_SIZE + 32 * ret, sizeof(struct FAT32Entry));

  io_flush_write_buffer(fat32.io);
  /* strncpy(io.output_sector_buf, str, 512); */

  /* io.output_sector_addr = 0x7E78; */

  /* if(sd_put_sector(0x00007E78, io.output_sector_buf, 512) < 0) */
  /*   UART_DBG("main: unable to write block\r\n"); */
  /* else */
  /*   UART_DBG("main successfully wrote block\r\n"); */

  /* if(io_put_byte(&io, ((uint32_t) 0x00007E78 * (uint32_t) 512), &ch) < 0) */
  /*   UART_DBG("main: unable to write byte\r\n"); */
  /* else */
  /*   UART_DBG("main: write byte successful\r\n"); */

  /* if(io_flush_write_buffer(&io) < 0) */
  /*   UART_DBG("main: unable to flush write buffer\r\n"); */
  /* else */
  /*   UART_DBG("main: successfully flushed write buffer\r\n"); */

  while (1) {
    if (pin_high(26) < 0)
      UART_DBG("pin_high error\r\n");
    DELAY_MS(1000);
    if (pin_low(26) < 0)
      UART_DBG("pin_low error\r\n");
    DELAY_MS(1000);
  }

  return (0);
}
