#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "i2c.h"
#include "rgb.h"
#include "pwm.h"
#include "realtime.h"
#include "signal_processing.h"

#define PWM_BUF 20
#define PWM_LIMIT 80

int a = 0;
char redSize;
uint8_t redSize_int;

uint8_t pwm_act[3];
uint8_t pwm_target[3];


int main (void) {    
    init_pwm();
    serial_init();
    i2c_init();
    rgb_init();
    bufInit();

    init_rtclk();

    uint8_t timeNow[3] = {0, 0, 0};
    int n;
	for (;;) {
        readTime(timeNow);

        uint16_t red = readRed();
        uint16_t green = readGreen();
        uint16_t blue = readBlue();
        if (bufPut(red, green, blue) == -1){
            bufPop();
            bufPut(red, green, blue);
        }
        
        // build average to mitigate noise
        uint32_t redTot = 0, greenTot = 0, blueTot = 0;
        uint16_t redAvg, greenAvg, blueAvg;
        int i;
        for (i = 0; i < BUF_SIZE; i++){
            redTot += (uint32_t)redBuf[i];
            greenTot += (uint32_t)greenBuf[i];
            blueTot += (uint32_t)blueBuf[i];

        }
        redAvg = (uint16_t)(redTot/BUF_SIZE);
        greenAvg = (uint16_t)(greenTot/BUF_SIZE);
        blueAvg = (uint16_t)(blueTot/BUF_SIZE);

        uint8_t scaledRed = (uint8_t)(((double)redAvg/65536) * 255);
        uint8_t scaledGreen = (uint8_t)(((double)greenAvg/65536) * 255);
        uint8_t scaledBlue = (uint8_t)(((double)blueAvg/65536) * 255);

        // serial_write('R');
        // serial_write_uint16(scaledRed);
        // serial_write('G');
        // serial_write_uint16(scaledGreen);
        // serial_write('B');
        // serial_write_uint16(scaledBlue);
        
        pwm_curve(timeNow, pwm_target);
        // pwm attempt2 TO TRY TOMORROW
        if (scaledRed < pwm_target[0])
            pwm_act[0] = (uint8_t)(((double)scaledRed/(double)pwm_target[0]) * 255);
        else
            pwm_act[0] = pwm_target[0];
            
        // pwm attempt1
        // // serial_write_uint16((uint16_t)pwm_target[0]);
        // if(pwm_act[0] < pwm_target[0] - PWM_LIMIT){
        //     if(n < PWM_LIMIT){
        //         pwm_act[0] += 1;
        //         n++;                
        //     }
        //     else{
        //         n = 0;
        //     }
        //     // serial_write_string("too low");
        // }
        // else if (pwm_act[0] > pwm_target[0] + PWM_LIMIT){
        //     if(n < PWM_LIMIT){
        //         pwm_act[0] -= 1;
        //         n++;                
        //     }
        //     else{
        //         n = 0;
        //     }
        //     // serial_write_string("too high");
            
        // }
        // else{
        //     if(scaledRed > pwm_target[0] + PWM_BUF)
        //         pwm_act[0] -= 1;            
        //     else if (scaledRed < pwm_target[0] - PWM_BUF)
        //         pwm_act[0] += 1;
        //     // serial_write_string("control");
            
        // }
        // serial_write_uint16((uint16_t)pwm_act[0]);
        
        OCR1A = pwm_act[0];

        serial_write_uint16(timeNow[0]);
        serial_write_string(":");
        serial_write_uint16(timeNow[1]);
        serial_write_string(":");
        serial_write_uint16(timeNow[2]);
        serial_write_string("    ");



        //_delay_ms(1000);*/
        _delay_ms(100);
	}
}