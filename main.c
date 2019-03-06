/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */

// the code is very simple
// hi its hanna
// hello it's paul

#include <avr/io.h>

#include "fat.h"
#include "fifo.h"
#include "io.h"
#include "pins.h"
#include "sd.h"
#include "spi.h"
#include "time.h"
#include "uart.h"
#include "utils.h"
#include "debug.h"

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

  /* atmel hardware */
  initialize_pins();

  UART_DBG("main: initialized pins\r\n");

  initialize_uart(MYUBRR);

  UART_DBG("main: initialized uart\r\n");

  initialize_spi();

  UART_DBG("main: initialized spi\r\n");

  /* software resources */
  if (initialize_sd(&sd) < 0)
    UART_DBG("main: unable to initialize SD\r\n");
  else 
    UART_DBG("main: initialized sd\r\n");

  if (initialize_io(&io, &sd) < 0)
    UART_DBG("main: unable to initialize IO\r\n");
  else
    UART_DBG("main: initialized IO\r\n");

  if (initialize_fat32(&fat32, &io, &sd) < 0)
    UART_DBG("main: unable to initialize FAT32\r\n");
  else
    UART_DBG("main: initialized fat32\r\n");

  if (sd.sd_status == SD_ENABLED) {
    for (int j = 0; j < 16; j++) {
      if (io_read_nbytes(&io, &e,
                         (fat32.root_dir_sector * sd.sd_sector_size) +
                             (j * sizeof(struct FAT32Entry)),
                         sizeof(struct FAT32Entry)) < 0) {
        UART_DBG("main: error reading FAT32 root entry\r\n");
        break;
      }

      if (fat32_parse_entry(&e) < 0)
        break;
      dump_bin(&e, sizeof(struct FAT32Entry));

      if((e.file_attr == FILENAME_NEVER_USED) || (e.file_attr == FILENAME_FILE_DELETED) || ((e.file_attr & 0x0F) == 0x0F))
        continue;

      uart_write_str("main: file\r\n");
      fat32_open_file(&fat32, &e, &io, &file);
      fat32_dump_file_meta(&file);

      fat32_close_file(&fat32, &file);
    }
  }

  while (1) {
    if(pin_high(26) < 0)
      UART_DBG("pin_high error\r\n");
    DELAY_MS(1000);
    if(pin_low(26) < 0)
      UART_DBG("pin_low error\r\n");
    DELAY_MS(1000);
  }

  return (0);
}
