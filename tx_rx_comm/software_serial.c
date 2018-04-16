#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#define BAUD 9600
#define STOP_BITS 1

#define SOFT_TX_BIT 0
#define SOFT_TX_DDR DDRB
#define SOFT_TX_PORT PORTB

#define MICROSECONDS_OVERHEAD_ADJUST 0

#define MICROSECONDS_PER_BIT ((1000000ul / BAUD) - MICROSECONDS_OVERHEAD_ADJUST)

FILE static usartout = {0};

void serial_configure();
int usart_putchar (char c, FILE *stream);

void serial_configure() {

  SOFT_TX_PORT |= (1<<SOFT_TX_BIT);
  SOFT_TX_DDR |= (1<<SOFT_TX_BIT);
  fdev_setup_stream (&usartout, usart_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &usartout;
  
}

int usart_putchar (char c, FILE *stream) {

  uint8_t  bit_mask;

  // start bit
  SOFT_TX_PORT &= ~(1<<SOFT_TX_BIT);
  _delay_us(MICROSECONDS_PER_BIT);

  // data bits
  for (bit_mask=0x01; bit_mask; bit_mask<<=1) {
    if (c & bit_mask) {
      SOFT_TX_PORT |= (1<<SOFT_TX_BIT);
    }
    else {
      SOFT_TX_PORT &= ~(1<<SOFT_TX_BIT);
    }
    _delay_us(MICROSECONDS_PER_BIT);
  }

  // stop bit(s)
  SOFT_TX_PORT |= (1<<SOFT_TX_BIT);
  _delay_us(MICROSECONDS_PER_BIT * STOP_BITS);

  return c;

}