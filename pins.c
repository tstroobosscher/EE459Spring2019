/*
 *	USC EE459 Spring 2019 Team 17 - Pins
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stddef.h>
#include <util/delay.h>

#include "debug.h"
#include "pins.h"
#include "utils.h"

/* set pin behavior here (OUTPUT/INPUT), ENA/DIS internal pull ups */
static const struct atmel_328_pin {
  volatile uint8_t *port_reg;
  uint8_t port_num;
  uint8_t pin_dir;
  uint8_t pull_up;
  volatile uint8_t *dir_reg;
} __attribute((packed)) atmega328_pins[] = {

#if defined ATMEGA2560 && defined ATMEGA328
#error Cannot compile for multiple chips
#endif

#ifdef ATMEGA328
    /* 0, no 0 pin, just used to align the array index with the pin number */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 1  */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 2  */
    {
        &PORTD,
        0,
        ATMEL_INPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 3  */
    {
        &PORTD,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 4  */
    {
        &PORTD,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 5  */
    {
        &PORTD,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 6  */
    {
        &PORTD,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 7  */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 8  */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 9  */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 10 */
    {
        &PORTB,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 11 */
    {
        &PORTD,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 12 */
    {
        &PORTD,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 13 */
    {
        &PORTD,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 14 */
    {
        &PORTB,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 15 */
    {
        &PORTB,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 16 */
    {
        &PORTB,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 17 */
    {
        &PORTB,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 18, MISO */
    {
        &PORTB,
        4,
        ATMEL_INPUT,
        ATMEL_PULL_ENA,
        &DDRB,
    },
    /* 19 */
    {
        &PORTB,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 20 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 21 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 22 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 23 */
    {
        &PORTC,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 24 */
    {
        &PORTC,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 25 */
    {
        &PORTC,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 26 */
    {
        &PORTC,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 27 */
    {
        &PORTC,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 28 */
    {
        &PORTC,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },

#elif defined ATMEGA2560
    /* 0 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 1 */
    {
        &PORTG,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRG,
    },
    /* 2, RXD0 */
    {
        &PORTE,
        0,
        ATMEL_INPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 3, TXD0 */
    {
        &PORTE,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 4  */
    {
        &PORTE,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 5  */
    {
        &PORTE,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 6  */
    {
        &PORTE,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 7  */
    {
        &PORTE,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 8  */
    {
        &PORTE,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 9  */
    {
        &PORTE,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRE,
    },
    /* 10 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 11 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 12 */
    {
        &PORTH,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 13 */
    {
        &PORTH,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 14 */
    {
        &PORTH,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 15 */
    {
        &PORTH,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 16 */
    {
        &PORTH,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 17 */
    {
        &PORTH,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 18 */
    {
        &PORTH,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 19 */
    {
        &PORTB,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 20 */
    {
        &PORTB,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 21 */
    {
        &PORTB,
        2,
        ATMEL_INPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 22, MISO */
    {
        &PORTB,
        3,
        ATMEL_INPUT,
        ATMEL_PULL_ENA,
        &DDRB,
    },
    /* 23 */
    {
        &PORTB,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 24 */
    {
        &PORTB,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 25 */
    {
        &PORTB,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 26, Arduino Mega D13 - LED */
    {
        &PORTB,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRB,
    },
    /* 27 */
    {
        &PORTH,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRH,
    },
    /* 28 */
    {
        &PORTG,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRG,
    },
    /* 29 */
    {
        &PORTG,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRG,
    },
    /* 30 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 31 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 32 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 33 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 34 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 35 */
    {
        &PORTL,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRG,
    },
    /* 36 */
    {
        &PORTL,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRL,
    },
    /* 37 */
    {
        &PORTL,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRL,
    },
    /* 38 */
    {
        &PORTL,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRL,
    },
    /* 39 */
    {
        &PORTL,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRL,
    },
    /* 40 */
    {
        &PORTL,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRL,
    },
    /* 41 */
    {
        &PORTL,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRL,
    },
    /* 42 */
    {
        &PORTL,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRL,
    },
    /* 43 */
    {
        &PORTD,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 44 */
    {
        &PORTD,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 45 */
    {
        &PORTD,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 46 */
    {
        &PORTD,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 47 */
    {
        &PORTD,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 48 */
    {
        &PORTD,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 49 */
    {
        &PORTD,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 50 */
    {
        &PORTD,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRD,
    },
    /* 51 */
    {
        &PORTG,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRG,
    },
    /* 52 */
    {
        &PORTG,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRG,
    },
    /* 53 */
    {
        &PORTC,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 54 */
    {
        &PORTC,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 55 */
    {
        &PORTC,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 56 */
    {
        &PORTC,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 57 */
    {
        &PORTC,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 58 */
    {
        &PORTC,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 59 */
    {
        &PORTC,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 60 */
    {
        &PORTC,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRC,
    },
    /* 61 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 62 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 63 */
    {
        &PORTJ,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 64 */
    {
        &PORTJ,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 65 */
    {
        &PORTJ,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 66 */
    {
        &PORTJ,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 67 */
    {
        &PORTJ,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 68 */
    {
        &PORTJ,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 69 */
    {
        &PORTJ,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 70 */
    {
        &PORTG,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRG,
    },
    /* 71 */
    {
        &PORTA,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 72 */
    {
        &PORTA,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 73 */
    {
        &PORTA,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 74 */
    {
        &PORTA,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 75 */
    {
        &PORTA,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 76 */
    {
        &PORTA,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 77 */
    {
        &PORTA,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 78 */
    {
        &PORTA,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRA,
    },
    /* 79 */
    {
        &PORTJ,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRJ,
    },
    /* 80 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 81 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 82 */
    {
        &PORTK,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 83 */
    {
        &PORTK,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 84 */
    {
        &PORTK,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 85 */
    {
        &PORTK,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 86 */
    {
        &PORTK,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 87 */
    {
        &PORTK,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 88 */
    {
        &PORTK,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 89 */
    {
        &PORTK,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRK,
    },
    /* 90 */
    {
        &PORTF,
        7,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 91 */
    {
        &PORTF,
        6,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 92 */
    {
        &PORTF,
        5,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 93 */
    {
        &PORTF,
        4,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 94 */
    {
        &PORTF,
        3,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 95 */
    {
        &PORTF,
        2,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 96 */
    {
        &PORTF,
        1,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 97 */
    {
        &PORTF,
        0,
        ATMEL_OUTPUT,
        ATMEL_PULL_DIS,
        &DDRF,
    },
    /* 98 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 99 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },
    /* 100 */
    {
        NULL,
        0,
        ATMEL_DIR_INVAL,
        ATMEL_PULL_INVAL,
        NULL,
    },

#endif
};

/* export */
void initialize_pins() {
  for (uint8_t i = 0; i < ARRAY_SIZE(atmega328_pins); i++) {
    if (atmega328_pins[i].port_reg != NULL) {
      if (atmega328_pins[i].pin_dir == ATMEL_OUTPUT) {
        UART_DBG("pin_init: OUTPUT\r\n");
        *(atmega328_pins[i].dir_reg) |= (1 << atmega328_pins[i].port_num);
      } else if (atmega328_pins[i].pin_dir == ATMEL_INPUT) {
        UART_DBG("pin_init: INPUT\r\n");
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
      } else UART_DBG("pin_high: not output\r\n");
    } else UART_DBG("pin_high: NULL reg\r\n");
  } else UART_DBG("pin_high: pin out of range\r\n");

  return -1;
}

/* export */
int8_t pin_low(uint8_t pin) {
  if (pin < ARRAY_SIZE(atmega328_pins) && pin > 0) {
    if (atmega328_pins[pin].port_reg != NULL) {
      if (atmega328_pins[pin].pin_dir == ATMEL_OUTPUT) {
        *(atmega328_pins[pin].port_reg) &= ~(1 << atmega328_pins[pin].port_num);
        return 0;
      } else UART_DBG("pin_low: not output\r\n");
    } else UART_DBG("pin_low: NULL reg\r\n");
  } else UART_DBG("pin_low: pin out of range\r\n");

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
