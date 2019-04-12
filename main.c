/*
 *	USC EE459 Spring 2019 Team 17 - Main
 */

// the code is very simple
// hi its hanna
// hello it's paul

#include <avr/interrupt.h>
#include <avr/io.h>
//#include <stdint.h>
#include <stdio.h>

#include "debug.h"
//#include "fat.h"
#include "fifo.h"
//#include "io.h"
#include "pins.h"
//#include "sd.h"
//#include "spi.h"
//#include "time.h"
#include "utils.h"
#include "uart.h"
#include "elm.h"
#include "obd.h"
#include "astdio.h"

// struct elm_ctx elm;

// volatile struct fifo_t fifo;

// struct obd_ctx obd;
// volatile int interrupt_flag = 0;

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

//struct sd_ctx sd;

/*
 *  the io level should also be global because a single io structure
 *  can operate on more than one instance of a file
 */
//struct io_ctx io;

/*
 *  fat32 context should be global because we can associate more than
 *  one file with a single file system
 */
//struct fat32_ctx fat32;

//char buffer[1];

extern FILE *stdout;

int main() {

  /* atmel hardware */
  initialize_pins();

  initialize_uart(UART_PORT_0, MYUBRR(BUAD_UART_0));

  UART_DBG("main: initialized uart\r\n");

  initialize_stdio(&stdout);

  // initialize_fifo(&fifo);

  DBG("Hello World! \n%s\n", "Goodbye World!");

  // UART_DBG("main: initialized fifo\r\n");

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

  // strncpy(file.file_name, "logfile1", 8);
  // strncpy(file.file_ext, "txt", 3);

  // fat32_creat_file(&fat32, &file);

  // char *buf_1 = "Hello World!\n";

  // fat32_write_file_nbytes(&fat32, &file, buf_1, strlen(buf_1));

  // char *buf_2 = "Goodbye World!\n";

  // fat32_write_file_nbytes(&fat32, &file, buf_2, strlen(buf_2));

  // char *buf_3 = "How Are You Doing World!\n";

  // fat32_write_file_nbytes(&fat32, &file, buf_3, strlen(buf_3));

  // char *buf_4 = "I Am Doing Well World!\n";

  // fat32_write_file_nbytes(&fat32, &file, buf_4, strlen(buf_4));

  // io_flush_write_buffer(&io);

  // initialize_uart(UART_PORT_1, MYUBRR(BUAD_UART_1));
  // sei();
  // UCSR1B |= (1 << RXCIE1);

  // if (initialize_elm(&fifo, &elm, UART_PORT_1) < 0) 
  //   UART_DBG("main: unable to initialize elm\r\n");
  // else
  //   UART_DBG("main: initialized elm\r\n");

  // if (initialize_obd(&elm, &obd) < 0)
  //   UART_DBG("main: unable to initialize obd\r\n");
  // else
  //   UART_DBG("main: initialized obd\r\n");

  // char buf[64];

  while (1) {
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
    //     UART_DBG(cmd->cmd_str);
    //     UART_DBG(" = ");
    //     UART_DBG(ret);
    //     UART_DBG(" ");
    //     UART_DBG(cmd->obd_units);
    //     UART_DBG("\r\n");
    //     //printf("%s = %s %s\n", cmd->cmd_str, res, cmd->obd_units);
    //   }
    //   ptr = ptr->next;
    // }
    // DELAY_MS(1000);
  }

  return (0);
}

// ISR(USART1_RX_vect) {
//   fifo_write_byte(&fifo, &UDR1);
// }
