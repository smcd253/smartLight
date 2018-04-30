#ifndef SERIAL_H
#define SERIAL_H

char serial_read();

void serial_write(char c);

void serial_write_uint16(uint16_t n);

void serial_write_string(char* s);

void serial_init();

void serial_write_char_to_peripheral(char c);

void send_red(uint8_t c);

void send_green(uint8_t c);

void send_blue(uint8_t c);

#endif