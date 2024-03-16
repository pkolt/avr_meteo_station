/**
 * C Library for BMP180
 * Author: Pavel Koltyshev
 * (c) 2024
*/

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <util/delay.h>
#include "i2c.h"
#include "bmp180_def.h"

static bool i2c_read_uin16(uint8_t i2c_address, uint8_t reg, uint16_t* result) {
    bool is_ok;
    uint8_t msb = 0;
    uint8_t lsb = 0;
    is_ok = i2c_start(i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(reg);
    is_ok = is_ok && i2c_start(i2c_address, I2C_MODE_READ);
    is_ok = is_ok && i2c_read_byte_ACK(&msb);
    is_ok = is_ok && i2c_read_byte_NACK(&lsb);
    if (is_ok) {
        *result = (uint16_t)(msb << 8 | lsb);
    }
    i2c_stop();
    return is_ok;
}

static bool i2c_read_in16(uint8_t i2c_address, uint8_t reg, int16_t* result) {
    uint16_t value;
    bool is_ok = i2c_read_uin16(i2c_address, reg, &value);
    if (is_ok) {
        *result = (int16_t)value;
    }
    return is_ok;
}

bmp180_t bmp180_create(uint8_t i2c_address) {
    const bmp180_t bmp180 = {
        .i2c_address = i2c_address,
        .mode = BMP180_STANDARD_MODE,
        .data = {},
    };
    return bmp180;
}

bool bmp180_init(bmp180_t* bmp180) {
    bool is_ok = true;
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_AC1, &bmp180->data.AC1);
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_AC2, &bmp180->data.AC2);
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_AC3, &bmp180->data.AC3);
    is_ok = is_ok && i2c_read_uin16(bmp180->i2c_address, BMP180_EPROM_AC4, &bmp180->data.AC4);
    is_ok = is_ok && i2c_read_uin16(bmp180->i2c_address, BMP180_EPROM_AC5, &bmp180->data.AC5);
    is_ok = is_ok && i2c_read_uin16(bmp180->i2c_address, BMP180_EPROM_AC6, &bmp180->data.AC6);
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_B1, &bmp180->data.B1);
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_B2, &bmp180->data.B2);
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_MB, &bmp180->data.MB);
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_MC, &bmp180->data.MC);
    is_ok = is_ok && i2c_read_in16(bmp180->i2c_address, BMP180_EPROM_MD, &bmp180->data.MD);
    return is_ok;
}

void bmp180_set_mode(bmp180_t* bmp180, bmp180_mode_t mode) {
    bmp180->mode = mode;
}

