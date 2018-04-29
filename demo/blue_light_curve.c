/**
 * A PWM example for the ATmega328P using the 8-Bit Fast PWM mode.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#include "blue_light_curve.h"
#include "rgb_sensor.h"


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
	 * output of the OCR1A pin low when the timer's counter reaches
	 * a compare value (which will be explained below).  CS10 being
	 * set simply turns the timer on without a prescaler (so at full
	 * speed).  The timer is used to determine when the PWM pin should be
	 * on and when it should be off.
	 */
	TCCR1A |= _BV(COM1A1) | _BV(WGM10);
	TCCR1B |= _BV(CS10) | _BV(WGM12);
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);
	TCCR0B |= _BV(CS00);

    // set initial colors
    // {red, green, blue}
    pwm_day[0] = 255;
    pwm_day[1] = 255;
    pwm_day[2] = 255;
    
    pwm_evening[0] = 255;
    pwm_evening[1] =  128;
    pwm_evening[2] = 0;

    pwm_night[0] = 0;
    pwm_night[1] = 0;
    pwm_night[2] = 0;
}

void blue_light_curve(){
    int i;

    // pwmx = 255 --> WHITE
    pwmr = 255;
    pwmg = 255;
    pwmb = 255;
    
    OCR1A = pwmr;
    OCR0B = pwmg;
    OCR0A = pwmb;

    _delay_ms(2000);
    // dim blue
    for (i = 0; i < 128; i++) {
        cli();
        pwmb--;
        OCR0A = pwmb;
        sei();
        _delay_ms(10);
    }

    _delay_ms(2000);
    // dim green and blue
    for (i = 0; i < 127; i++) {
        cli();
        pwmb--;
        pwmg--;
        OCR0B = pwmg;
        OCR0A = pwmb;
        sei();
        _delay_ms(10);
    }

    _delay_ms(2000);
    // dim green
    for (i = 0; i < 128; i++) {
        cli();
        pwmg--;
        OCR0B = pwmg;
        sei();
        _delay_ms(10);
    }

    _delay_ms(2000);
    // brighten green
    for (i = 0; i < 128; i++) {
        cli();
        pwmg++;
        OCR0B = pwmg;
        sei();
        _delay_ms(10);
    }

    _delay_ms(2000);
    // brighten green and blue
    for (i = 0; i < 127; i++) {
        cli();
        pwmg++;
        pwmb++;
        OCR0B = pwmg;
        OCR0A = pwmb;
        sei();
        _delay_ms(10);
    }

    // brighten blue
    for (i = 0; i < 128; i++) {
        cli();
        pwmb++;
        OCR0A = pwmb;
        sei();
        _delay_ms(10);
    }
}

// bool transitioning = false;
// bool transition_finished = false;


// // void brite(uint8_t* pwm_target){

// // }
// // takes in time[3] by reference and outputs the proper pwm values
// void pwm_curve(uint8_t* time){
//     bool daytime = (
//                     (time[0] >= WAKEUP_HOR) && (time[0] < DIM_HOR)
//                     && (time[1] >= WAKEUP_MIN) && (time[1] < DIM_MIN)
//                     );
//     bool evening = (
//                     (time[0] >= DIM_HOR) && (time[0] < SLEEP_HOR)
//                     && (time[1] >= DIM_MIN) && (time[1] < SLEEP_MIN)
//                     );
//     bool sleep = (
//                     ((time[0] >= SLEEP_HOR) && (time[0] < 0)
//                     && (time[1] >= SLEEP_MIN) && (time[1] < 0))
//                     || 
//                     ((time[0] >= 0) && (time[0] < WAKEUP_HOR)
//                     && (time[1] >= 0) && (time[1] < WAKEUP_MIN))
//                     );
//     if (daytime){
//         // set target pwm values
//         if(!transitioning && !transition_finished){
//             pwm_target = pwm_day;
//             transitioning = true;
//         }
//         else if (transitioning && !transition_finished){
//             // brite(pwm_target); // should output transition_finished true when done
//         }
//         else if (transitioning && transition_finished){
//             transition_finished = true;
//         }
//     }
//     else if (evening){
//         // set target pwm values
//         if(!transitioning && !transition_finished){
//             pwm_target = pwm_evening;
//             transitioning = true;
//         }
//         else if (transitioning && !transition_finished){
//             // dim(pwm_target); // should output transition_finished true when done
//         }
//         else if (transitioning && transition_finished){
//             transition_finished = true;
//         }
//     }
//     else if (sleep){
//         // set target pwm values
//         if(!transitioning && !transition_finished){
//             pwm_target = pwm_night;
//             transitioning = true;
//         }
//         else if (transitioning && !transition_finished){
//             // dim(pwm_target); // should output transition_finished true when done
//         }
//         else if (transitioning && transition_finished){
//             transition_finished = true;
//         }
//     }
// }                