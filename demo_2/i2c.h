#ifndef I2C_H
#define I2C_H

#define NULL 0
#define FOSC 7372800

void i2c_init();

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn);

#endif