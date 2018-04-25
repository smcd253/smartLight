#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "rgb_sensor.h"
#include "blue_light_curve.h"

int a = 0;
char redSize;
uint8_t redSize_int;

// ISR for RGB sensor values
ISR(TIMER1_OVF_vect){
    uint8_t data;
    data = read8(DEVICE_ID);
    //serial_write_string(" DECIVE_ID:");
    //serial_write_uint16(data);
    // serial_write('R');
    // serial_write_uint16(readRed());
    // serial_write('G');
    // serial_write_uint16(readGreen());
    // serial_write('B');
    // serial_write_uint16(readBlue());

    // trying to find size of array and print to screen...
    redBuf[a] = 1;
    a += 1;
    redSize = sizeof(redBuf)/sizeof(redBuf[0]);
    redSize_int = (int)redSize;
    serial_write('S');
    serial_write(redSize_int);

}

int main (void) {
    init_pwm();
    serial_init();
    i2c_init();
    rgb_init();

    // Turn on timer with no prescaler on the clock for fastest
	// triggering of the interrupt service routine.
	TCCR1B |= (1 << CS22)|(1 << CS21); // initialize timer with prescaler 256
	TIMSK1 |= _BV(TOIE1);

    // enable interupts
    sei();

	for (;;) {
        blue_light_curve();
        // serial_write('E');
	}
}