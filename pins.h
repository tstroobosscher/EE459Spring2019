#ifndef PINS_H
#define PINS_H

/* milliseconds */
#define PULSE_WIDTH_MS 100

void initialize_pins();
int assert_pin(int pin);
int deassert_pin(int pin);
void pulse_pin(int pin);
void cycle_pins();

#endif