/*
 *	USC EE459 Spring 2019 Team 17 - 2560 Test
 */



#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE 300
#define BAUD_PRESCALLER (((7372810 / (BAUDRATE * 16UL))) - 1)

#define FOSC 7372810

#define MYUBRR(x) ((FOSC / 16 / x) - 1)

//Declaration of our functions
void USART_init(void);
unsigned char USART_receive(void);
void USART_send( unsigned char data);

int main(void){
USART_init();        //Call the USART initialization code

DDRB |= (1 << 0);

while(1){        //Infinite loop
  USART_send('f');
  PORTB |= (1 << 0);
  _delay_ms(1000);        //Delay for 5 seconds so it will re-send the string every 5 seconds
  PORTB &= ~(1 << 0);
  _delay_ms(1000);
 }

return 0;
}

void USART_init(void){

  /* 23 */
  UBRR0 = MYUBRR(19200);
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  UCSR0C = (3<<UCSZ00);
}

unsigned char USART_receive(void){

 while(!(UCSR0A & (1<<RXC0)));
 return UDR0;

}

void USART_send( unsigned char data){

 while(!(UCSR0A & (1<<UDRE0)));
 UDR0 = data;

}
