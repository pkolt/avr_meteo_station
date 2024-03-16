/**
 * C Library for BMP180
 * Author: Pavel Koltyshev
 * (c) 2024
*/

#ifndef BMP180_H
#define BMP180_H

#include <stdint.h>
#include "bmp180_def.h"

bmp180_t bmp180_create(uint8_t i2c_address);
bool bmp180_init(bmp180_t *bmp180);
bool bmp180_get_temperature(bmp180_t *bmp180, int32_t *temp);
bool bmp180_get_pressure(const bmp180_t *bmp180, int32_t *press);
void bmp180_set_mode(bmp180_t *bmp180, bmp180_mode_t mode);
bool bmp180_reset(const bmp180_t *bmp180);
bool bmp180_get_id(const bmp180_t *bmp180, uint8_t *chip_id);
uint16_t bmp180_pressure_to_altitude(int32_t *pressure);
uint16_t bmp180_pressure_to_mm(int32_t *pressure);

#endif // BMP180_H