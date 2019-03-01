/*
 *	USC EE459 Spring 2019 Team 17 - Timer routines
 */

#include <avr/io.h>
#include <stdint.h>

#include "time.h"
#include "uart.h"
#include "utils.h"

static uint16_t get_timer_val(struct atmel_328_time *t) {
  /* time ops must be atomic */
  switch (t->timer_reg) {
  case TIMER0:
    return TCNT0;
  case TIMER1:
    return TCNT1;
  }
}

static void set_time(struct atmel_328_time *t, uint16_t val) {
  switch (t->timer_reg) {
  case TIMER0:
    TCNT0 = val;
    return;
  case TIMER1:
    TCNT1 = val;
    return;
  }
}

void dump_time(struct atmel_328_time *t) {
  char *buf[16];
  snprintf(buf, 16, "%u", get_timer_val(t));
  uart_write_str(buf);
  uart_write_str("\r\n");
}

void timein(struct atmel_328_time *t) { set_time(t, 0); }

int8_t timeout(struct atmel_328_time *t, uint16_t timeout_val) {
  return (get_timer_val(t) >= timeout_val);
}

void initialize_timer(struct atmel_328_time *t, uint8_t timer_reg) {
  t->timer_reg = timer_reg;
  t->timer_value = 0;
  set_time(t, 0);
}
