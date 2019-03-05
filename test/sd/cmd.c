#include <avr/io.h>

#define ATMEGA2560

#define FOSC 16000000
/*
 *	UART Baud rate
 */
#define BAUD 9600

/*
 *	Value for UBRR0 register
 */
#define MYUBRR ((FOSC / 16 / BAUD) - 1)

char USARTReadChar() {
  while (!(UCSR0A & (1 << RXC0))) {
  }
  return UDR0;
}

void USARTWriteChar(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  }
  UDR0 = data;
}

void uwrite_str(char *buf) {
  while (*buf) {
    USARTWriteChar(*buf);
    buf++;
  }
}

void uart_write_strn(uint8_t *buf, uint8_t n) {
  for (char i = 0; i < n; i++) {
    USARTWriteChar(*buf);
    buf++;
  }
}

void uwrite_hex(uint8_t n) {
  if (((n >> 4) & 15) < 10)
    USARTWriteChar('0' + ((n >> 4) & 15));
  else
    USARTWriteChar('A' + ((n >> 4) & 15) - 10);
  n <<= 4;
  if (((n >> 4) & 15) < 10)
    USARTWriteChar('0' + ((n >> 4) & 15));
  else
    USARTWriteChar('A' + ((n >> 4) & 15) - 10);
}

void uart_write_32(uint32_t val) {
  uwrite_hex(val >> 24);
  uwrite_hex(val >> 16);
  uwrite_hex(val >> 8);
  uwrite_hex(val);
}

void USARTInit(unsigned long ubrr_value) {
  /*
   * Set Baud rate
   */
  UBRR0H = (unsigned char)(ubrr_value >> 8);
  UBRR0L = (unsigned char)(ubrr_value & 255);

  /*
   *	Frame Format: asynchronous, no parity, 1 stop bit, char size 8
   */
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  /*
   *	Enable The receiver and transmitter
   */
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  /*
   *	flush output buffer, kind of a hack, should really be checking
   *	buffer status
   */
  uwrite_str("\n\r\n\r");
}

#if defined ATMEGA328

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI (1<<PB3)
#define MISO (1<<PB4)
#define SCK (1<<PB5)

#define CS_DDR DDRB
#define CS (1<<PB2)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

#elif defined ATMEGA2560

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI (1<<PB2)
#define MISO (1<<PB3)
#define SCK (1<<PB1)

#define CS_DDR DDRB
#define CS (1<<PB0)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

#endif

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

	// uwrite_str("sd: cmd sent: ");
 //    dump_byte(cmd);
 //    dump_byte(arg >> 24);
 //    dump_byte(arg >> 16);
 //    dump_byte(arg >> 8);
 //    dump_byte(arg);
 //    dump_byte(crc);
 //    uwrite_str("\r\n");
		
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
	
	USARTInit(MYUBRR); // 20 MHz / (16 * 19200 baud) - 1 = 64.104x
	SPI_init();

	uwrite_str("test\r\n");

	// ]r:10
	CS_DISABLE();
	for(i=0; i<10; i++) // idle for 1 bytes / 80 clocks
		SPI_write(0xFF);
	
	while(1) {
		switch(USARTReadChar()) {
		case '1':
			/* CMD0 */
			SD_command(0x40 | 0, 0x00000000, 0x95, 8);
			CS_DISABLE();	
			CS_ENABLE();
			break;
		case '2':
			/* CMD1 */
			SD_command(0x40 | 1, 0x00000000, 0xF9, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '3':
			/* CMD16 */
			SD_command(0x40 | 16, 512, 0xFF, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '4':
			/* CMD8 */
			SD_command(0x40 | 8, 0x000001AA, 0x87, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '5':
			/* CMD58 */
			SD_command(0x40 | 58, 0x00000000, 0xFD, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '6':
			/* ACMD41 */
			SD_command(0x40 | 41, 0X40000000, 0xFF, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;
		case '7':
			/* CMD55 */
			SD_command(0x40 | 55, 0x00000000, 0xFF, 8);
			CS_DISABLE();
			CS_ENABLE();
			break;

		case '8':
			/* CMD17 */
			// SD_command(0x40 | 17, 0x00000000, 0xFF, 8);
			// CS_DISABLE();
			// CS_ENABLE();
			SPI_write(0x40 | 17);
			SPI_write(0x00);
			SPI_write(0x00);
			SPI_write(0x7E);
			SPI_write(0x78);
			SPI_write(0xFF);
			for(uint8_t i = 0; i < 10 && (SPI_write(0xFF) == 0xFF); i++) {}
			CS_DISABLE();
			CS_ENABLE();
			uint8_t buf;

			do {
				buf = SPI_write(0xFF);
				USARTWriteChar(' ');
				uwrite_hex(buf);
			} while(buf == 0xFF);

			for(uint16_t i = 0; i < 512; i++)
				USARTWriteChar(SPI_write(0xFF));
			
			break;
		}
	}	
	
	return 0;
}