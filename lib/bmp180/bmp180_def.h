/**
 * C Library for BMP180
 * Author: Pavel Koltyshev
 * (c) 2024
*/

#ifndef BMP180_DEF_H
#define BMP180_DEF_H

#include <stdbool.h>
#include <stdint.h>

#define BMP180_EPROM_AC1 0xAA
#define BMP180_EPROM_AC2 0xAC
#define BMP180_EPROM_AC3 0xAE
#define BMP180_EPROM_AC4 0xB0
#define BMP180_EPROM_AC5 0xB2
#define BMP180_EPROM_AC6 0xB4
#define BMP180_EPROM_B1 0xB6
#define BMP180_EPROM_B2 0xB8
#define BMP180_EPROM_MB 0xBA
#define BMP180_EPROM_MC 0xBC
#define BMP180_EPROM_MD 0xBE

#define BMP180_START_MEASURE_TEMPERATURE 0x2E // Start measure temperature
#define BMP180_START_MEASURE_PRESSURE 0x34 // Start measure pressure
#define BMP180_START_SOFT_RESET 0xB6

#define BMP180_DELAY_MS_TEMPERATURE 4.5
#define BMP180_DELAY_MS_PRESSURE_0 4.5
#define BMP180_DELAY_MS_PRESSURE_1 7.5
#define BMP180_DELAY_MS_PRESSURE_2 13.5
#define BMP180_DELAY_MS_PRESSURE_3 25.5

#define BMP180_REGISTER_OUT_XLSB 0xF8
#define BMP180_REGISTER_OUT_LSB 0xF7
#define BMP180_REGISTER_OUT_MSB 0xF6
#define BMP180_REGISTER_CTR_MEAS 0xF4
#define BMP180_REGISTER_SOFT_RESET 0xE0
#define BMP180_REGISTER_CHIP_ID 0xD0

typedef enum {
    BMP180_ULTRA_LOW_POWER_MODE = 0,
    BMP180_STANDARD_MODE = 1,
    BMP180_HIGH_RESOLUTION_MODE = 2,
    BMP180_ULTRA_HIGH_RESOLUTION_MODE = 3,
} bmp180_mode_t;

typedef struct {
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;
} bmp180_calibration_data_t;

typedef struct {
    const uint8_t i2c_address;
    bmp180_mode_t mode;
    bmp180_calibration_data_t data;
    int32_t B5; // For calculation pressure
} bmp180_t;

#endif // BMP180_DEF_H