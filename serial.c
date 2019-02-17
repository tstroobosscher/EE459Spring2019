#define FOSC 16000000           // Clock frequency??
#define BAUD 9600               // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0 register

/*
serial_init - Initialize the USART port
*/

// prototypes:

void serial_init(unsigned short); // maybe I should move this to a header file
void serial_out(char ch);
char serial_in()

int main(void) {

	serial_init(MYUBRR);

}

void serial_init(unsigned short ubrr) {
	UBRR0 = ubrr ; // Set baud rate
	UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
	UCSR0B |= (1 << RXEN0 ); // Turn on receiver
	UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity ,
	// one stop bit , 8 data bits
}


// serial_out - Output a byte to the USART0 port


void serial_out(char ch)
{
while (( UCSR0A & (1 < < UDRE0 )) == 0);
UDR0 = ch ;
} 


// serial_in - Read a byte from the USART0 and return it


char serial_in()
{
while ( !( UCSR0A & (1 << RXC0 )) );
return UDR0 ;
}

