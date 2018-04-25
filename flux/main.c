/**
 * A PWM example for the ATmega328P using the 8-Bit Fast PWM mode.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

int main (void) {
	int i;
	DDRB |= _BV(PB1); // 15
    DDRD |= _BV(PD5) | _BV(PD6); // 11 12
	TCCR1A |= _BV(COM1A1) | _BV(WGM10);
    TCCR1B |= _BV(CS10) | _BV(WGM12);
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);
    TCCR0B |= _BV(CS00);
	uint8_t pwmr = 255;
	uint8_t pwmg = 255;
	uint8_t pwmb = 255;
	for (;;) {
		OCR1A = 0;
    	OCR0B = 0;
    	OCR0A = 0;
    	_delay_ms(2000);
		// White
		OCR1A = pwmr;
        OCR0B = pwmg;
        OCR0A = pwmb;

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
        	OCR0A = pwmb;
        	_delay_ms(10);
        }

        _delay_ms(2000);

        for (i = 0; i < 127; i++) {
        	pwmg++;
        	pwmb++;
        	OCR0B = pwmg;
        	OCR0A = pwmb;
        	_delay_ms(10);
        }

        for (i = 0; i < 128; i++) {
        	pwmb++;
        	OCR0A = pwmb;
        	_delay_ms(10);
        }

        _delay_ms(2000);
	}
}