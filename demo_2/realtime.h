#ifndef REALTIME_H
#define REALTIME_H

/*
* already defined in rgb_sensor.h
TODO: 1. make a general i2c script and header file
    2. make specific time and rgb_sensor scripts and header files
*/

#define RTCLK_I2C_ADDR 0xA3
#define SEC 0x02
#define MIN 0x03
#define HOR 0x04

// alarm info
#define WAKEUP_MIN 00
#define WAKEUP_HOR 06
#define DIM_MIN 00
#define DIM_HOR 20
#define SLEEP_MIN 00
#define SLEEP_HOR 22

// current time for initialization
#define TIME_SEC 0
#define TIME_MIN 1
#define TIME_HOR 16

void init_rtclk(void);

void readTime(uint8_t* _time);

#endif