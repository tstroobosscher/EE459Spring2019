/*
 *	USC EE459 Spring 2019 Team 17 - Pins header
 */

#ifndef PINS_H
#define PINS_H

/* milliseconds */
#define PULSE_WIDTH_MS 100

#include "debug.h"

/* DDRX setting */
static const enum {
  ATMEL_INPUT,
  ATMEL_OUTPUT,
  ATMEL_DIR_INVAL,
} atmel_328_ddr_status;

/* Internal Pull-Ups */
static const enum {
  ATMEL_PULL_ENA,
  ATMEL_PULL_DIS,
  ATMEL_PULL_INVAL,
} atmel_328_pull_status;

#if defined ATMEGA328

#define PIN_RX 2
#define PIN_TX 3
#define PIN_SS_SD 16
#define PIN_MOSI 17
#define PIN_MISO 18 /* input ! */
#define PIN_SCK 19

#elif defined ATMEGA2560

#define PIN_RX 2
#define PIN_TX 3
#define PIN_SS_SD 19
#define PIN_MOSI 21
#define PIN_MISO 22 /* input ! */
#define PIN_SCK 20

#elif defined ATMEGA1284

#define PIN_RX_0 14
#define PIN_TX_0 15
#define PIN_RX_1 16
#define PIN_TX_1 17
#define PIN_LCD_SS 4
#define PIN_SS_SD 5
#define PIN_MOSI 6
#define PIN_MISO 7 /* input ! */
#define PIN_SCK 8

#endif

void initialize_pins();
int8_t pin_get(uint8_t pin);
int8_t pin_high(uint8_t pin);
int8_t pin_low(uint8_t pin);
void pulse_pin(uint8_t pin);
void cycle_pins();

#endif
