/**
 * C Library for SSD1306 OLED Display
 * Author: Pavel Koltyshev
 * (c) 2023-2024
*/

#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "ssd1306_def.h"

ssd1306_config_t ssd1306_create_config(uint8_t i2c_address);
ssd1306_t ssd1306_create(const ssd1306_config_t* config);
bool ssd1306_init(const ssd1306_t* ssd1306, const ssd1306_config_t* config);
bool ssd1306_set_contrast(const ssd1306_t* ssd1306, uint8_t contrast);
bool ssd1306_set_inverse(const ssd1306_t* ssd1306, bool value);
bool ssd1306_display_on(const ssd1306_t* ssd1306);
bool ssd1306_display_off(const ssd1306_t* ssd1306);
bool ssd1306_set_start_line(const ssd1306_t* ssd1306, uint8_t line);
bool ssd1306_set_offset(const ssd1306_t* ssd1306, uint8_t value);
bool ssd1306_entire_display_on(const ssd1306_t* ssd1306);
bool ssd1306_entire_display_off(const ssd1306_t* ssd1306);
bool ssd1306_set_display_clock(const ssd1306_t* ssd1306, uint8_t divide_ratio, uint8_t oscillator_frequency);
bool ssd1306_set_multiplex_ratio(const ssd1306_t* ssd1306, uint8_t value);
bool ssd1306_set_memory_addressing_mode(const ssd1306_t* ssd1306, ssd1306_memory_addressing_mode_t mode);
bool ssd1306_set_pre_charge_period(const ssd1306_t* ssd1306, uint8_t phase_1, uint8_t phase_2);
bool ssd1306_set_vcomh_deselect_level(const ssd1306_t* ssd1306, ssd1306_vcomh_deselect_level_t level);
bool ssd1306_set_com_pins_hardware_config(const ssd1306_t* ssd1306, bool com_alt_pin_config, bool com_disable_left_right_remap);
bool ssd1306_set_charge_pump(const ssd1306_t* ssd1306, bool enabled);
bool ssd1306_set_segment_re_map(const ssd1306_t* ssd1306, bool inverse);
bool ssd1306_set_com_output_scan_direction(const ssd1306_t* ssd1306, bool remapped);
bool ssd1306_set_zoom(const ssd1306_t* ssd1306, bool enabled);
bool ssd1306_set_fade_out_and_blinking(const ssd1306_t* ssd1306, ssd1306_fade_out_blinking_mode_t mode, uint8_t time_interval);
bool ssd1306_clear_display(const ssd1306_t* ssd1306);
bool ssd1306_draw_bitmap(const ssd1306_t* ssd1306, uint8_t start_page, uint8_t start_column, uint8_t width, uint8_t height, ssd1306_bitmap_t bitmap);
void ssd1306_set_font(ssd1306_t* ssd1306, const ssd1306_font_t* font);
bool ssd1306_print(const ssd1306_t* ssd1306, const char* text, uint8_t start_page, uint8_t start_column);

#endif // SSD1306_H
