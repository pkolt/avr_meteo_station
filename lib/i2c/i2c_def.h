#ifndef I2C_DEF_H
#define I2C_DEF_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    I2C_MODE_WRITE = 0,
    I2C_MODE_READ = 1,
} i2c_mode_t;

#endif // I2C_DEF_H