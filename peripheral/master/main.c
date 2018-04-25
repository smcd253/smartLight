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
    DDRD |= 1 << PD3;
}

void serial_write_char_to_peripheral(char c) {
    PORTD |= 1 << PD3;
    while ((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = c;
}

void serial_write_char_to_monitor(char c) {
    PORTD &= ~(1 << PD3);
    while ((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = c;
}

int main (void) {
    int i;
    serial_init();
	uint8_t pwmr = 255;
	uint8_t pwmg = 255;
	uint8_t pwmb = 255;
	for (;;) {
        //serial_write_char_to_monitor('a');
    	_delay_ms(2000);
		// White
        serial_write_char_to_peripheral((pwmr & ~(3)) + 0);
        serial_write_char_to_peripheral((pwmg & ~(3)) + 1);
        serial_write_char_to_peripheral((pwmb & ~(3)) + 2);

        _delay_ms(2000);

        for (i = 0; i < 128; i++) {
        	pwmb--;
        	pwmg--;
        	OCR0B = pwmg;
        	OCR0A = pwmb;
        	_delay_ms(10);
        }

        _delay_ms(2000);

        for (i = 0; i < 127; i++) {
        	pwmb--;
            serial_write_char_to_peripheral((pwmb & ~(3)) + 2);
        	_delay_ms(10);
        }

        _delay_ms(2000);

        for (i = 0; i < 127; i++) {
        	pwmg++;
        	pwmb++;
            serial_write_char_to_peripheral((pwmg & ~(3)) + 1);
            serial_write_char_to_peripheral((pwmb & ~(3)) + 2);
        	_delay_ms(10);
        }

        for (i = 0; i < 128; i++) {
        	pwmb++;
            serial_write_char_to_peripheral((pwmb & ~(3)) + 2);
        	_delay_ms(10);
        }

        _delay_ms(2000);
	}
}