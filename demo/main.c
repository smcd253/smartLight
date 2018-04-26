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
ISR(TIMER2_OVF_vect){
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
    // serial_write('S');
    // serial_write(redSize_int);
}

int main (void) {
    init_pwm();
    serial_init();
    i2c_init();
    rgb_init();

    serial_write('Begining of Main');
    // // Turn on timer2 with 1024 prescaler
    // use timer2 so as not to interfere with pwm
	TCCR2B |= (1 << CS20)|(1 << CS22); // initialize timer with prescaler 1024
    TIMSK2 |= _BV(TOIE2); // enable timer overflow signal


    // // enable interupts
    sei();

	for (;;) {
        blue_light_curve();
        // serial_write('E');
	}
}