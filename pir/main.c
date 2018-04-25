#include <avr/io.h>
#include <util/delay.h>
#define BAUD_PRESCALE 47
#define bool int

void serial_init() {
	UCSR0B |= (1<<RXEN0)  | (1<<TXEN0);
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	UBRR0 = BAUD_PRESCALE;
	DDRD |= 1 << PD3;
	PORTD &= ~(1 << PD3);
}

char serial_read_char() {
	char c;
	while ((UCSR0A & (1 << RXC0)) == 0);
	c = UDR0;
	return c;
}

void serial_write_char(char c) {
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = c;
}

void serial_write_string(char* s) {
	for (; *s; s++)
		serial_write_char(*s);
}

void serial_write_endline() {
	serial_write_char('\n');
}

void serial_write_uint16(uint16_t n) {
	int i = 0;
	char s[20];
	if (n == 0) {
		serial_write_char('0');
	} else {
		for (; n > 0; n /= 10, i++)
			s[i] = '0' + n % 10;
		for (; i > 0; i--)
			serial_write_char(s[i - 1]);
	}
}

void pir_init() {
	DDRC &= ~(1 << PC3);
	PORTC &= ~(1 << PC3);  
}

bool pir_is_on() {
	return (PINC & (1 << PC3)) != 0;
}

int main(void){
	serial_init();
	pir_init();
	while (1) {
		if (pir_is_on()) {
			serial_write_string("on ");
		} else {
			serial_write_string("off ");
		}
		_delay_ms(1000);
	}
	return 0;
}