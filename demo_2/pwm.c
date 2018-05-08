/**
 * A PWM example for the ATmega328P using the 8-Bit Fast PWM mode.
 */
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "pwm.h"
#include "realtime.h"
#include "serial.h"

uint8_t pwm_day[3] = {255, 255, 255};
uint8_t pwm_evening[3] = {255, 128, 0};
uint8_t pwm_night[3] = {0, 0, 0};

bool awake = false;
uint8_t pir_timer = 0;

void pir_init() {
	DDRC &= ~(1 << PC3);
	PORTC &= ~(1 << PC3);  
}

bool pir_is_on() {
	return (PINC & (1 << PC3)) != 0;
}

void init_pwm(){
	/**
	 * We will be using OCR1A as our PWM output which is the
	 * same pin as PB1.
	 */
	DDRB |= _BV(PB1); // 15
    DDRD |= _BV(PD5) | _BV(PD6); // 11 12
    

	/**
	 * There are quite a number of PWM modes available but for the
	 * sake of simplicity we'll just use the 8-bit Fast PWM mode.
	 * This is done by setting the WGM10 and WGM12 bits.  We 
	 * Setting COM1A1 tells the microcontroller to set the 
	 * output of the OCR1A pin low when the _timer's counter reaches
	 * a compare value (which will be explained below).  CS10 being
	 * set simply turns the _timer on without a prescaler (so at full
	 * speed).  The _timer is used to determine when the PWM pin should be
	 * on and when it should be off.
	 */
	TCCR1A |= _BV(COM1A1) | _BV(WGM10);
	TCCR1B |= _BV(CS10) | _BV(WGM12);
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);
	TCCR0B |= _BV(CS00);
}

