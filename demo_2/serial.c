#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "common.h"

char serial_read() {
    char received_byte;
    while ((UCSR0A & ( 1 << RXC0 )) == 0) {}
    received_byte = UDR0;
    return received_byte;
}

void serial_write(char c) {
    while ((UCSR0A & ( 1 << UDRE0 )) == 0) {} 
    UDR0 = c;   
}

void serial_write_uint16(uint16_t n) {
    int i = 0;
    char s[20];
    serial_write(' ');
    if (n == 0) {
        serial_write('0');
    } else {
        for (; n > 0; n /= 10, i++){
            s[i] = '0' + n % 10;            
        }
        for (; i > 0; i--){
            serial_write(s[i - 1]);
        }
    }
    
    serial_write(' ');
}

void serial_write_string(char* s) {
    for (; *s; s++)
        serial_write(*s);
}

void serial_init() {
    UCSR0B |= (1<<RXEN0)  | (1<<TXEN0);
    UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
    UBRR0 = BAUD_PRESCALE;
    DDRD |= (1 << PD3);
    PORTD &= ~(1 << PD3);
    //PORTD |= 1 << PD3;
    serial_write_string("----serial initialized----");
}