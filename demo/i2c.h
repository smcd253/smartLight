#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

#define NULL 0
#define FOSC 7372800
//#define BDIV (FOSC/500000-16)/2+1
#define BDIV (FOSC/500000-16)/2+1
#define BAUD_PRESCALE 47

void i2c_init();

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn);

uint8_t read8(uint8_t reg);

uint16_t read16(uint8_t reg);

void write8(uint8_t reg, uint8_t data);

#endif