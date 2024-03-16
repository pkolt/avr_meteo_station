/**
 * C Library for SSD1306 OLED Display
 * Author: Pavel Koltyshev
 * (c) 2023-2024
*/

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>
#include "i2c.h"
#include "bitwise.h"
#include "ssd1306_def.h"

static uint8_t div_ceil(uint8_t a, uint8_t b) {
    return (a / b + (a % b > 0 ? 1 : 0));
}

static bool ssd1306_send_command(const ssd1306_t* ssd1306, uint8_t command) {
    bool is_ok;
    is_ok = i2c_start(ssd1306->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(SSD1306_SEND_COMMAND);
    is_ok = is_ok && i2c_write_byte(command);
    i2c_stop();
    return is_ok;
}

static bool ssd1306_send_command_value(const ssd1306_t* ssd1306, uint8_t command, uint8_t value) {
    bool is_ok;
    is_ok = i2c_start(ssd1306->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok & i2c_write_byte(SSD1306_SEND_COMMAND);
    is_ok = is_ok & i2c_write_byte(command);
    is_ok = is_ok & i2c_write_byte(value);
    i2c_stop();
    return is_ok;
}

/**
 * Set Contrast Control
 * @param contrast (1-255) 0x7F = 127 (RESET)
*/
bool ssd1306_set_contrast(const ssd1306_t* ssd1306, uint8_t contrast) {
    return ssd1306_send_command_value(ssd1306, SSD1306_CONTRAST_COMMAND, contrast);
}

/**
 * Inverse Display
 * @param ssd1306
 * @param enabled (RESET = false)
*/
bool ssd1306_set_inverse(const ssd1306_t* ssd1306, bool enabled) {
    return ssd1306_send_command(ssd1306, enabled ? SSD1306_DISPLAY_INVERSE_COMMAND : SSD1306_DISPLAY_NORMAL_COMMAND);
}

/**
 * Display ON
*/
bool ssd1306_display_on(const ssd1306_t* ssd1306) {
    return ssd1306_send_command(ssd1306, SSD1306_DISPLAY_ON_COMMAND);
}

/**
 * Display OFF (sleep mode) (RESET)
*/
bool ssd1306_display_off(const ssd1306_t* ssd1306) {
    return ssd1306_send_command(ssd1306, SSD1306_DISPLAY_OFF_COMMAND);
}

//  Set display RAM display start line register from. Line values: 0-63
bool ssd1306_set_start_line(const ssd1306_t* ssd1306, uint8_t line) {
    const uint8_t command = line >= 0 && line <= 63 ? (SSD1306_DISPLAY_START_LINE_COMMAND | line) : SSD1306_DISPLAY_START_LINE_COMMAND;
    return ssd1306_send_command(ssd1306, command);
}

/**
 * Set Display Offset
 * Set vertical shift by COM from 0-63
 * The value is reset to 0 after RESET.
 *
*/
static bool ssd1306_set_offset(const ssd1306_t* ssd1306, uint8_t value) {
    bool is_ok = false;
    if (value >= SSD1306_DISPLAY_OFFSET_MIN && value <= SSD1306_DISPLAY_OFFSET_MAX) {
        is_ok = ssd1306_send_command_value(ssd1306, SSD1306_DISPLAY_OFFSET_COMMAND, value);
    }
    return is_ok;
}

/**
 * Entire Display On (RESET)
 * Output follows RAM content
*/
bool ssd1306_entire_display_on(const ssd1306_t* ssd1306) {
    return ssd1306_send_command(ssd1306, SSD1306_ENTIRE_DISPLAY_ON_COMMAND);
}

/**
 * Entire Display Off
 * Output ignores RAM content
 *
*/
bool ssd1306_entire_display_off(const ssd1306_t* ssd1306) {
    return ssd1306_send_command(ssd1306, SSD1306_ENTIRE_DISPLAY_OFF_COMMAND);
}

/**
 * Set Display Clock Divide Ratio/Oscillator Frequency
 * @param ssd1306
 * @param divide_ratio (1-15) Define the divide ratio (D) of the display clocks (DCLK)
 * @param oscillator_frequency (0-15) Set the Oscillator Frequency.
*/
bool ssd1306_set_display_clock(const ssd1306_t* ssd1306, uint8_t divide_ratio, uint8_t oscillator_frequency) {
    bool is_ok = false;
    if (divide_ratio >= SSD1306_DISPLAY_CLOCK_DIVIDE_RATIO_MIN &&
        divide_ratio <= SSD1306_DISPLAY_CLOCK_DIVIDE_RATIO_MAX &&
        oscillator_frequency >= SSD1306_DISPLAY_CLOCK_OSCILLATOR_FREQUENCY_MIN &&
        oscillator_frequency <= SSD1306_DISPLAY_CLOCK_OSCILLATOR_FREQUENCY_MAX) {
        const uint8_t value = divide_ratio | (oscillator_frequency << 4);
        is_ok = ssd1306_send_command_value(ssd1306, SSD1306_DISPLAY_CLOCK_DIVIDE_COMMAND, value);
    }
    return is_ok;
}

/**
 * Set Multiplex Ratio
 * @param ssd1306
 * @param value (15-63) MUX ratio
*/
bool ssd1306_set_multiplex_ratio(const ssd1306_t* ssd1306, uint8_t value) {
    bool is_ok = false;
    if (value >= SSD1306_MUX_RATIO_MIN && value <= SSD1306_MUX_RATIO_MAX) {
        is_ok = ssd1306_send_command_value(ssd1306, SSD1306_MUX_RATIO_COMMAND, value);
    }
    return is_ok;
}

/**
 * Set Memory Addressing Mode
 * @param ssd1306
 * @param mode
*/
bool ssd1306_set_memory_addressing_mode(const ssd1306_t* ssd1306, ssd1306_memory_addressing_mode_t mode) {
    return ssd1306_send_command_value(ssd1306, SSD1306_MEMORY_ADDRESSING_MODE_COMMAND, mode);
}

/**
 * Set VCOMH Deselect 0 Level
 * @param ssd1306
 * @param level
*/
bool ssd1306_set_vcomh_deselect_level(const ssd1306_t* ssd1306, ssd1306_vcomh_deselect_level_t level) {
    bool is_ok = false;
    if (level == SSD1306_VCOMH_DESELECT_LEVEL0 || level == SSD1306_VCOMH_DESELECT_LEVEL1 || level == SSD1306_VCOMH_DESELECT_LEVEL2) {
        is_ok = ssd1306_send_command_value(ssd1306, SSD1306_VCOMH_DESELECT_LEVEL_COMMAND, level);
    }
    return is_ok;
}

/**
 * Set Pre-charge Period
 * @param ssd1306
 * @param phase_1 (1-15) period of up to 15 DCLK clocks 0 is invalid entry
 * @param phase_2 (1-15) period of up to 15 DCLK clocks 0 is invalid entry
*/
bool ssd1306_set_pre_charge_period(const ssd1306_t* ssd1306, uint8_t phase_1, uint8_t phase_2) {
    bool is_ok = false;
    if (phase_1 >= SSD1306_PRE_CHARGE_PERIOD_PHASE_MIN &&
        phase_1 <= SSD1306_PRE_CHARGE_PERIOD_PHASE_MAX &&
        phase_2 >= SSD1306_PRE_CHARGE_PERIOD_PHASE_MIN &&
        phase_2 <= SSD1306_PRE_CHARGE_PERIOD_PHASE_MAX) {
        is_ok = ssd1306_send_command_value(ssd1306, SSD1306_PRE_CHARGE_PERIOD_COMMAND, (phase_1 << 4) | phase_2);
    }
    return is_ok;
}

/**
 * Set COM Pins Hardware Configuration
 * @param ssd1306
 * @param com_alt_pin_config (default = true)
 * @param com_disable_left_right_remap (default = true)
*/
bool ssd1306_set_com_pins_hardware_config(const ssd1306_t* ssd1306, bool com_alt_pin_config, bool com_disable_left_right_remap) {
    const uint8_t value1 = com_alt_pin_config ? SSD1306_COM_PINS_HARDWARE_CONFIG_ALTERNATIVE_COM_PIN : SSD1306_COM_PINS_HARDWARE_CONFIG_SEQUENTIAL_COM_PIN;
    const uint8_t value2 = com_disable_left_right_remap ? SSD1306_COM_PINS_HARDWARE_CONFIG_DISABLE_REMAP : SSD1306_COM_PINS_HARDWARE_CONFIG_ENABLE_REMAP;
    return ssd1306_send_command_value(ssd1306, SSD1306_COM_PINS_HARDWARE_CONFIG_COMMAND, value1 | value2);
}

/**
 * Set Charge Pump
 * The Charge Pump must be enabled by the following command sequence:
 * 1. Charge Pump Setting (enable)
 * 2. Display ON
 * @param ssd1306
 * @param enabled (RESET = true)
*/
bool ssd1306_set_charge_pump(const ssd1306_t* ssd1306, bool enabled) {
    return ssd1306_send_command_value(ssd1306, SSD1306_CHARGE_PUMP_COMMAND, enabled ? SSD1306_CHARGE_PUMP_ENABLE : SSD1306_CHARGE_PUMP_DISABLE);
}

/**
 * Set Segment Re-map
 * @param ssd1306
 * @param inverse (RESET = false)
*/
bool ssd1306_set_segment_re_map(const ssd1306_t* ssd1306, bool inverse) {
    return ssd1306_send_command(ssd1306, inverse ? SSD1306_SEGMENT_RE_MAP_INVERSE_COMMAND : SSD1306_SEGMENT_RE_MAP_NORMAL_COMMAND);
}

/**
 * COM Output Scan Direction
 * Scan from COM0 to COM[N-1]
 * Scan from COM[N-1] to COM0. Where N is the Multiplex ratio.
 * @param ssd1306
 * @param remapped (RESET = false)
*/
bool ssd1306_set_com_output_scan_direction(const ssd1306_t* ssd1306, bool remapped) {
    return ssd1306_send_command(ssd1306, remapped ? SSD1306_COM_OUTPUT_SCAN_DIRECTION_REMAPPED_COMMAND : SSD1306_COM_OUTPUT_SCAN_DIRECTION_NORMAL_COMMAND);
}

/**
 * Zoom Mode
 * @param ssd1306
 * @param enabled (RESET = false)
*/
bool ssd1306_set_zoom(const ssd1306_t* ssd1306, bool enabled) {
    return ssd1306_send_command_value(ssd1306, SSD1306_ZOOM_IN_COMMAND, enabled ? SSD1306_ZOOM_IN_ENABLE : SSD1306_ZOOM_IN_DISABLE);
}

/**
 * Set Fade Out and Blinking
 * @param ssd1306
 * @param mode
 * 1. Disable Fade Out / Blinking Mode (RESET)
 * 2. Enable Fade Out mode.
 * Once Fade Mode is enabled, contrast decrease gradually to all pixels OFF.
 * Output follows RAM content when Fade mode is disabled.
 * 3. Enable Blinking mode.
 * Once Blinking Mode is enabled, contrast decrease gradually to all pixels OFF and than
 * contrast increase gradually to normal display.
 * This process loop continuously until the Blinking mode is disabled.
 * @param time_interval Set time interval for each fade step (0-15)
 * 0 - 8 Frames
 * 1 - 16 Frames
 * 2 - 24 Frames
 * ...
 * 15 - 128 Frames
*/
bool ssd1306_set_fade_out_and_blinking(const ssd1306_t* ssd1306, ssd1306_fade_out_blinking_mode_t mode, uint8_t time_interval) {
    bool is_ok = false;
    if ((mode == SSD1306_FADE_OUT_BLINKING_DISABLE || mode == SSD1306_FADE_OUT_MODE || mode == SSD1306_BLINKING_MODE) &&
        (time_interval >= SSD1306_FADE_OUT_BLINKING_TIME_INTERVAL_MIN && time_interval <= SSD1306_FADE_OUT_BLINKING_TIME_INTERVAL_MAX)) {
        const uint8_t value = mode | time_interval;
        is_ok = ssd1306_send_command_value(ssd1306, SSD1306_FADE_OUT_BLINKING_COMMAND, value);
    }
    return is_ok;
}

/**
 * Validate Page
 * @param page (0-7)
*/
static bool is_valid_page(uint8_t page) {
    return page >= SSD1306_PAGE_START_ADDRESS && page <= SSD1306_PAGE_END_ADDRESS;
}

/**
 * Validate Column
 * @param column (0-127)
*/
static bool is_valid_column(uint8_t column) {
    return column >= SSD1306_COLUMN_START_ADDRESS && column <= SSD1306_COLUMN_END_ADDRESS;
}

/**
 * Set area for draw
 * @param ssd1306
 * @param start_page (0-7)
 * @param end_page (0-7)
 * @param start_column (0-127)
 * @param end_column (0-127)
*/
static bool ssd1306_set_area(const ssd1306_t* ssd1306, uint8_t start_page, uint8_t end_page, uint8_t start_column, uint8_t end_column) {
    bool is_ok;
    is_ok = i2c_start(ssd1306->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(SSD1306_SEND_COMMAND);
    is_ok = is_ok && i2c_write_byte(SSD1306_DISPLAY_START_LINE_COMMAND); // Reset start line

    if (is_valid_page(start_page) && is_valid_page(end_page)) {
        is_ok = is_ok && i2c_write_byte(SSD1306_PAGE_START_END_ADDRESS_COMMAND);
        is_ok = is_ok && i2c_write_byte(start_page);
        is_ok = is_ok && i2c_write_byte(end_page);
    }

    if (is_valid_column(start_column) && is_valid_column(end_column)) {
        is_ok = is_ok && i2c_write_byte(SSD1306_COLUMN_START_END_ADDRESS_COMMAND);
        is_ok = is_ok && i2c_write_byte(start_column);
        is_ok = is_ok && i2c_write_byte(end_column);
    }

    i2c_stop();
    return is_ok;
}

bool ssd1306_clear_display(const ssd1306_t* ssd1306) {
    bool is_ok = ssd1306_set_area(ssd1306, SSD1306_PAGE_START_ADDRESS, SSD1306_PAGE_END_ADDRESS, SSD1306_COLUMN_START_ADDRESS, SSD1306_COLUMN_END_ADDRESS);

    // Reset Display
    is_ok = is_ok && i2c_start(ssd1306->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(SSD1306_SEND_DATA);
    for (int i = 0; i < SSD1306_DISPLAY_BYTES; i++) {
        is_ok = is_ok && i2c_write_byte(0x00);
    }
    i2c_stop();
    return is_ok;
}

/**
 * Draw Bitmap
 * @param ssd1306
 * @param start_page (0-7)
 * @param start_column (0-127)
 * @param width Width bitmap
 * @param height Height bitmap
 * @param bitmap Bitmap (from PROGMEM)
*/
bool ssd1306_draw_bitmap(const ssd1306_t* ssd1306, uint8_t start_page, uint8_t start_column, uint8_t width, uint8_t height, ssd1306_bitmap_t bitmap) {
    bool is_ok;
    const uint8_t end_page = height / SSD1306_BITS_PER_COLUMN + start_page;
    const uint8_t end_column = start_column + width - 1;
    is_ok = ssd1306_set_area(ssd1306, start_page, end_page, start_column, end_column);

    is_ok = is_ok && i2c_start(ssd1306->i2c_address, I2C_MODE_WRITE);
    is_ok = is_ok && i2c_write_byte(SSD1306_SEND_DATA);

    const uint8_t x_len = div_ceil(width, SSD1306_BITS_IN_BYTE);
    const uint8_t y_len = div_ceil(height, SSD1306_BITS_IN_BYTE);
    const uint16_t max_index = width * height / SSD1306_BITS_IN_BYTE;

    for (uint8_t y = 0; y < y_len; y++) {
        for (uint8_t x = 0; x < x_len; x++) {
            uint8_t buff[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
            for (int8_t i = 0; i < SSD1306_BITS_IN_BYTE; i++) {
                const uint16_t index = i * x_len + x + y * width;
                if (index < max_index) {
                    buff[i] = pgm_read_byte(&bitmap[index]);
                }
            }
            for (uint8_t col = 0; col < SSD1306_BITS_IN_BYTE; col++) {
                uint8_t dst_value = 0;
                for (uint8_t bit = 0; bit < SSD1306_BITS_IN_BYTE; bit++) {
                    const uint8_t src_value = buff[bit];
                    const uint8_t src_bit = col;
                    copy_bit(src_value, dst_value, src_bit, bit);
                }
                is_ok = is_ok && i2c_write_byte(dst_value);
            }
        }
    }
    i2c_stop();
    return is_ok;
};

ssd1306_config_t ssd1306_create_config(uint8_t i2c_address) {
    ssd1306_config_t settings = { .i2c_address = i2c_address };

    // 1. Fundamental Command
    settings.contrast = SSD1306_CONTRAST_DEFAULT;
    settings.inverse = false;

    // 2. Scrolling Command
    // ...

    // 3. Addressing Setting Command
    settings.memory_addressing_mode = SSD1306_MEMORY_ADDRESSING_MODE_HORIZONTAL;

    // 4. Hardware Configuration Command
    settings.segment_re_map_inverse = true;
    settings.mux_ratio = SSD1306_MUX_RATIO_MAX;
    settings.com_output_scan_direction_remapped = true;
    settings.com_alt_pin_config = true;
    settings.com_disable_left_right_remap = true;

    // 5. Timing & Driving Scheme Setting Command
    settings.divide_ratio = SSD1306_DISPLAY_CLOCK_DIVIDE_RATIO_MIN;
    settings.oscillator_frequency = SSD1306_DISPLAY_CLOCK_OSCILLATOR_FREQUENCY_MAX;
    settings.pre_charge_period_phase_1 = SSD1306_PRE_CHARGE_PERIOD_PHASE1_DEFAULT;
    settings.pre_charge_period_phase_2 = SSD1306_PRE_CHARGE_PERIOD_PHASE2_DEFAULT;
    settings.vcomh_deselect_level = SSD1306_VCOMH_DESELECT_LEVEL1;

    // 6. Advance Graphic Command
    settings.fade_out_blinking_mode = SSD1306_FADE_OUT_BLINKING_DISABLE;
    settings.fade_out_time_interval = SSD1306_FADE_OUT_BLINKING_TIME_INTERVAL_MIN;
    settings.zoom = false;

    // 7. Charge Pump Command
    settings.charge_pump = true;

    return settings;
}

ssd1306_t ssd1306_create(const ssd1306_config_t* settings) {
    ssd1306_t ssd1306 = { .i2c_address = settings->i2c_address, .font = NULL };
    return ssd1306;
};

bool ssd1306_init(const ssd1306_t* ssd1306, const ssd1306_config_t* config) {
    bool is_ok = true;
    is_ok = is_ok && ssd1306_set_com_output_scan_direction(ssd1306, config->com_output_scan_direction_remapped);
    is_ok = is_ok && ssd1306_set_multiplex_ratio(ssd1306, config->mux_ratio);
    is_ok = is_ok && ssd1306_set_display_clock(ssd1306, config->divide_ratio, config->oscillator_frequency);
    is_ok = is_ok && ssd1306_set_inverse(ssd1306, config->inverse);
    is_ok = is_ok && ssd1306_set_contrast(ssd1306, config->contrast);
    is_ok = is_ok && ssd1306_set_fade_out_and_blinking(ssd1306, config->fade_out_blinking_mode, config->fade_out_time_interval);
    is_ok = is_ok && ssd1306_set_zoom(ssd1306, config->zoom);
    is_ok = is_ok && ssd1306_set_offset(ssd1306, SSD1306_DISPLAY_OFFSET_MIN);
    is_ok = is_ok && ssd1306_set_memory_addressing_mode(ssd1306, config->memory_addressing_mode);
    is_ok = is_ok && ssd1306_set_pre_charge_period(ssd1306, config->pre_charge_period_phase_1, config->pre_charge_period_phase_2);
    is_ok = is_ok && ssd1306_set_vcomh_deselect_level(ssd1306, config->vcomh_deselect_level);
    is_ok = is_ok && ssd1306_set_com_pins_hardware_config(ssd1306, config->com_alt_pin_config, config->com_disable_left_right_remap);
    is_ok = is_ok && ssd1306_set_segment_re_map(ssd1306, config->segment_re_map_inverse);
    is_ok = is_ok && ssd1306_set_charge_pump(ssd1306, config->charge_pump);
    is_ok = is_ok && ssd1306_display_on(ssd1306);
    return is_ok;
}

ssd1306_bitmap_t ssd1306_find_char(char chr, const ssd1306_font_t* font) {
    for (uint8_t i = 0; i < font->size; i++) {
        if (font->data[i].letter == chr) {
            return font->data[i].bitmap;
        }
    }
    return NULL;
}

void ssd1306_set_font(ssd1306_t* ssd1306, const ssd1306_font_t* font) {
    ssd1306->font = font;
}

bool ssd1306_print(ssd1306_t* ssd1306, const char* text, uint8_t start_page, uint8_t start_column) {
    const ssd1306_font_t* font = ssd1306->font;
    if (font == NULL) {
        return false;
    }

    bool is_ok = true;
    uint8_t i = 0;

    while (text[i] != '\0') {
        ssd1306_bitmap_t bitmap = ssd1306_find_char(text[i], font);
        if (bitmap != NULL) {
            const uint8_t column = start_column + i * font->width + i * font->letter_spacing;
            is_ok = is_ok && ssd1306_draw_bitmap(ssd1306, start_page, column, font->width, font->height, bitmap);
        }
        i++;
    }
    return is_ok;
}
