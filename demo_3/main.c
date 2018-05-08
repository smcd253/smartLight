#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "i2c.h"
#include "rgb.h"
#include "pwm.h"
#include "realtime.h"
#include "signal_processing.h"

int a = 0;
char redSize;
uint8_t redSize_int;

uint8_t pwm_act[3];
uint8_t pwm_target[3];

uint8_t scaledRed;
uint8_t scaledGreen;
uint8_t scaledBlue;

void setRed(){
    OCR1A = pwm_act[0];
    send_red(pwm_act[0]);
}

void setGreen(){
    OCR0B = pwm_act[1];
    send_green(pwm_act[1]);
}

void setBlue(){
    OCR0A = pwm_act[2];
    send_blue(pwm_act[2]);
}
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
        serial_write_uint16(timeNow[0]);
        serial_write_string(":");
        serial_write_uint16(timeNow[1]);
        serial_write_string(":");
        serial_write_uint16(timeNow[2]);
        serial_write_string("    ");

        uint16_t red = readRed();
        uint16_t green = readGreen();
        uint16_t blue = readBlue();
        if (bufPut(red, green, blue) == -1){
            bufPop();
            bufPut(red, green, blue);
        }
        
        /*************** SIGNAL PROCESSING **********************/
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

        scaledRed = (uint8_t)(redAvg/257);
        scaledGreen = (uint8_t)(greenAvg/257);
        scaledBlue = (uint8_t)(blueAvg/257);
        
        /*************** ADAPTIVE PWM **********************/
        pwm_curve(timeNow, pwm_act, pwm_target);
        // pwm attempt2 TO TRY TOMORROW

        if (scaledRed > pwm_target[0]/2){
            uint8_t start = pwm_act[0];
            uint8_t end = pwm_target[0]/2;

            uint8_t i;
            // gradualy bring pwm_act to scaledRed
            for (i = start; i > end; i--){
                pwm_act[0] = i;
                setRed();
                _delay_ms(10);
            }
        }   
        else{
            pwm_act[0] = pwm_target[0];
        }

        if (scaledGreen > pwm_target[1]/2){
            uint8_t start = pwm_act[1];
            uint8_t end = pwm_target[1]/2;

            uint8_t i;
            // gradualy bring pwm_act to scaledGreen
            for (i = start; i > end; i--){
                pwm_act[1] = i;
                setGreen();
                _delay_ms(10);
            }
        }   
        else{
            pwm_act[1] = pwm_target[1];
        }
            
        if (scaledBlue > pwm_target[2]/2){
            uint8_t start = pwm_act[2];
            uint8_t end = pwm_target[2]/2;

            uint8_t i;
            // gradualy bring pwm_act to scaledRed
            for (i = start; i > end; i--){
                pwm_act[2] = i;
                setBlue();
                _delay_ms(10);
            }
        }   
        else{
            pwm_act[2] = pwm_target[2];
        }

        /*************** SET PWM and SEND DATA TO PERIPH BOARD **********************/
        setRed();
        setGreen();
        setBlue();

        //_delay_ms(1000);*/
        _delay_ms(100);
	}
}