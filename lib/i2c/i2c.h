#ifndef I2C_H
#define I2C_H

#include <stdbool.h>
#include <stdint.h>
#include "i2c_def.h"

void i2c_init(void);
bool i2c_start(uint8_t address, i2c_mode_t mode);
bool i2c_write_byte(uint8_t byte);
bool i2c_read_byte_ACK(uint8_t* byte);
bool i2c_read_byte_NACK(uint8_t* byte);
void i2c_stop(void);
uint8_t i2c_get_error();

#endif // I2C_H