bool bmp180_get_temperature(bmp180_t *bmp180, int32_t *temp) {
    bool is_ok;
    is_ok = i2c_start(bmp180->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(BMP180_REGISTER_CTR_MEAS);
    is_ok = is_ok && i2c_write_byte(BMP180_START_MEASURE_TEMPERATURE);
    i2c_stop();

    if (!is_ok) {
        return is_ok;
    }

    // Waiting measurement
    _delay_ms(BMP180_DELAY_MS_TEMPERATURE);

    uint8_t msb = 0;
    uint8_t lsb = 0;
    is_ok = is_ok && i2c_start(bmp180->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(BMP180_REGISTER_OUT_MSB);
    is_ok = is_ok && i2c_start(bmp180->i2c_address, I2C_MODE_READ);
    is_ok = is_ok && i2c_read_byte_ACK(&msb);
    is_ok = is_ok && i2c_read_byte_NACK(&lsb);
    i2c_stop();

    if (is_ok) {
        const bmp180_calibration_data_t *data = &bmp180->data;
        int32_t ut = (msb << 8) | lsb;
        int32_t x1 = (ut - data->AC6) * data->AC5 / pow(2, 15);
        int32_t x2 = data->MC * pow(2, 11) / (x1 + data->MD);
        bmp180->B5 = x1 + x2;
        int32_t t = (bmp180->B5 + 8) / pow(2, 4); // Temperature in 0.1 C
        *temp = t;
    }
    return is_ok;
}

bool bmp180_get_pressure(const bmp180_t *bmp180, int32_t *press) {
    bool is_ok;
    is_ok = i2c_start(bmp180->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(BMP180_REGISTER_CTR_MEAS);
    is_ok = is_ok && i2c_write_byte(BMP180_START_MEASURE_PRESSURE | (bmp180->mode << 6));
    i2c_stop();

    if (!is_ok) {
        return is_ok;
    }

    // Waiting measurement
    switch (bmp180->mode) {
        case BMP180_STANDARD_MODE:
            _delay_ms(BMP180_DELAY_MS_PRESSURE_1);
            break;
        case BMP180_HIGH_RESOLUTION_MODE:
            _delay_ms(BMP180_DELAY_MS_PRESSURE_2);
            break;
        case BMP180_ULTRA_HIGH_RESOLUTION_MODE:
            _delay_ms(BMP180_DELAY_MS_PRESSURE_3);
            break;
        default:
            _delay_ms(BMP180_DELAY_MS_PRESSURE_0);
            break;
    }

    uint8_t msb = 0;
    uint8_t lsb = 0;
    uint8_t xlsb = 0;
    
    is_ok = is_ok && i2c_start(bmp180->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(BMP180_REGISTER_OUT_MSB);
    is_ok = is_ok && i2c_start(bmp180->i2c_address, I2C_MODE_READ);
    is_ok = is_ok && i2c_read_byte_ACK(&msb);
    is_ok = is_ok && i2c_read_byte_ACK(&lsb);
    is_ok = is_ok && i2c_read_byte_NACK(&xlsb);
    i2c_stop();

    if (is_ok) {
        const bmp180_calibration_data_t *data = &bmp180->data;
        int32_t up = ((uint32_t)msb << 16 | (uint32_t)lsb << 8 | xlsb) >> (8 - bmp180->mode);
        int32_t b6 = bmp180->B5 - 4000;
        int32_t x1 = (data->B2 * (b6 * b6 / pow(2, 12))) / pow(2, 11);
        int32_t x2 = data->AC2 * b6 / pow(2, 11);
        int32_t x3 = x1 + x2;
        int32_t b3 = ((((int32_t)data->AC1 * 4 + x3) << bmp180->mode) + 2) / 4;
        x1 = data->AC3 * b6 / pow(2, 13);
        x2 = (data->B1 * (b6 * b6 / pow(2, 12))) / pow(2, 16);
        x3 = ((x1 + x2) + 2) / pow(2, 2);
        uint32_t b4 = data->AC4 * (uint32_t)(x3 + 32768) / pow(2, 15);
        uint32_t b7 = ((uint32_t)up - b3) * (50000 >> bmp180->mode);
        int32_t p = 0;
        if (b7 < 0x80000000) {
            p = (b7 * 2) / b4;
        } else {
            p = (b7 / b4) * 2;
        }
        x1 = (p / pow(2, 8)) * (p / pow(2, 8));
        x1 = (x1 * 3038) / pow(2, 16);
        x2 = (-7357 * p) / pow(2, 16);
        p = p + (x1 + x2 + 3791) / pow(2, 4); // Pressure in Pa
        *press = p;
    }
    return is_ok;
}

uint16_t bmp180_pressure_to_altitude(int32_t *pressure) {
    return (uint16_t)(44330 * (1 - pow((*pressure / 101325.0), 1 / 5.255)));
}

uint16_t bmp180_pressure_to_mm(int32_t *pressure) {
    return (uint16_t)(*pressure / 133.322);
}

bool bmp180_reset(const bmp180_t *bmp180) {
    bool is_ok;
    is_ok = i2c_start(bmp180->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(BMP180_REGISTER_SOFT_RESET);
    is_ok = is_ok && i2c_write_byte(BMP180_START_SOFT_RESET);
    i2c_stop();
    return is_ok;
}

bool bmp180_get_id(const bmp180_t *bmp180, uint8_t *chip_id) {
    bool is_ok;
    is_ok = i2c_start(bmp180->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(BMP180_REGISTER_CHIP_ID);
    is_ok = is_ok && i2c_start(bmp180->i2c_address, I2C_MODE_READ);
    is_ok = is_ok && i2c_read_byte_NACK(chip_id);
    i2c_stop();
    return is_ok;
}
