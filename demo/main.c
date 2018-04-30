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

int main (void) {    
    init_pwm();
    _delay_ms(1000);
    serial_init();
    _delay_ms(1000);
    i2c_init();
    _delay_ms(1000);
    rgb_init();

    uint8_t timeNow[3] = {0, 0, 0};
    
	for (;;) {
        readTime(timeNow);

        serial_write_uint16((uint16_t)timeNow[0]);
        serial_write_string(":");
        serial_write_uint16(timeNow[1]);
        serial_write_string(":");
        serial_write_uint16(timeNow[2]);
        serial_write_string("    ");


        _delay_ms(1000);
	}
}