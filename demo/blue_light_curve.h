#ifndef BLUE_LIGHT_CURVE_H
#define BLUE_LIGHT_CURVE_H

#include <avr/io.h>

// global variables
volatile uint8_t pwmr, pwmg, pwmb;
volatile uint8_t *pwm_day;
volatile uint8_t *pwm_evening;
volatile uint8_t *pwm_night;
volatile uint8_t* pwm_target;

// pwm states
enum CURVE_STATE{
    day,
    evening,
    night
} cState;



// function definitions

void init_pwm(void);
void blue_light_curve(void);

#endif