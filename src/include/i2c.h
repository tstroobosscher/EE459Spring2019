#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void initialize_i2c();
uint8_t i2c_io(uint8_t, uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, uint16_t);

#endif
