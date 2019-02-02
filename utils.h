/*
 *	EE459 Team 17 - Utils header
 */

#ifndef UTILS_H
#define UTILS_H

#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))

/* avr requires compile-time knowledge of delay times */
#define DELAY_MS(x) (_delay_ms(x))

#endif
