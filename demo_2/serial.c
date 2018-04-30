#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "serial.h"
#include "common.h"

char serial_read() {
    PORTD &= ~(1 << PD3);
    char received_byte;
    while ((UCSR0A & ( 1 << RXC0 )) == 0) {}
    received_byte = UDR0;
    return received_byte;
}

void serial_write(char c) {
    PORTD &= ~(1 << PD3);
    while ((UCSR0A & ( 1 << UDRE0 )) == 0) {} 
    UDR0 = c;   
}

void serial_write_uint16(uint16_t n) {
    PORTD &= ~(1 << PD3);
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
    PORTD &= ~(1 << PD3);
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

void serial_write_char_to_peripheral(char c) {
    PORTD |= 1 << PD3;
    _delay_us(1);
    while ((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = c;
}

void send_red(uint8_t c) {
    serial_write_char_to_peripheral((c & ~(3)) + 0);
}

void send_green(uint8_t c) {
    serial_write_char_to_peripheral((c & ~(3)) + 1);
}

void send_blue(uint8_t c) {
    serial_write_char_to_peripheral((c & ~(3)) + 2);
}