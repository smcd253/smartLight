#include <avr/io.h>
#include <util/delay.h>
 
#define USART_BAUDRATE 9600
//#define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)
#define BAUD_PRESCALE 47
 
int main(void){
	char received_byte;
  	int i;
	UCSR0B |= (1<<RXEN0)  | (1<<TXEN0);
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	UBRR0 = BAUD_PRESCALE;
	//UBRR0H  = (BAUD_PRESCALE >> 8);
	//UBRR0L  = BAUD_PRESCALE;

	DDRC |= 1;

	for (i = 0; i < 1; i++) {
    	_delay_ms(1000);
    	PORTC &= 0b11111110;
    	_delay_ms(1000);
    	PORTC |= 0b00000001;
	}

	/*while (1) {
		for (received_byte = 'a'; received_byte <= 'z'; received_byte++) {
			while( ( UCSR0A & ( 1 << UDRE0 ) ) == 0 ){}
 
			// write the byte to the serial port
			UDR0 = received_byte;
    		_delay_ms(1);
		}

    	_delay_ms(10);
		for (received_byte = 'A'; received_byte <= 'Z'; received_byte++) {
			while( ( UCSR0A & ( 1 << UDRE0 ) ) == 0 ){}
 
			// write the byte to the serial port
			UDR0 = received_byte;
    		_delay_ms(1);
		}

    	_delay_ms(10);
	}*/

	while (1) {

		// wait until a byte is ready to read
		while( ( UCSR0A & ( 1 << RXC0 ) ) == 0 ){}

		// grab the byte from the serial port
		received_byte = UDR0;

    	PORTC &= 0b11111110;
		// wait until the port is ready to be written to
		while( ( UCSR0A & ( 1 << UDRE0 ) ) == 0 ){}
 
		// write the byte to the serial port
		UDR0 = received_byte;

    	// wait until a byte is ready to read
		while( ( UCSR0A & ( 1 << RXC0 ) ) == 0 ){}

		// grab the byte from the serial port
		received_byte = UDR0;

    	PORTC |= 0b00000001;
		// wait until the port is ready to be written to
		while( ( UCSR0A & ( 1 << UDRE0 ) ) == 0 ){}
 
		// write the byte to the serial port
		UDR0 = received_byte;
	}
	return 0;   /* never reached */
}