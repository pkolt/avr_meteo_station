#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "i2c.h"
#include "ssd1306.h"
#include "bmp180.h"
#include "bitwise.h"
#include "numeric_font.h"
#include "thermometer_bitmap.h"
#include "barometer_bitmap.h"

#define SSD1306_I2C_ADDRESS 0x3C
#define BMP180_I2C_ADDRESS 0x77
#define LED_PIN PB5 // D13

char get_trend(int32_t *prev_val, int32_t *val) {
  if (*prev_val == *val) {
    return ' ';
  }
  return *val > *prev_val ? '<' : '>';
}

void update_display(const ssd1306_t *ssd1306, int32_t *prev_temp, int32_t *temp, int32_t *prev_press, int32_t *press) {
  #define TEXT_MARGIN 5
  #define IMG_MARGIN 16

  static char buff[10];

  ssd1306_clear_display(ssd1306);

  ssd1306_draw_bitmap(ssd1306, 0, IMG_MARGIN, THERMOMETER_BITMAP_WIDTH, THERMOMETER_BITMAP_HEIGHT, thermometer_bitmap);

  buff[0] = '\0';
  sprintf_P(buff, PSTR("%c%d.%d*%c"), *temp > 0 ? '+' : '-', abs(*temp / 10), abs(*temp % 10), get_trend(prev_temp, temp));
  ssd1306_print(ssd1306, buff, 1, THERMOMETER_BITMAP_WIDTH + TEXT_MARGIN + IMG_MARGIN);

  buff[0] = '\0';

  ssd1306_draw_bitmap(ssd1306, 4, IMG_MARGIN, BAROMETER_BITMAP_WIDTH, BAROMETER_BITMAP_HEIGHT, barometer_bitmap);
  sprintf_P(buff, PSTR(" %dh%c"), bmp180_pressure_to_mm(press), get_trend(prev_press, press));
  ssd1306_print(ssd1306, buff, 5, BAROMETER_BITMAP_WIDTH + TEXT_MARGIN + IMG_MARGIN);
}

int main(void) {
  set_bit(DDRB, LED_PIN); // Pin as OUTPUT

  i2c_init();

  ssd1306_config_t ssd1306_cfg = ssd1306_create_config(SSD1306_I2C_ADDRESS);
  // ssd1306_cfg.contrast = 1;
  ssd1306_t ssd1306 = ssd1306_create(&ssd1306_cfg);
  ssd1306_set_font(&ssd1306, &numeric_font);

  if (!ssd1306_init(&ssd1306, &ssd1306_cfg)) {
    while(1) {}
  }

  bmp180_t bmp180 = bmp180_create(BMP180_I2C_ADDRESS);
  if (!bmp180_init(&bmp180)) {
    while(1) {}
  }

  bool is_first_measure = true;
  int32_t prev_temp = 0;
  int32_t prev_press = 0;
  int32_t temp = 0; // Temperature in 0.1 C
  int32_t press = 0; // Pressure in Pa

  while (1) {
    prev_temp = temp;
    prev_press = press;
    if (bmp180_get_temperature(&bmp180, &temp) && bmp180_get_pressure(&bmp180, &press)) {
      if (is_first_measure) {
        is_first_measure = false;
        prev_temp = temp;
        prev_press = press;
      }
      update_display(&ssd1306, &prev_temp, &temp, &prev_press, &press);
    }
    _delay_ms(60000);
  }
}