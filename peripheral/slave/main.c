/**
 * A PWM example for the ATmega328P using the 8-Bit Fast PWM mode.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
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

int main (void) {
    serial_init();
	uint8_t data, color, brightness;
	DDRB |= _BV(PB1); // 15
    DDRD |= _BV(PD5) | _BV(PD6); // 11 12
	TCCR1A |= _BV(COM1A1) | _BV(WGM10);
    TCCR1B |= _BV(CS10) | _BV(WGM12);
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);
    TCCR0B |= _BV(CS00);
	for (;;) {
        data = serial_read_char();
        color = data & 3;
        if (color == 0) {
            OCR1A = data;
        } else if (color == 1) {
            OCR0B = data;
        } else if (color == 2) {
            OCR0A = data;
        }
	}
}