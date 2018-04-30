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

        uint8_t scaledRed = (uint8_t)(((double)redAvg/65536) * 255);
        uint8_t scaledGreen = (uint8_t)(((double)greenAvg/65536) * 255);
        uint8_t scaledBlue = (uint8_t)(((double)blueAvg/65536) * 255);
        
        /*************** ADAPTIVE PWM **********************/
        pwm_curve(timeNow, pwm_act, pwm_target);
        // pwm attempt2 TO TRY TOMORROW
        serial_write_uint16(scaledRed);
        if (scaledRed < pwm_target[0]){
            uint8_t start = pwm_act[0];
            //uint8_t end = (uint8_t)(((double)scaledRed/(double)pwm_target[0]) * 255);
            uint8_t end = scaledRed;

            uint8_t i;
            // gradualy bring pwm_act to scaledRed
            for (i = start; i < end; i++){
                pwm_act[0] = i;
                OCR1A = pwm_act[0];
                send_red(pwm_act[0]);

                _delay_ms(5);
            }
        }
            
        else
            pwm_act[0] = pwm_target[0];
        

        /*************** SEND PWM DATA TO PERIPH BOARD **********************/
        send_red(pwm_act[0]);
        OCR1A = pwm_act[0];


        // serial_write_uint16(timeNow[0]);
        // serial_write_string(":");
        // serial_write_uint16(timeNow[1]);
        // serial_write_string(":");
        // serial_write_uint16(timeNow[2]);
        // serial_write_string("    ");

        //_delay_ms(1000);*/
        _delay_ms(100);
	}
}