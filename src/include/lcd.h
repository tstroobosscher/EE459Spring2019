/*
 *	USC EE459 Spring 2019 Team 17 - LCD Header
 */

#ifndef LCD_H
#define LCD_H

#include <stdint.h>

void i2c_init();
uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
               uint8_t *, uint16_t, uint8_t *, uint16_t);

#endif
