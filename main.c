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

  char *str = 
	"1606\n"
	"THE TRAGEDY OF MACBETH\n"
	"\n"
	"\n"
	"by Walliam Shakespeare\n"
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

  /* main routines */
  if(sd_put_sector(0x00007E78, str, 512) < 0)
    UART_DBG("main: unable to write block\r\n");
  else
    UART_DBG("main: write block successful\r\n");

  while(sd_is_busy()){
  }
  
loop:

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
