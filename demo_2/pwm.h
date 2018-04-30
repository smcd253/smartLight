#ifndef BLUE_LIGHT_CURVE_H
#define BLUE_LIGHT_CURVE_H

// pwm states
enum CURVE_STATE{
    day,
    evening,
    night
} cState;



// function definitions

void init_pwm(void);
void pwm_curve(uint8_t* _time, uint8_t* pwm_target);

#endif