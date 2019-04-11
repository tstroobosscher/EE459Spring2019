#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <lib/libiberty.>
int main() {
  char *s;

  vasprintf(&s, "Hello World");

  while(1) {
  }

  return 0;
}
