#include <avr/io.h>
#include <util/delay.h>
#define BAUD_PRESCALE 47

void serial_init() {
	UCSR0B |= (1<<RXEN0)  | (1<<TXEN0);
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	UBRR0 = BAUD_PRESCALE;
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

void adc_init()
{
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);    
}

uint16_t adc_read(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

int main(void){
	uint16_t adc_val;
	serial_init();
	adc_init();
	DDRC &= ~(1 << PC5);
	PORTC &= ~(1 << PC5);
	//PORTC |= 1 << PC5;
	serial_write_string("fuck\n");
	while (1) {
		adc_val = adc_read(5);
		serial_write_uint16(adc_val);
		serial_write_endline();
		_delay_ms(1000);
	}
	return 0;
}