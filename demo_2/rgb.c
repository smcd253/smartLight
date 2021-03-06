#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "i2c.h"
#include "rgb.h"

uint8_t read8(uint8_t reg) {
    uint8_t data;
    i2c_io(ISL_I2C_ADDR, NULL, 0, &reg, 1, NULL, 0);
    i2c_io(ISL_I2C_ADDR, NULL, 0, NULL, 0, &data, 1);
    return data;
}

uint16_t read16(uint8_t reg) {
    uint8_t array[2];
    array[0] = 2;
    array[1] = 2;
    if (i2c_io(ISL_I2C_ADDR, NULL, 0, &reg, 1, NULL, 0)) {
        serial_write_string(" Read16_w failed");
        return 0;
    }
    if (i2c_io(ISL_I2C_ADDR, NULL, 0, NULL, 0, array, 2)) {
        serial_write_string(" Read16_r failed");
        return 0;
    }
    uint16_t ret = array[0];
    ret = ret << 8;
    ret = ret + array[1];
    return ret;
}

void write8(uint8_t reg, uint8_t data) {
    uint8_t array[2];
    array[0] = reg;
    array[1] = data;
    if (i2c_io(ISL_I2C_ADDR, NULL, 0, array, 2, NULL, 0)) {
        serial_write_string(" Write8 failed");
    }
}

bool reset() {
    uint8_t data = 0x00;
    // Reset registers
    write8(DEVICE_ID, 0x46);
    // Check reset
    data = read8(CONFIG_1);
    data |= read8(CONFIG_2);
    data |= read8(CONFIG_3);
    data |= read8(STATUS);
    return data == 0;
}

bool config(uint8_t config1, uint8_t config2, uint8_t config3) {
    uint8_t data = 0x00;
  
    // Set 1st configuration register
    write8(CONFIG_1, config1);
    // Set 2nd configuration register
    write8(CONFIG_2, config2);
    // Set 3rd configuration register
    write8(CONFIG_3, config3);
  
    // Check if configurations were set correctly
    data = read8(CONFIG_1);
    if (data != config1)
        return false;
    data = read8(CONFIG_2);
    if (data != config2)
        return false;
    data = read8(CONFIG_3);
    if (data != config3)
        return false;
    return true;
}

bool rgb_init() {
    uint16_t data = 0x00;
    bool ret = true;
    data = read8(DEVICE_ID);
    serial_write_string("DEVICE_ID:");
    serial_write_uint16(data);
    if (data != 0x7D) {
        //ret &= false;
        serial_write_string(" data!=0x7D");
    }
    serial_write_string(" CheckedID");
    ret &= reset();
    serial_write_string(" Reset");
    // Set to RGB mode, 10k lux, and high IR compensation
    ret &= config(CFG1_MODE_RGB | CFG1_10KLUX, CFG2_IR_ADJUST_HIGH, CFG_DEFAULT);
    serial_write_string("Config");
    return ret;
}

uint16_t readRed() {
  return read16(RED_L);
}

uint16_t readGreen() {
  return read16(GREEN_L);
}

uint16_t readBlue() {
  return read16(BLUE_L);
}

// void rgb_feeback(uint8_t *pwmr_act, uint8_t *pwmg_act, uint *pwmb_act,
//                 uint8_t *pwmr, uint8_t *pwmg, uint8_t *pwmb
//                 uint16_t *redAvg, uint16_t *greenAvg, uint16_t *blueAvg){
//     // scale averages to 255 scale for pwm
//     int scaledRed = (int)(((double)redAvg/65536) * 255);
//     int scaledGreen = (int)(((double)greenAvg/65536) * 255);
//     int scaledBlue = (int)(((double)redBlue/65536) * 255);
    
//     serial_write('R');
//     serial_write_uint16(scaledRed);
//     serial_write('G');
//     serial_write_uint16(scaledGreen);
//     serial_write('B');
//     serial_write_uint16(scaledBlue);
// }

// void rgb_feedback(){
//     uint8_t data;
//     char redSize;
//     int redSize_int;
//     int a = 0;
//     // rgb feedback
//     data = read8(DEVICE_ID);
//     redBuf[a] = 1;
//     a += 1;
//     redSize = sizeof(redBuf)/sizeof(redBuf[0]);
//     redSize_int = (uint8_t)redSize;
//     serial_write('S');
//     serial_write(redSize_int);
// }