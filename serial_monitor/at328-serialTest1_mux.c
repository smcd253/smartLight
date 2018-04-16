#include <avr/io.h>
#include <util/delay.h>

/*
serial_init - Initialize the USART port
*/
void serial_init ( unsigned short ubrr ) {
UBRR0 = ubrr; // Set baud rate
UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
UCSR0B |= (1 << RXEN0 ); // Turn on receiver
UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity ,
// one stop bit , 8 data bits
}

/*
selects serial input/output on SN74S253N Multiplexers
PD2 = 0
PD3 = 0
*/
void select_serial(){
    PORTD |= (0 << PD2) | (0 << PD3);
}

/*
selects serial input/output on SN74S253N Multiplexers
PD2 = 0
PD3 = 1
*/
void select_periph(){
    PORTD |= (0 << PD2) | (1 << PD3);
}

/*
serial_out - Output a byte to the USART0 port
*/
void serial_out ( char ch ) {
while (( UCSR0A & (1 << UDRE0 )) == 0);
    UDR0 = ch ;
}

/*
serial_in - Read a byte from the USART0 and return it
*/
char serial_in () {
    while ( !( UCSR0A & (1 << RXC0 )) );
    return UDR0;
}

int main(){
    serial_init(9600);
    select_serial();
    while(1){
        serial_out('a');
        _delay_ms(500);
    }
    return 0;
}