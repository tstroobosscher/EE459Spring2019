#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
 
#define FOSC 16000000

#define BUAD_UART_0 9600

#define MYUBRR(x) ((FOSC / 16 / x) - 1)

volatile int index = 0;

void uart_write_char(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = data;
}

int main() {
  DDRB |= (1 << 7);

  sei();           
  
  UBRR0=MYUBRR(BUAD_UART_0);
 
  UCSR0B|=(1<<RXCIE0)|(1<<TXEN0)|(1<<RXEN0);
 
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);

  while(1) {
    PORTB |= (1 << 7);
    _delay_ms(1000);
    PORTB &= ~(1 << 7);
    _delay_ms(1000);
    uart_write_char(index + 48);
  }

  return 0;
}

ISR(USART0_RX_vect) {
  index++;
  char x = UDR0;
}

