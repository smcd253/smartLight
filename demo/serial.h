#ifndef SERIAL_H
#define SERIAL_H


char serial_read();

void serial_write(char c);

void serial_write_uint16(uint16_t n);

void serial_write_string(char* s);

void serial_init();

#endif