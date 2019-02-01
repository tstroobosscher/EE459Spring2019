#ifndef PINS_H
#define PINS_H

/* milliseconds */
#define PULSE_WIDTH_MS 100

const enum {
	ATMEL_PORT_B,
	ATMEL_PORT_C,
	ATMEL_PORT_D,
	ATMEL_PORT_INVAL,
} port_name;

const enum {
	ATMEL_INPUT,
	ATMEL_OUTPUT,
	ATMEL_DIR_INVAL,
} data_dir;

const struct atmel_328_pin {
	int pin_num;
	int port_reg;
	int port_num;
	int pin_dir;
};

void initialize_pins();
int assert_pin(int pin);
int deassert_pin(int pin);
void pulse_pin(int pin);

#endif