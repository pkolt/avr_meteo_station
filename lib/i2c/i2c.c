#include <stdint.h>
#include <stdbool.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include "i2c_def.h"

#define I2C_FREQ 400000   // 2C bus frequency (in Hz)
#define I2C_PRESCALER_VALUE 1 // Prescaler value (TWPS0/TWPS1)

#define I2C_NACK _BV(TWINT) | _BV(TWEN) // Interrupt flag + Enable bit
#define I2C_ACK _BV(TWINT) | _BV(TWEN) | _BV(TWEA) // Interrupt flag + Enable bit + Enable Acknowledge bit

static bool i2c_ready = false;
static uint8_t i2c_error = 0; // See: Table 22-2. Status codes for Master Transmitter Mode

void i2c_init(void) {
    // Calculate the TWBR value (bitrate register) for a given I2C frequency
    TWBR = ((F_CPU / I2C_FREQ) - 16) / (2 * I2C_PRESCALER_VALUE);
    i2c_ready = true;
};

static void i2c_wait(void) {
    // Wait clear TWINT flag (from 1 to 0)
    loop_until_bit_is_set(TWCR, TWINT);
};

bool i2c_start(uint8_t address, i2c_mode_t mode) {
    if (!i2c_ready) {
        return i2c_ready;
    }
    bool is_ok;
    // Start condition
    // Interrupt flag + Start bit + Enable bit
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    i2c_wait();
    is_ok = TW_STATUS == TW_START || TW_STATUS == TW_REP_START;
    if (!is_ok) {
        i2c_error = TW_STATUS;
        return is_ok;
    }
    // Send Address
    TWDR = (address << 1) | mode; // SLA + (R/W)
    TWCR = I2C_NACK;
    i2c_wait();
    is_ok = (mode == I2C_MODE_WRITE && TW_STATUS == TW_MT_SLA_ACK) || (mode == I2C_MODE_READ && TW_STATUS == TW_MR_SLA_ACK);
    if (!is_ok) {
        i2c_error = TW_STATUS;
    }
    return is_ok;
};

bool i2c_write_byte(uint8_t data) {
    if (!i2c_ready) {
        return i2c_ready;
    }
    TWDR = data;
    TWCR = I2C_NACK;
    i2c_wait();
    bool is_ok = TW_STATUS == TW_MT_DATA_ACK;
    if (!is_ok) {
        i2c_error = TW_STATUS;
    }
    return is_ok;
};

bool i2c_read_byte_ACK(uint8_t* byte) {
    if (!i2c_ready) {
        return i2c_ready;
    }
    TWCR = I2C_ACK;
    i2c_wait();
    bool is_ok = TW_STATUS == TW_MR_DATA_ACK;
    if (is_ok) {
        *byte = TWDR;
    }
    else {
        i2c_error = TW_STATUS;
    }
    return is_ok;
}

bool i2c_read_byte_NACK(uint8_t* byte) {
    if (!i2c_ready) {
        return i2c_ready;
    }
    TWCR = I2C_NACK;
    i2c_wait();
    bool is_ok = TW_STATUS == TW_MR_DATA_NACK;
    if (is_ok) {
        *byte = TWDR;
    }
    else {
        i2c_error = TW_STATUS;
    }
    return is_ok;
}

void i2c_stop(void) {
    if (!i2c_ready) {
        return;
    }
    // Stop condition: Interrupt flag + Stop bit + Enable bit
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
};

uint8_t i2c_get_error() {
    return i2c_error;
}
