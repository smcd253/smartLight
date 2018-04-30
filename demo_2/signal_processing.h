#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

/* buf structure */
#define BUF_ELEMENTS 20
#define BUF_SIZE (BUF_ELEMENTS + 1)
uint32_t redBuf[BUF_SIZE], greenBuf[BUF_SIZE], blueBuf[BUF_SIZE];
uint32_t bufIn, bufOut;

// declare functions
void bufInit(void);
int bufPut(uint16_t newRed, uint8_t newGreen, uint8_t newBlue);
int bufPop();

#endif