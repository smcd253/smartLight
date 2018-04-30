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

// void blue_light_curve(){
//     int i;

//     // pwmx = 255 --> WHITE
//     pwmr = 255;
//     pwmg = 255;
//     pwmb = 255;
    
//     OCR1A = pwmr;
//     OCR0B = pwmg;
//     OCR0A = pwmb;

//     _delay_ms(2000);
//     // dim blue
//     for (i = 0; i < 128; i++) {
//         cli();
//         pwmb--;
//         OCR0A = pwmb;
//         sei();
//         _delay_ms(10);
//     }

//     _delay_ms(2000);
//     // dim green and blue
//     for (i = 0; i < 127; i++) {
//         cli();
//         pwmb--;
//         pwmg--;
//         OCR0B = pwmg;
//         OCR0A = pwmb;
//         sei();
//         _delay_ms(10);
//     }

//     _delay_ms(2000);
//     // dim green
//     for (i = 0; i < 128; i++) {
//         cli();
//         pwmg--;
//         OCR0B = pwmg;
//         sei();
//         _delay_ms(10);
//     }

//     _delay_ms(2000);
//     // brighten green
//     for (i = 0; i < 128; i++) {
//         cli();
//         pwmg++;
//         OCR0B = pwmg;
//         sei();
//         _delay_ms(10);
//     }

//     _delay_ms(2000);
//     // brighten green and blue
//     for (i = 0; i < 127; i++) {
//         cli();
//         pwmg++;
//         pwmb++;
//         OCR0B = pwmg;
//         OCR0A = pwmb;
//         sei();
//         _delay_ms(10);
//     }

//     // brighten blue
//     for (i = 0; i < 128; i++) {
//         cli();
//         pwmb++;
//         OCR0A = pwmb;
//         sei();
//         _delay_ms(10);
//     }
// }

// bool transitioning = false;
// bool transition_finished = false;


// // void brite(uint8_t* pwm_target){

// // }
// takes in _time[3] by reference and outputs the proper pwm values
void pwm_curve(uint8_t* _time, uint8_t* pwm_target){
    bool daytime = (
                    (_time[0] >= WAKEUP_HOR) && (_time[0] < DIM_HOR)
                    /*&& (_time[1] >= WAKEUP_MIN) && (_time[1] < DIM_MIN)*/
                    );
    // if ((_time[0] >= WAKEUP_HOR) && (_time[0] < DIM_HOR)){
    //     daytime = true;
    //     if ((_time[1] >= WAKEUP_MIN) && (_time[1] < DIM_MIN))
    // }
    bool evening = (
                    (_time[0] >= DIM_HOR) && (_time[0] < SLEEP_HOR)
                    && (_time[1] >= DIM_MIN) && (_time[1] < SLEEP_MIN)
                    );
    bool sleep = (
                    ((_time[0] >= SLEEP_HOR) && (_time[0] < 0)
                    && (_time[1] >= SLEEP_MIN) && (_time[1] < 0))
                    || 
                    ((_time[0] >= 0) && (_time[0] < WAKEUP_HOR)
                    && (_time[1] >= 0) && (_time[1] < WAKEUP_MIN))
                    );
    if (daytime){
        // set target pwm values
        // if(!transitioning && !transition_finished){
        //     pwm_target = pwm_day;
        //     transitioning = true;
        // }
        // else if (transitioning && !transition_finished){
        //     // brite(pwm_target); // should output transition_finished true when done
        // }
        // else if (transitioning && transition_finished){
        //     transition_finished = true;
        // }
        pwm_target[0] = pwm_day[0];
        pwm_target[1] = pwm_day[1];
        pwm_target[2] = pwm_day[2];
        
    }
    else if (evening){
        // set target pwm values
        // if(!transitioning && !transition_finished){
        //     pwm_target = pwm_evening;
        //     transitioning = true;
        // }
        // else if (transitioning && !transition_finished){
        //     // dim(pwm_target); // should output transition_finished true when done
        // }
        // else if (transitioning && transition_finished){
        //     transition_finished = true;
        // }
        pwm_target = pwm_evening;
    }
    else if (sleep){
        // // set target pwm values
        // if(!transitioning && !transition_finished){
        //     pwm_target = pwm_night;
        //     transitioning = true;
        // }
        // else if (transitioning && !transition_finished){
        //     // dim(pwm_target); // should output transition_finished true when done
        // }
        // else if (transitioning && transition_finished){
        //     transition_finished = true;
        // }
        pwm_target = pwm_night;
    }
}                