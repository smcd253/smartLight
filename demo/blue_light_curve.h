#ifndef BLUE_LIGHT_CURVE_H
#define BLUE_LIGHT_CURVE_H

#include <avr/io.h>

// global variables
volatile uint8_t pwmr;
volatile uint8_t pwmg;
volatile uint8_t pwmb;

// function definitions

void init_pwm(void);
void blue_light_curve(void);

#endif