#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define _GNU_SOURCE

#include <stdio.h>

int main() {
  char *s;

  asprintf(&s, "Hello World");

  while(1) {
  }

  return 0;
}
