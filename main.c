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

  /* atmel hardware */
  initialize_pins();

  UART_DBG("main: initialized pins\r\n");

  initialize_uart(MYUBRR);

  UART_DBG("main: initialized uart\r\n");

  // initialize_spi();

  // UART_DBG("main: initialized spi\r\n");

  // /* software resources */
  // if (initialize_sd(&sd) < 0)
  //   UART_DBG("main: unable to initialize sd\r\n");
  // else
  //   UART_DBG("main: initialized sd\r\n");

  // if (initialize_io(&io, &sd) < 0)
  //   UART_DBG("main: unable to initialize io\r\n");
  // else
  //   UART_DBG("main: initialized io\r\n");

  // if (initialize_fat32(&fat32, &io, &sd) < 0)
  //   UART_DBG("main: unable to initialize fat32\r\n");
  // else
  //   UART_DBG("main: initialized fat32\r\n");

  // /* main routines */
  // struct fat32_file file;

  // strncpy(file.file_name, "logfile2", 8);
  // strncpy(file.file_ext, "txt", 3);

  // fat32_creat_file(&fat32, &file);

  // io_flush_write_buffer(&io);

  // uint32_t address = fat32.root_dir_sector;
  // UART_DBG_32(address);
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
