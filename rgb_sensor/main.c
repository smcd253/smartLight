/**
 * A PWM example for the ATmega328P using the 8-Bit Fast PWM mode.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#define NULL 0
#define FOSC 7372800
//#define BDIV (FOSC/500000-16)/2+1
#define BDIV (FOSC/500000-16)/2+1
#define BAUD_PRESCALE 47

#define ISL_I2C_ADDR 0x44

// ISL29125 Registers
#define DEVICE_ID 0x00
#define CONFIG_1 0x01
#define CONFIG_2 0x02
#define CONFIG_3 0x03
#define THRESHOLD_LL 0x04
#define THRESHOLD_LH 0x05
#define THRESHOLD_HL 0x06
#define THRESHOLD_HH 0x07
#define STATUS 0x08 
#define GREEN_L 0x09 
#define GREEN_H 0x0A
#define RED_L 0x0B
#define RED_H 0x0C
#define BLUE_L 0x0D
#define BLUE_H 0x0E

// Configuration Settings
#define CFG_DEFAULT 0x00

// CONFIG1
// Pick a mode, determines what color[s] the sensor samples, if any
#define CFG1_MODE_POWERDOWN 0x00
#define CFG1_MODE_G 0x01
#define CFG1_MODE_R 0x02
#define CFG1_MODE_B 0x03
#define CFG1_MODE_STANDBY 0x04
#define CFG1_MODE_RGB 0x05
#define CFG1_MODE_RG 0x06
#define CFG1_MODE_GB 0x07

// Light intensity range
// In a dark environment 375Lux is best, otherwise 10KLux is likely the best option
#define CFG1_375LUX 0x00
#define CFG1_10KLUX 0x08

// Change this to 12 bit if you want less accuracy, but faster sensor reads
// At default 16 bit, each sensor sample for a given color is about ~100ms
#define CFG1_16BIT 0x00
#define CFG1_12BIT 0x10

// Unless you want the interrupt pin to be an input that triggers sensor sampling, leave this on normal
#define CFG1_ADC_SYNC_NORMAL 0x00
#define CFG1_ADC_SYNC_TO_INT 0x20

// CONFIG2
// Selects upper or lower range of IR filtering
#define CFG2_IR_OFFSET_OFF 0x00
#define CFG2_IR_OFFSET_ON 0x80

// Sets amount of IR filtering, can use these presets or any value between 0x00 and 0x3F
// Consult datasheet for detailed IR filtering calibration
#define CFG2_IR_ADJUST_LOW 0x00
#define CFG2_IR_ADJUST_MID 0x20
#define CFG2_IR_ADJUST_HIGH 0x3F

// CONFIG3
// No interrupts, or interrupts based on a selected color
#define CFG3_NO_INT 0x00
#define CFG3_G_INT 0x01
#define CFG3_R_INT 0x02
#define CFG3_B_INT 0x03

// How many times a sensor sample must hit a threshold before triggering an interrupt
// More consecutive samples means more times between interrupts, but less triggers from short transients
#define CFG3_INT_PRST1 0x00
#define CFG3_INT_PRST2 0x04
#define CFG3_INT_PRST4 0x08
#define CFG3_INT_PRST8 0x0C

// If you would rather have interrupts trigger when a sensor sampling is complete, enable this
// If this is disabled, interrupts are based on comparing sensor data to threshold settings
#define CFG3_RGB_CONV_TO_INT_DISABLE 0x00
#define CFG3_RGB_CONV_TO_INT_ENABLE 0x10

// STATUS FLAG MASKS
#define FLAG_INT 0x01
#define FLAG_CONV_DONE 0x02
#define FLAG_BROWNOUT 0x04
#define FLAG_CONV_G 0x10
#define FLAG_CONV_R 0x20
#define FLAG_CONV_B 0x30

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
    serial_write_string(" DECIVE_ID:");
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
    serial_write_string(" Configed");
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

int main (void) {
    serial_init();
    //serial_write('s');
    /*i2c_init();
    serial_write('t');
    //rgb_init();
    serial_write('u');*/
	for (;;) {
        //serial_write('R');
        //serial_write_uint16(readRed());
        /*serial_write('G');
        serial_write_uint16(readGreen());
        serial_write('B');
        serial_write_uint16(readBlue());*/
        //readRed();
        //_delay_ms(10);

        serial_write_string("HAL, open the pod ");
        _delay_ms(2000);
	}
}