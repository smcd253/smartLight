#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "i2c.h"
#include "rgb_sensor.h"


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
    // serial_write_uint16(data);
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