// takes in _time[3] by reference and outputs the proper pwm values
bool daytime = false, evening = false, sleep = false;
uint8_t oldSec;
void pwm_curve(uint8_t* _time, uint8_t* pwm_act, uint8_t* pwm_target){
    // if ((_time[0] >= WAKEUP_HOR) && (_time[0] < DIM_HOR)){
    //     daytime = true;
    //     if ((_time[1] < WAKEUP_MIN) && (_time[0] == WAKEUP_HOR)){
    //         daytime = false;
    //     }
    // }

    // if ((_time[0] >= DIM_HOR) && (_time[0] < SLEEP_HOR)){
    //     evening = true;
    //     if ((_time[1] < DIM_MIN) && (_time[0] == DIM_HOR)){
    //         evening = false;
    //     }
    // }

    // if ((_time[0] >= SLEEP_HOR) && (_time[0] < 0)){
    //     sleep = true;
    //     if ((_time[1] < SLEEP_MIN) && (_time[0] == SLEEP_HOR)){
    //         sleep = false;
    //     }
    // }
    // else if ((_time[0] >= 0) && (_time[0] < WAKEUP_HOR)){
    //     sleep = true;
    //     if ((_time[1] < SLEEP_MIN) && (_time[0] == 0)){
    //         sleep = false;
    //     }
    // }

    if ((_time[2] >= 00) && (_time[2] < 10)){
        daytime = true;
        evening = false;
        sleep = false;
    }
    else if ((_time[2] >= 20) && (_time[2] < 30)){
        daytime = false;
        evening = true;
        sleep = false;
    }
    else if ((_time[2] >= 40) && (_time[2] < 50)){
        daytime = false;
        evening = false;
        sleep = true;
    }
    else{
        daytime = false;
        evening = false;
        sleep = false;
    }
    if (daytime){
        // serial_write_string("daytime");
        
        pwm_target[0] = pwm_day[0];
        pwm_target[1] = pwm_day[1];
        pwm_target[2] = pwm_day[2];

        // set target pwm values

        pwm_act[0] = (_time[2]*pwm_target[0])/10;
        pwm_act[1] = (_time[2]*pwm_target[1])/10;
        pwm_act[2] = (_time[2]*pwm_target[2])/10;
        
        // if (_time[2] != oldSec){
        //     if (pwm_act[0] < pwm_target[0]){
        //         pwm_act[0] += 20;
        //     }
        //     else if(pwm_act[0] > pwm_target[0]){
        //         pwm_act[0] -= 20;
        //     }
        //     if (pwm_act[1] < pwm_target[1]){
        //         pwm_act[1] += 20;
        //     }
        //     else if(pwm_act[1] > pwm_target[1]){
        //         pwm_act[1] -= 20;
        //     }
        //     if (pwm_act[2] < pwm_target[2]){
        //         pwm_act[2] += 20;
        //     }
        //     else if(pwm_act[2] > pwm_target[2]){
        //     }
        // }
    }
    else if (evening){  
        // serial_write_string("evening");
        pwm_target[0] = pwm_evening[0];
        pwm_target[1] = pwm_evening[1];
        pwm_target[2] = pwm_evening[2];

        pwm_act[0] = 255;
        pwm_act[1] = 255 - (_time[2] - 20) * 12;
        pwm_act[2] = 255 - (_time[2] - 20) * 25;
        //pwm_act[0] = (uint8_t)(((uint16_t)_time[2] - 20)*(uint16_t)pwm_target[0] + (30 - (uint16_t)_time[2]) * (uint16_t)pwm_day[0])/10;
        //pwm_act[1] = (uint8_t)(((uint16_t)_time[2] - 20)*(uint16_t)pwm_target[1] + (30 - (uint16_t)_time[2]) * (uint16_t)pwm_day[1])/10;
        //pwm_act[2] = (uint8_t)(((uint16_t)_time[2] - 20)*(uint16_t)pwm_target[2] + (30 - (uint16_t)_time[2]) * (uint16_t)pwm_day[2])/10;

        // // set target pwm values
        // if (_time[2] != oldSec){
        //     if (pwm_act[0] < pwm_target[0]){
        //         pwm_act[0] += 20;
        //     }
        //     else if(pwm_act[0] > pwm_target[0]){
        //         pwm_act[0] -= 20;
        //     }
        //     if (pwm_act[1] < pwm_target[1]){
        //         pwm_act[1] += 20;
        //     }
        //     else if(pwm_act[1] > pwm_target[1]){
        //         pwm_act[1] -= 20;
        //     }
        //     if (pwm_act[2] < pwm_target[2]){
        //         pwm_act[2] += 20;
        //     }
        //     else if(pwm_act[2] > pwm_target[2]){
        //     }
        // }
    }
    else if (sleep){
        // serial_write_string("night");
        
        if (pir_is_on()){
            awake = true;
        }
        if (awake){
            pwm_target[0] = pwm_evening[0];
            pwm_target[1] = pwm_evening[1];
            pwm_target[2] = pwm_evening[2];
            pir_timer++;
            if (pir_timer >= 30){
                awake = false;
            }
        }
        else{
            pwm_target[0] = pwm_night[0];
            pwm_target[1] = pwm_night[1];
            pwm_target[2] = pwm_night[2];
        }

        // pwm_act[0] = 0;
        // pwm_act[1] = 255 - (_time[2] - 40) * 25;
        // pwm_act[2] = 255 - (_time[2] - 40) * 25;
        pwm_act[0] = ((_time[2] - 40)*pwm_target[0] + ((50 - _time[2]) * pwm_evening[0]))/10;
        pwm_act[1] = ((_time[2] - 40)*pwm_target[1] + ((50 - _time[2]) * pwm_evening[1]))/10;
        pwm_act[2] = ((_time[2] - 40)*pwm_target[2] + ((50 - _time[2]) * pwm_evening[2]))/10;

        // // set target pwm values
        // if (_time[2] != oldSec){
        //     if (pwm_act[0] < pwm_target[0]){
        //         pwm_act[0] += 20;
        //     }
        //     else if(pwm_act[0] > pwm_target[0]){
        //         pwm_act[0] -= 20;
        //     }
        //     if (pwm_act[1] < pwm_target[1]){
        //         pwm_act[1] += 20;
        //     }
        //     else if(pwm_act[1] > pwm_target[1]){
        //         pwm_act[1] -= 20;
        //     }
        //     if (pwm_act[2] < pwm_target[2]){
        //         pwm_act[2] += 20;
        //     }
        //     else if(pwm_act[2] > pwm_target[2]){
        //     }
        // }
    }
    oldSec = _time[2];
}                