#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
 
#define FOSC 16000000

#define BUAD_UART_0 9600

#define MYUBRR(x) ((FOSC / 16 / x) - 1)

const char message[] PROGMEM = "Hello World!";

void uart_write_char(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = data;
}

void uwrite_str(char *buf) {
  while (*buf) {
   uart_write_char(*buf);
    buf++;
  }
}

int main() {
  DDRB |= (1 << 7);
  
  UBRR0=MYUBRR(BUAD_UART_0);
 
  UCSR0B|=(1<<TXEN0)|(1<<RXEN0);
 
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);


  while(1) {
    uwrite_str((PGM_P)pgm_read_word(message));
    PORTB |= (1 << 7);
    _delay_ms(1000);
    PORTB &= ~(1 << 7);
    _delay_ms(1000);
  }

  return 0;
}
