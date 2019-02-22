#include <avr/io.h>

void USARTInit(unsigned int ubrr_value) { // is UBRR>255 supported?
	//Set Baud rate
	UBRR0H = (unsigned char)(ubrr_value >> 8);  
	UBRR0L = (unsigned char)(ubrr_value & 255);
	// Frame Format: asynchronous, no parity, 1 stop bit, char size 8
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	//Enable The receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

char USARTReadChar() { // blocking
	while(!(UCSR0A & (1<<RXC0))) {}
	return UDR0;
}

void USARTWriteChar(char data) { // blocking
	while(!(UCSR0A & (1<<UDRE0))) {}
	UDR0=data;
}

void uwrite_hex(unsigned char n) {
	if(((n>>4) & 15) < 10)
		USARTWriteChar('0' + ((n>>4)&15));
	else
		USARTWriteChar('A' + ((n>>4)&15) - 10);
	n <<= 4;
	if(((n>>4) & 15) < 10)
		USARTWriteChar('0' + ((n>>4)&15));
	else
		USARTWriteChar('A' + ((n>>4)&15) - 10);
}

void uwrite_str(char *str) {
	char i;
	
	for(i=0; str[i]; i++)
		USARTWriteChar(str[i]);
}

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI (1<<PB3)
#define MISO (1<<PB4)
#define SCK (1<<PB5)

#define CS_DDR DDRB
#define CS (1<<PB2)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

void SPI_init() {
	CS_DDR |= CS; // SD card circuit select as output
	SPI_DDR |= MOSI + SCK; // MOSI and SCK as outputs
	SPI_PORT |= MISO; // pullup in MISO, might not be needed
	
	// Enable SPI, master, set clock rate fck/128
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
}

unsigned char SPI_write(unsigned char ch) {
	SPDR = ch;
	while(!(SPSR & (1<<SPIF))) {}	
	return SPDR;
}

#define R1_IDLE_STATE (1 << 0)
#define R1_ERASE_RESET (1 << 1)
#define R1_ILLEGAL_COMMAND (1 << 2)
#define R1_CRC_ERROR (1 << 3)
#define R1_ERASE_SEQ_ERROR (1 << 4)
#define R1_ADDRESS_ERR (1 << 5)
#define R1_PARAMETER_ERR (1 << 6)

void SD_command(unsigned char cmd, unsigned long arg, unsigned char crc, unsigned char read) {
	unsigned char i, buffer[8];
	
	uwrite_str("CMD ");
	uwrite_hex(cmd);
	
	CS_ENABLE();
	SPI_write(cmd);
	SPI_write(arg>>24);
	SPI_write(arg>>16);
	SPI_write(arg>>8);
	SPI_write(arg);
	SPI_write(crc);
		
	for(i=0; i<read; i++)
		buffer[i] = SPI_write(0xFF);
		
	CS_DISABLE();		
		
	for(i=0; i<read; i++) {
		USARTWriteChar(' ');
		uwrite_hex(buffer[i]);
	}
	
	uwrite_str("\r\n");

	if(buffer[1] & R1_IDLE_STATE)
		uwrite_str("R1: IDLE STATE\r\n");
	if(buffer[1] & R1_ERASE_RESET)
		uwrite_str("R1: ERASE RESET\r\n");
	if(buffer[1] & R1_ILLEGAL_COMMAND)
		uwrite_str("R1: ILLEGAL COMMAND\r\n");
	if(buffer[1] & R1_CRC_ERROR)
		uwrite_str("R1: CRC ERROR\r\n");
	if(buffer[1] & R1_ERASE_SEQ_ERROR)
		uwrite_str("R1: ERASE SEQUENCE ERROR\r\n");
	if(buffer[1] & R1_ADDRESS_ERR)
		uwrite_str("R1: ADDRESS ERROR\r\n");
	if(buffer[1] & R1_PARAMETER_ERR)
		uwrite_str("R1: PARAMETER ERROR\r\n");
}

int main() {
	char i;
	
	USARTInit(52); // 20 MHz / (16 * 19200 baud) - 1 = 64.104x
	SPI_init();

	// ]r:10
	CS_DISABLE();
	for(i=0; i<10; i++) // idle for 1 bytes / 80 clocks
		SPI_write(0xFF);
	
	while(1) {
		switch(USARTReadChar()) {
		case '1':
			SD_command(0x40, 0x00000000, 0x95, 8);
			CS_DISABLE();	
			CS_ENABLE();
			break;
		case '2':
			SD_command(0x41, 0x00000000, 0xF9, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '3':
			SD_command(0x50, 0x00000200, 0xFF, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '4':
			SD_command(0x48, 0x000001AA, 0x87, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '5':
			SD_command(0x69, 0x00000000, 0xFF, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '6':
			SD_command(0x29, 0X40000000, 0xFF, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '7':
			SD_command(0X3A, 0x00000000, 0xFF, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;

		}
	}	
	
	return 0;
}