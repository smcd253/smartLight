#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "i2c.h"
#include "rgb_sensor.h"
#include "blue_light_curve.h"
#include "time.h"

int a = 0;
char redSize;
uint8_t redSize_int;

// // ISR for RGB sensor values
// ISR(TIMER2_OVF_vect){
//     uint8_t data;
//     data = read8(DEVICE_ID);
//     //serial_write_string(" DECIVE_ID:");
//     //serial_write_uint16(data);
//     // serial_write('R');
//     // serial_write_uint16(readRed());
//     // serial_write('G');
//     // serial_write_uint16(readGreen());
//     // serial_write('B');
//     // serial_write_uint16(readBlue());

//     // trying to find size of array and print to screen...
//     redBuf[a] = 1;
//     a += 1;
//     redSize = sizeof(redBuf)/sizeof(redBuf[0]);
//     redSize_int = (int)redSize;
//     // serial_write('S');
//     // serial_write(redSize_int);
// }

int main (void) {    
    init_pwm();
    _delay_ms(1000);
    serial_init();
    _delay_ms(1000);
    i2c_init();
    _delay_ms(1000);
    rgb_init();

    uint8_t* timeNow;
    
	for (;;) {
        timeNow = readTime();

        serial_write_uint16(timeNow[0]);
        serial_write_string(":");
        serial_write_uint16(timeNow[1]);
        serial_write_string(":");
        serial_write_uint16(timeNow[2]);
        serial_write_string("    ");


        _delay_ms(1000);
	}
}