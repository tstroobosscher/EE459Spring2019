/*
 *	USC EE459 Spring 2019 Team 17 - Pins
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include <stdint.h>
#include <util/delay.h>

#include "debug.h"
#include "pins.h"
#include "utils.h"

#define PINDEF(PORTREG, PORTNUM, PINDIR, PULLUP, DIRREG)                       \
  { PORTREG, PORTNUM, PINDIR, PULLUP, DIRREG }

/* set pin behavior here (OUTPUT/INPUT), ENA/DIS internal pull ups */
static const struct atmel_328_pin {
  volatile uint8_t *port_reg;
  volatile uint8_t port_num;
  volatile uint8_t pin_dir;
  volatile uint8_t pull_up;
  volatile uint8_t *dir_reg;
} __attribute((packed)) atmega328_pins[] = {

#if defined ATMEGA2560 && defined ATMEGA328 ||                                 \
    defined ATMEGA328 && defined ATMEGA1284 ||                                 \
    defined ATMEGA1284 && defined ATMEGA2560
#error Cannot compile for multiple chips
#endif

#if defined ATMEGA328
    /* 0, no 0 pin, just used to align the array index with the pin number */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 1  */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 2  */
    PINDEF(&PORTD, 0, ATMEL_INPUT, ATMEL_PULL_DIS, &DDRD),
    /* 3  */
    PINDEF(&PORTD, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 4  */
    PINDEF(&PORTD, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 5  */
    PINDEF(&PORTD, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 6  */
    PINDEF(&PORTD, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 7  */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 8  */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 9  */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 10 */
    PINDEF(&PORTB, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 11 */
    PINDEF(&PORTD, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 12 */
    PINDEF(&PORTD, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 13 */
    PINDEF(&PORTD, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 14 */
    PINDEF(&PORTB, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 15 */
    PINDEF(&PORTB, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 16 */
    PINDEF(&PORTB, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 17 */
    PINDEF(&PORTB, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 18, MISO */
    PINDEF(&PORTB, 4, ATMEL_INPUT, ATMEL_PULL_ENA, &DDRB),
    /* 19 */
    PINDEF(&PORTB, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 20 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 21 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 22 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 23 */
    PINDEF(&PORTC, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 24 */
    PINDEF(&PORTC, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 25 */
    PINDEF(&PORTC, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 26 */
    PINDEF(&PORTC, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 27 */
    PINDEF(&PORTC, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 28 */
    PINDEF(&PORTC, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),

#elif defined ATMEGA2560
    /* 0 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 1 */
    PINDEF(&PORTG, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRG),
    /* 2, RXD0 */
    PINDEF(&PORTE, 0, ATMEL_INPUT, ATMEL_PULL_DIS, &DDRE),
    /* 3, TXD0 */
    PINDEF(&PORTE, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRE),
    /* 4 */
    PINDEF(&PORTE, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRE),
    /* 5 */
    PINDEF(&PORTE, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRE),
    /* 6 */
    PINDEF(&PORTE, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRE),
    /* 7 */
    PINDEF(&PORTE, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRE),
    /* 8 */
    PINDEF(&PORTE, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRE),
    /* 9 */
    PINDEF(&PORTE, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRE),
    /* 10 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 11 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 12 */
    PINDEF(&PORTH, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 13 */
    PINDEF(&PORTH, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 14 */
    PINDEF(&PORTH, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 15 */
    PINDEF(&PORTH, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 16 */
    PINDEF(&PORTH, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 17 */
    PINDEF(&PORTH, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 18 */
    PINDEF(&PORTH, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 19, Slave Select SD */
    PINDEF(&PORTB, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 20, SCK */
    PINDEF(&PORTB, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 21, MOSI */
    PINDEF(&PORTB, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 22, MISO */
    PINDEF(&PORTB, 3, ATMEL_INPUT, ATMEL_PULL_ENA, &DDRB),
    /* 23 */
    PINDEF(&PORTB, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 24 */
    PINDEF(&PORTB, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 25 */
    PINDEF(&PORTB, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 26, Arduino Mega D13 - LED */
    PINDEF(&PORTB, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 27 */
    PINDEF(&PORTH, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRH),
    /* 28 */
    PINDEF(&PORTG, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRG),
    /* 29 */
    PINDEF(&PORTG, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRG),
    /* 30 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 31 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 32 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 33 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 34 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 35 */
    PINDEF(&PORTL, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRG),
    /* 36 */
    PINDEF(&PORTL, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRL),
    /* 37 */
    PINDEF(&PORTL, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRL),
    /* 38 */
    PINDEF(&PORTL, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRL),
    /* 39 */
    PINDEF(&PORTL, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRL),
    /* 40 */
    PINDEF(&PORTL, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRL),
    /* 41 */
    PINDEF(&PORTL, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRL),
    /* 42 */
    PINDEF(&PORTL, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRL),
    /* 43 */
    PINDEF(&PORTD, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 44 */
    PINDEF(&PORTD, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 45, RX1 */
    PINDEF(&PORTD, 2, ATMEL_INPUT, ATMEL_PULL_DIS, &DDRD),
    /* 46, TX1 */
    PINDEF(&PORTD, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 47 */
    PINDEF(&PORTD, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 48 */
    PINDEF(&PORTD, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 49 */
    PINDEF(&PORTD, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 50 */
    PINDEF(&PORTD, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 51 */
    PINDEF(&PORTG, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRG),
    /* 52 */
    PINDEF(&PORTG, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRG),
    /* 53 */
    PINDEF(&PORTC, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 54 */
    PINDEF(&PORTC, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 55 */
    PINDEF(&PORTC, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 56 */
    PINDEF(&PORTC, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 57 */
    PINDEF(&PORTC, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 58 */
    PINDEF(&PORTC, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 59 */
    PINDEF(&PORTC, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 60 */
    PINDEF(&PORTC, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 61 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 62 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 63 */
    PINDEF(&PORTJ, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 64 */
    PINDEF(&PORTJ, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 65 */
    PINDEF(&PORTJ, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 66 */
    PINDEF(&PORTJ, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 67 */
    PINDEF(&PORTJ, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 68 */
    PINDEF(&PORTJ, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 69 */
    PINDEF(&PORTJ, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 70 */
    PINDEF(&PORTG, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRG),
    /* 71 */
    PINDEF(&PORTA, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 72 */
    PINDEF(&PORTA, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 73 */
    PINDEF(&PORTA, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 74 */
    PINDEF(&PORTA, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 75 */
    PINDEF(&PORTA, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 76 */
    PINDEF(&PORTA, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 77 */
    PINDEF(&PORTA, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 78 */
    PINDEF(&PORTA, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 79 */
    PINDEF(&PORTJ, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRJ),
    /* 80 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 81 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 82 */
    PINDEF(&PORTK, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 83 */
    PINDEF(&PORTK, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 84 */
    PINDEF(&PORTK, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 85 */
    PINDEF(&PORTK, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 86 */
    PINDEF(&PORTK, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 87 */
    PINDEF(&PORTK, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 88 */
    PINDEF(&PORTK, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 89 */
    PINDEF(&PORTK, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRK),
    /* 90 */
    PINDEF(&PORTF, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 91 */
    PINDEF(&PORTF, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 92 */
    PINDEF(&PORTF, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 93 */
    PINDEF(&PORTF, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 94 */
    PINDEF(&PORTF, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 95 */
    PINDEF(&PORTF, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 96 */
    PINDEF(&PORTF, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 97 */
    PINDEF(&PORTF, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRF),
    /* 98 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 99 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 100 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),

#elif defined ATMEGA1284
    /* 0 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 1 */
    PINDEF(&PORTB, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 2 */
    PINDEF(&PORTB, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 3 */
    PINDEF(&PORTB, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 4, Slave Select LCD */
    PINDEF(&PORTB, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 5, Slave Select SD */
    PINDEF(&PORTB, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 6, MOSI */
    PINDEF(&PORTB, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 7, MISO  */
    PINDEF(&PORTB, 6, ATMEL_INPUT, ATMEL_PULL_ENA, &DDRB),
    /* 8, SCK */
    PINDEF(&PORTB, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRB),
    /* 9, ~Reset  */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 10, VCC */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 11, GND */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 12, XTAL1 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 13, XTAL2 */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 14, RX0 */
    PINDEF(&PORTD, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 15, TX0 */
    PINDEF(&PORTD, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 16, RX1 */
    PINDEF(&PORTD, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 17, TX1 */
    PINDEF(&PORTD, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 18 */
    PINDEF(&PORTD, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 19 */
    PINDEF(&PORTD, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 20 */
    PINDEF(&PORTD, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 21 */
    PINDEF(&PORTD, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRD),
    /* 22, SCL */
    PINDEF(&PORTC, 0, ATMEL_INPUT, ATMEL_PULL_DIS, &DDRC),
    /* 23, SDA */
    PINDEF(&PORTC, 1, ATMEL_INPUT, ATMEL_PULL_DIS, &DDRC),
    /* 24 */
    PINDEF(&PORTC, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 25 */
    PINDEF(&PORTC, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 26 */
    PINDEF(&PORTC, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 27 */
    PINDEF(&PORTC, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 28 */
    PINDEF(&PORTC, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 29 */
    PINDEF(&PORTC, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRC),
    /* 30, AVCC */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 31, GND */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 32, AREF */
    PINDEF(NULL, 0, ATMEL_DIR_INVAL, ATMEL_PULL_INVAL, NULL),
    /* 33 */
    PINDEF(&PORTA, 7, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 34 */
    PINDEF(&PORTA, 6, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 35 */
    PINDEF(&PORTA, 5, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 36 */
    PINDEF(&PORTA, 4, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 37 */
    PINDEF(&PORTA, 3, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 38 */
    PINDEF(&PORTA, 2, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 39 */
    PINDEF(&PORTA, 1, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),
    /* 40 */
    PINDEF(&PORTA, 0, ATMEL_OUTPUT, ATMEL_PULL_DIS, &DDRA),

#endif
};

/* export */
void initialize_pins() {
  for (uint8_t i = 0; i < ARRAY_SIZE(atmega328_pins); i++) {
    if (atmega328_pins[i].port_reg != NULL) {
      if (atmega328_pins[i].pin_dir == ATMEL_OUTPUT) {
        *(atmega328_pins[i].dir_reg) |= (1 << atmega328_pins[i].port_num);
      } else if (atmega328_pins[i].pin_dir == ATMEL_INPUT) {
        *(atmega328_pins[i].dir_reg) &= ~(1 << atmega328_pins[i].port_num);
        /* set internal pull-ups if input pin */
        if (atmega328_pins[i].pull_up == ATMEL_PULL_ENA) {
          *(atmega328_pins[i].port_reg) |= (1 << atmega328_pins[i].port_num);
        }
      }
    }
  }
}

// Need to move over to the next struct def

// /* export */
// int8_t pin_get(uint8_t pin) {
//   /*
//    *  return 1 if pin 1, 0 if 0, and -1 if error
//    *	########### Not tested yet ############
//    */
//   if (pin < ARRAY_SIZE(atmega328_pins) && pin > 0)
//     if (atmega328_pins[pin].port_reg != ATMEL_PORT_INVAL)
//       if (atmega328_pins[pin].pin_dir == ATMEL_INPUT)
//         switch (atmega328_pins[pin].port_reg) {
//         case ATMEL_PORT_B:
//           return ((PINB & ~(1 << atmega328_pins[pin].port_num)) > 0);
//         case ATMEL_PORT_C:
//           return ((PINC & ~(1 << atmega328_pins[pin].port_num)) > 0);
//         case ATMEL_PORT_D:
//           return ((PIND & ~(1 << atmega328_pins[pin].port_num)) > 0);
//         default:
//           return -1;
//         }

//   return -1;
// }

/* export */
int8_t pin_high(uint8_t pin) {
  /*
   *	need to search through the pin database to map ports to physical pins
   *	we can either iterate through the db and look for the pin_num,
   *	which would cost us CPU cycles, or we could map the pin number to the
   *	index of the db array. 0 is always invalid, no 0 pin
   *	array size is thus the maximum pin number, plus one for the 0-index.
   * 	therefore a pin request of ARRAY_SIZE is invalid
   */
  if (pin < ARRAY_SIZE(atmega328_pins) && pin > 0) {
    if (atmega328_pins[pin].port_reg != NULL) {
      if (atmega328_pins[pin].pin_dir == ATMEL_OUTPUT) {
        *(atmega328_pins[pin].port_reg) |= (1 << atmega328_pins[pin].port_num);
        return 0;
      } else
        UART_DBG("pin_high: not output\r\n");
    } else
      UART_DBG("pin_high: NULL reg\r\n");
  } else
    UART_DBG("pin_high: pin out of range\r\n");

  return -1;
}

/* export */
int8_t pin_low(uint8_t pin) {
  if (pin < ARRAY_SIZE(atmega328_pins) && pin > 0) {
    if (atmega328_pins[pin].port_reg != NULL) {
      if (atmega328_pins[pin].pin_dir == ATMEL_OUTPUT) {
        *(atmega328_pins[pin].port_reg) &= ~(1 << atmega328_pins[pin].port_num);
        return 0;
      } else
        UART_DBG("pin_low: not output\r\n");
    } else
      UART_DBG("pin_low: NULL reg\r\n");
  } else
    UART_DBG("pin_low: pin out of range\r\n");

  return -1;
}

/* export */
void pulse_pin(uint8_t pin) {
  /*
   *	assert the pin, wait, deassert
   *	delay time must be known at compile time
   */
  pin_high(pin);
  DELAY_MS(PULSE_WIDTH_MS);
  pin_low(pin);
}

/* export */
void cycle_pins() {
  for (uint8_t i = 0; i < ARRAY_SIZE(atmega328_pins); i++)
    if (atmega328_pins[i].port_reg != NULL)
      pulse_pin(i);
}
