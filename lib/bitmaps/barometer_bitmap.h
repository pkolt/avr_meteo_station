#ifndef BAROMETER_BITMAP_H
#define BAROMETER_BITMAP_H
#include <stdint.h>
#include <avr/pgmspace.h>

#define BAROMETER_BITMAP_WIDTH 24
#define BAROMETER_BITMAP_HEIGHT 24

const uint8_t PROGMEM barometer_bitmap[] = { 0x0, 0xfe, 0x0, 0xc0, 0xff, 0x3, 0xe0, 0x0, 0x7, 0x38, 0x18, 0x1c, 0x18, 0x18, 0x18, 0x4c, 0x18, 0x30, 0xe6, 0x0, 0x63, 0xc6, 0x80, 0x63, 0x3, 0xc0, 0xc1, 0x3, 0xe0, 0xc0, 0x3, 0x70, 0xc0, 0x7b, 0x38, 0xdc, 0x7b, 0x38, 0xdc, 0x3, 0x0, 0xc0, 0x3, 0x0, 0xc0, 0x83, 0xff, 0xc0, 0xe6, 0xff, 0x67, 0xfe, 0xff, 0x7f, 0xfc, 0xff, 0x3f, 0xf8, 0xff, 0x1f, 0xf8, 0xff, 0x1f, 0xe0, 0xff, 0x7, 0xc0, 0xff, 0x3, 0x0, 0x7f, 0x0 };

#endif // BAROMETER_BITMAP_H