/*
 *	USC EE459 Spring 2019 Team 17 - 2560 Test
 */

#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((16000000 / (BAUDRATE * 16UL))) - 1)

//Declaration of our functions
void USART_init(void);
unsigned char USART_receive(void);
void USART_send( unsigned char data);

int main(void){
USART_init();        //Call the USART initialization code

while(1){        //Infinite loop
 USART_send('A');
 _delay_ms(1000);        //Delay for 5 seconds so it will re-send the string every 5 seconds
 }

return 0;
}

void USART_init(void){

 UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
 UBRR0L = (uint8_t)(BAUD_PRESCALLER);
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
