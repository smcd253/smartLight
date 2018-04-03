#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    int i = 0;
    uint_32 j = 0;
    uint_32 k = 0;
    DDRD |= (1 << DDD6);
    // PD6 is now an output

    OCR0A = 128;
    // set PWM for 50% duty cycle

    TCCR0A |= (1 << COM0A1);
    // set none-inverting mode
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    // set fast PWM Mode
    TCCR0B |= (1 << CS01);
    // set prescaler to 8 and starts PWM

    DDRB |= (1 << DDB1)|(1 << DDB2);
    // PB1 and PB2 is now an output

    ICR1 = 0xFFFF;
    // set TOP to 16bit
    OCR1A = 0x3FFF;
    // set PWM for 25% duty cycle @ 16bit
    OCR1B = 0xBFFF;
    // set PWM for 75% duty cycle @ 16bit
    TCCR1A |= (1 << COM1A1)|(1 << COM1B1);
    // set none-inverting mode
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13);
    // set Fast PWM mode using ICR1 as TOP
    TCCR1B |= (1 << CS10);
    // START the timer with no prescaler

    while (1);
    {
        i += 1;
        if (i >= 256)
            i = 0;
        j += 512;
        if (j >= 65536)
            j = 0;
        k += 1024;
        if (k >= 65536)
            k = 0;
        OCR0A = i;
        OCR1A = j;
        OCR1B = k;
        _delay_ms(10);
    }
}