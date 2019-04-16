/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */

// the code is very simple
// hi its hanna
// hello it's paul

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

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
#include "elm.h"
#include "obd.h"
#include "astdio.h"

struct elm_ctx elm;

volatile struct fifo_t fifo;

struct obd_ctx obd;
volatile int interrupt_flag = 0;

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


extern FILE *stdout;

int main() {

  /* atmel hardware */
  sei();

  initialize_pins();

  initialize_uart(UART_PORT_0, MYUBRR(BUAD_UART_0));

  UART_DBG("main: initialized uart\r\n");

  initialize_stdio(&stdout);

  DBG("main: initialized stdout\n");

  initialize_uart(UART_PORT_1, MYUBRR(BUAD_UART_1));

  UCSR1B |= (1 << RXCIE1);

  DBG("main: initialized AT port\n");

  initialize_fifo(&fifo);

  DBG("main: initialized fifo\n");

  initialize_spi();

  DBG("main: initialized spi\n");

  /* software resources */
  if (initialize_sd(&sd) < 0)
    DBG("main: unable to initialize sd\n");
  else
    DBG("main: initialized sd\n");

  if (initialize_io(&io, &sd) < 0)
    DBG("main: unable to initialize io\n");
  else
    DBG("main: initialized io\n");

  if (initialize_fat32(&fat32, &io, &sd) < 0)
    DBG("main: unable to initialize fat32\n");
  else
    DBG("main: initialized fat32\n");

  /* main routines */
  if (initialize_elm(&fifo, &elm, UART_PORT_1) < 0) 
    DBG("main: unable to initialize elm\n");
  else
    DBG("main: initialized elm\n");

  if (initialize_obd(&elm, &obd) < 0)
    DBG("main: unable to initialize obd\n");
  else
    DBG("main: initialized obd\n");


  spi_device_enable(SPI_LCD);
  DELAY_MS(10);
  spi_write_char(0xFE);
  spi_write_char(0x41);
  DELAY_MS(10);
  spi_device_disable(SPI_LCD);
  spi_device_enable(SPI_LCD);
  spi_write_char(0xFE);
  spi_write_char(0x53);
  spi_write_char(0x08);
  spi_device_disable(SPI_LCD);

  char buf[BUF_SIZE];

  while (1) {
    spi_write_char(0xFF);
    spi_device_enable(SPI_LCD);
    spi_write_char('U');
    _delay_us(5);
    spi_write_char(0xFF);
    spi_device_disable(SPI_LCD);
    // struct node *ptr = obd.linked_list;

    // while(ptr) {

    //   /* the compiler needs offset information to dereference void pointers */
    //   struct obd_cmd *cmd = (struct obd_cmd *) ptr->data;

    //   char ret[64];

    //   if(cmd->handle_data != NULL) {
    //     if(obd_command(&obd, cmd->obd_cmd, buf, BUF_SIZE) < 0) {
    //       ptr = ptr->next;
    //       continue;
    //     }
    //     (*(cmd->handle_data))(ret, buf, cmd->resp_bytes, &obd);
    //     DBG("main: %s = %s %s\n", cmd->cmd_str, ret, cmd->obd_units);
    //   }
    //   ptr = ptr->next;
    // }
    
    PORTB ^= (1 << 0);
    DELAY_MS(1000);
  }

  return (0);
}

ISR(USART1_RX_vect) {
  fifo_write_byte(&fifo, &UDR1);
}
