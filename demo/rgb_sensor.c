#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#include "rgb_sensor.h"


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
        for (; n > 0; n /= 10, i++)
            s[i] = '0' + n % 10;
        for (; i > 0; i--)
            serial_write(s[i - 1]);
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

void i2c_init() {
    TWSR = 0;
    TWBR = BDIV;
}

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn) {
    uint8_t status, send_stop, wrote, start_stat;

    status = 0;
    wrote = 0;
    send_stop = 0;

    if (an > 0 || wn > 0) {
        wrote = 1;
        send_stop = 1;

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);  // Send start condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x08) {                 // Check that START was sent OK
            //serial_write_string(" CK1");
            return(status);
        }

        TWDR = device_addr & 0xfe;          // Load device address and R/W = 0;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x18) {               // Check that SLA+W was sent OK
            if (status == 0x20) {             // Check for NAK
                //serial_write_string(" CK2");
                goto nakstop;               // Send STOP condition
            }
            //serial_write_string(" CK3");
            return(status);                 // Otherwise just return the status
        }

        // Write "an" data bytes to the slave device
        while (an-- > 0) {
            TWDR = *ap++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }

        // Write "wn" data bytes to the slave device
        while (wn-- > 0) {
            TWDR = *wp++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30) {         // Check for NAK
                    //serial_write_string(" CK4");
                    goto nakstop;           // Send STOP condition
                }
                //serial_write_string(" CK5");
                return(status);             // Otherwise just return the status
            }
        }

        status = 0;                         // Set status value to successful
    }

    if (rn > 0) {
        send_stop = 1;

        // Set the status value to check for depending on whether this is a
        // START or repeated START
        start_stat = (wrote) ? 0x10 : 0x08;

        // Put TWI into Master Receive mode by sending a START, which could
        // be a repeated START condition if we just finished writing.
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
                                            // Send start (or repeated start) condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != start_stat)           // Check that START or repeated START sent OK
            return(status);

        TWDR = device_addr  | 0x01;         // Load device address and R/W = 1;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Send address+r
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x40) {               // Check that SLA+R was sent OK
            if (status == 0x48)             // Check for NAK
                goto nakstop;
            return(status);
        }

        // Read all but the last of n bytes from the slave device in this loop
        rn--;
        while (rn-- > 0) {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Read byte and send ACK
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x50)             // Check that data received OK
                return(status);
            *rp++ = TWDR;                   // Read the data
        }

        // Read the last byte
        TWCR = (1 << TWINT) | (1 << TWEN);  // Read last byte with NOT ACK sent
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x58)                 // Check that data received OK
            return(status);
        *rp++ = TWDR;                       // Read the data

        status = 0;                         // Set status value to successful
    }
    
nakstop:                                    // Come here to send STOP after a NAK
    if (send_stop)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // Send STOP condition

    return(status);
}

uint8_t read8(uint8_t reg) {
    uint8_t data;
    i2c_io(ISL_I2C_ADDR, NULL, 0, &reg, 1, NULL, 0);
    i2c_io(ISL_I2C_ADDR, NULL, 0, NULL, 0, &data, 1);
    return data;
}

uint16_t read16(uint8_t reg) {
    uint8_t array[2];
    array[0] = 2;
    array[1] = 2;
    if (i2c_io(ISL_I2C_ADDR, NULL, 0, &reg, 1, NULL, 0)) {
        serial_write_string(" Read16_w failed");
        return 0;
    }
    if (i2c_io(ISL_I2C_ADDR, NULL, 0, NULL, 0, array, 2)) {
        serial_write_string(" Read16_r failed");
        return 0;
    }
    uint16_t ret = array[0];
    ret = ret << 8;
    ret = ret + array[1];
    return ret;
}

void write8(uint8_t reg, uint8_t data) {
    uint8_t array[2];
    array[0] = reg;
    array[1] = data;
    if (i2c_io(ISL_I2C_ADDR, NULL, 0, array, 2, NULL, 0)) {
        serial_write_string(" Write8 failed");
    }
}

bool reset() {
    uint8_t data = 0x00;
    // Reset registers
    write8(DEVICE_ID, 0x46);
    // Check reset
    data = read8(CONFIG_1);
    data |= read8(CONFIG_2);
    data |= read8(CONFIG_3);
    data |= read8(STATUS);
    return data == 0;
}

bool config(uint8_t config1, uint8_t config2, uint8_t config3) {
    uint8_t data = 0x00;
  
    // Set 1st configuration register
    write8(CONFIG_1, config1);
    // Set 2nd configuration register
    write8(CONFIG_2, config2);
    // Set 3rd configuration register
    write8(CONFIG_3, config3);
  
    // Check if configurations were set correctly
    data = read8(CONFIG_1);
    if (data != config1)
        return false;
    data = read8(CONFIG_2);
    if (data != config2)
        return false;
    data = read8(CONFIG_3);
    if (data != config3)
        return false;
    return true;
}

bool rgb_init() {
    uint8_t data = 0x00;
    bool ret = true;
    data = read8(DEVICE_ID);
    serial_write_string("DEVICE_ID:");
    serial_write_uint16(data);
    if (data != 0x7D) {
        //ret &= false;
        serial_write_string(" data!=0x7D");
    }
    serial_write_string(" CheckedID");
    ret &= reset();
    serial_write_string(" Reset");
    // Set to RGB mode, 10k lux, and high IR compensation
    ret &= config(CFG1_MODE_RGB | CFG1_10KLUX, CFG2_IR_ADJUST_HIGH, CFG_DEFAULT);
    serial_write_string("Config");
    return ret;
}

uint16_t readRed() {
  return read16(RED_L);
}

uint16_t readGreen() {
  return read16(GREEN_L);
}

uint16_t readBlue() {
  return read16(BLUE_L);
}

// void rgb_feedback(){
//     uint8_t data;
//     char redSize;
//     int redSize_int;
//     int a = 0;
//     // rgb feedback
//     data = read8(DEVICE_ID);
//     redBuf[a] = 1;
//     a += 1;
//     redSize = sizeof(redBuf)/sizeof(redBuf[0]);
//     redSize_int = (uint8_t)redSize;
//     serial_write('S');
//     serial_write(redSize_int);
// }