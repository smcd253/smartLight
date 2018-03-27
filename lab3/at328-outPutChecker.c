/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // set all free pins to output
    // DDRB |= 0b10111111;
    // DDRC |= 0b00111111;
    int i = 0;      
    DDRD |= 0b11111111;

    while (1) {
        for (i = 0; i < 8; i++) {
            PORTD = 1 << i;
            _delay_us(500);           // wait 0.5ms
        }
        PORTD = 0;
    }

    return 0;   /* never reached */
}