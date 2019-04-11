#include <avr/io.h>
#include <stdio.h>
 
#define FOSC 16000000

#define BUAD_UART_0 38400

#define MYUBRR(x) ((FOSC / 16 / x) - 1)

void uart_write_char(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = data;
}

void uart_write_str(char *buf) {
  while(*buf) {
    uart_write_char(*buf);
    buf++;
  }
}


int main() {
  
  UBRR0=MYUBRR(BUAD_UART_0);
 
  UCSR0B|=(1<<TXEN0)|(1<<RXEN0);
 
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);

  float test = 1.0/2;

  char buf[32];

  sprintf(buf, "%f", test);

  uart_write_str("Hello World!\r\n");
  uart_write_str("float: ");
  uart_write_str(buf);
  uart_write_str("\r\n");

  while(1) {
  }

  return 0;
}
