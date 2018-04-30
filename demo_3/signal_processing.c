#include <avr/io.h>
#include "signal_processing.h"

// source: https://stackoverflow.com/questions/215557/how-do-i-implement-a-circular-list-ring-buffer-in-c

void bufInit(void)
{
    bufIn = bufOut = 0;
}

int bufPut(uint16_t newRed, uint8_t newGreen, uint8_t newBlue)
{
    if(bufIn == (( bufOut - 1 + BUF_SIZE) % BUF_SIZE))
    {
        return -1; /* buf Full*/
    }

    redBuf[bufIn] = newRed;
    greenBuf[bufIn] = newGreen;
    blueBuf[bufIn] = newBlue;
    
    bufIn = (bufIn + 1) % BUF_SIZE;

    return 0; // No errors
}

int bufPop()
{
    if(bufIn == bufOut)
    {
        return -1; /* buf Empty - nothing to get*/
    }

    bufOut = (bufOut + 1) % BUF_SIZE;

    return 0; // No errors
}