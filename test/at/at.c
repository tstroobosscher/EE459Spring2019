#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
 
#define FOSC 16000000

#define BUAD_UART_0 38400
#define BUAD_UART_1 38400

#define MYUBRR(x) ((FOSC / 16 / x) - 1)

void uart_write_char_0(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = data;
}

char uart_read_char_0() {
  while (!(UCSR0A & (1 << RXC0))) {
  }
  return UDR0;
}

char uart_data_available_0() {
  return (UCSR0A & (1 << RXC0));
}

void uart_write_char_1(char data) {
  while (!(UCSR1A & (1 << UDRE1))) {
  }
  UDR1 = data;
}

char uart_read_char_1() {
  while (!(UCSR1A & (1 << RXC1))) {
  }
  return UDR1;
}

char uart_data_available_1() {
  return (UCSR1A & (1 << RXC1));
}

int main() {   
  
  UBRR0=MYUBRR(BUAD_UART_0);
  UBRR1=MYUBRR(BUAD_UART_1);
 
  UCSR0B|=(1<<TXEN0)|(1<<RXEN0);
 
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);

  UCSR1B|=(1<<TXEN1)|(1<<RXEN1);
 
  UCSR1C|=(1<<UCSZ11)|(1<<UCSZ10);

  while(1) {
    if(uart_data_available_0()) {
      char ch = uart_read_char_0();
      uart_write_char_1(ch);
    }

    if(uart_data_available_1()) {
      char ch = uart_read_char_1();
      uart_write_char_0(ch);
    }
  }

  return 0;
}

