/**
 * A PWM example for the ATmega328P using the 8-Bit Fast PWM mode.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#include "blue_light_curve.h"

void init_pwm(){
	/**
	 * We will be using OCR1A as our PWM output which is the
	 * same pin as PB1.
	 */
	DDRB |= _BV(PB1); // 15
    DDRD |= _BV(PD5) | _BV(PD6); // 11 12
    

	/**
	 * There are quite a number of PWM modes available but for the
	 * sake of simplicity we'll just use the 8-bit Fast PWM mode.
	 * This is done by setting the WGM10 and WGM12 bits.  We 
	 * Setting COM1A1 tells the microcontroller to set the 
	 * output of the OCR1A pin low when the timer's counter reaches
	 * a compare value (which will be explained below).  CS10 being
	 * set simply turns the timer on without a prescaler (so at full
	 * speed).  The timer is used to determine when the PWM pin should be
	 * on and when it should be off.
	 */
	TCCR1A |= _BV(COM1A1) | _BV(WGM10);
	TCCR1B |= _BV(CS10) | _BV(WGM12);
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);
	TCCR0B |= _BV(CS00);
}

void blue_light_curve(){
    int i;

    // pwmx = 255 --> WHITE
    pwmr = 255;
    pwmg = 255;
    pwmb = 255;
    
    OCR1A = pwmr;
    OCR0B = pwmg;
    OCR0A = pwmb;

    _delay_ms(2000);
    // dim blue
    for (i = 0; i < 128; i++) {
        pwmb--;
        OCR0A = pwmb;
        _delay_ms(10);
    }

    _delay_ms(2000);
    // dim green and blue
    for (i = 0; i < 127; i++) {
        pwmb--;
        pwmg--;
        OCR0B = pwmg;
        OCR0A = pwmb;
        _delay_ms(10);
    }

    _delay_ms(2000);
    // dim green
    for (i = 0; i < 128; i++) {
        pwmg--;
        OCR0B = pwmg;
        _delay_ms(10);
    }

    _delay_ms(2000);
    // brighten green
    for (i = 0; i < 128; i++) {
        pwmg++;
        OCR0B = pwmg;
        _delay_ms(10);
    }

    _delay_ms(2000);
    // brighten green and blue
    for (i = 0; i < 127; i++) {
        pwmg++;
        pwmb++;
        OCR0B = pwmg;
        OCR0A = pwmb;
        _delay_ms(10);
    }

    // brighten blue
    for (i = 0; i < 128; i++) {
        pwmb++;
        OCR0A = pwmb;
        _delay_ms(10);
    }
}