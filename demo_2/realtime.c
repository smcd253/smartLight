#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <stdlib.h>

#include "common.h"
#include "serial.h"
#include "i2c.h"
#include "realtime.h"

uint8_t read8_clk(uint8_t reg) {
    uint8_t data;
    i2c_io(RTCLK_I2C_ADDR, NULL, 0, &reg, 1, NULL, 0);
    i2c_io(RTCLK_I2C_ADDR, NULL, 0, NULL, 0, &data, 1);
    return data;
}

uint16_t read16_clk(uint8_t reg) {
    uint8_t array[2];
    array[0] = 2;
    array[1] = 2;
    if (i2c_io(RTCLK_I2C_ADDR, NULL, 0, &reg, 1, NULL, 0)) {
        serial_write_string(" Read16_w failed");
        return 0;
    }
    if (i2c_io(RTCLK_I2C_ADDR, NULL, 0, NULL, 0, array, 2)) {
        serial_write_string(" Read16_r failed");
        return 0;
    }
    uint16_t ret = array[0];
    ret = ret << 8;
    ret = ret + array[1];
    return ret;
}

void write8_clk(uint8_t reg, uint8_t data) {
    uint8_t array[2];
    array[0] = reg;
    array[1] = data;
    if (i2c_io(RTCLK_I2C_ADDR, NULL, 0, array, 2, NULL, 0)) {
        serial_write_string(" Write8 failed");
    }
}

void init_rtclk(){
    write8_clk(SEC, TIME_SEC);
    write8_clk(MIN, TIME_MIN);
    write8_clk(HOR, TIME_HOR);
}

void readTime(uint8_t* _time){
    _time[2] = read8_clk(SEC) & ((1 << 6) - 1);
    _time[1] = read8_clk(MIN) & ((1 << 6) - 1);
    _time[0] = read8_clk(HOR) & ((1 << 5) - 1);
}