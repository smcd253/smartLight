#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "i2c.h"
/*#include "rgb_sensor.h"
#include "blue_light_curve.h"
#include "time.h"*/

int a = 0;
char redSize;
uint8_t redSize_int;

int main (void) {    
    //init_pwm();
    //_delay_ms(1000);
    serial_init();
    serial_write_string("something");
    //_delay_ms(1000);
    i2c_init();
    _delay_ms(1000);
    serial_write_string(" is working");
    rgb_init();
    serial_write_string(" quite well");

    uint8_t timeNow[3] = {0, 0, 0};
    
	for (;;) {
        serial_write('R');
        serial_write_uint16(readRed());
        serial_write('G');
        serial_write_uint16(readGreen());
        serial_write('B');
        serial_write_uint16(readBlue());
        readTime(timeNow);

        serial_write_uint16((uint16_t)timeNow[0]);
        serial_write_string(":");
        serial_write_uint16(timeNow[1]);
        serial_write_string(":");
        serial_write_uint16(timeNow[2]);
        serial_write_string("    ");


        //_delay_ms(1000);*/
        _delay_ms(2000);
	}
}