/*
 *	USC EE459 Spring 2019 Team 17 - Utils header
 */

#ifndef UTILS_H
#define UTILS_H


/* 7.3728 Mhz Crystal */
#define FOSC 7372800

#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))

/* avr requires compile-time knowledge of delay times */
#define DELAY_MS(x) (_delay_ms(x))

#endif
