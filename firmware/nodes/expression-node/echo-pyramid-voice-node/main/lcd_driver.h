/**
 * @file lcd_driver.h
 * @brief Native ESP-IDF drawing primitives for the AtomS3R LCD.
 */

#pragma once

#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_WIDTH  128
#define LCD_HEIGHT 128

#define LCD_COLOR_BLACK   0x0000
#define LCD_COLOR_WHITE   0xFFFF
#define LCD_COLOR_RED     0xF800
#define LCD_COLOR_GREEN   0x07E0
#define LCD_COLOR_BLUE    0x001F
#define LCD_COLOR_CYAN    0x07FF
#define LCD_COLOR_YELLOW  0xFFE0
#define LCD_COLOR_GRAY    0x8410
#define LCD_COLOR_NAVY    0x000F

esp_err_t lcd_driver_init(void);
esp_err_t lcd_driver_set_backlight(uint8_t brightness);
esp_err_t lcd_driver_fill(uint16_t rgb565);
esp_err_t lcd_driver_fill_rect(uint16_t x, uint16_t y,
                               uint16_t width, uint16_t height,
                               uint16_t rgb565);
esp_err_t lcd_driver_draw_text(uint16_t x, uint16_t y,
                               const char *text,
                               uint16_t foreground,
                               uint16_t background,
                               uint8_t scale);
int lcd_driver_text_width(const char *text, uint8_t scale);

#ifdef __cplusplus
}
#endif
