#pragma once

#include <stddef.h>
#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * M5Stack Unit OLED (U119) native controller geometry.
 *
 * The SH1107 RAM is addressed as 64 columns x 128 rows. The controller uses
 * 16 pages, each page representing eight vertical pixels. The visible panel
 * occupies columns 32..95 of the controller RAM.
 */
#define OLED_SH1107_WIDTH_PIXELS   64
#define OLED_SH1107_HEIGHT_PIXELS  128
#define OLED_SH1107_PAGE_COUNT     16
#define OLED_SH1107_TEXT_COLUMNS   10

/**
 * @brief Initializes the M5Stack Unit OLED / SH1107 on an existing I2C bus.
 *
 * The caller owns PaHub channel selection and I2C bus lifecycle. This
 * component owns only the SH1107 device handle, framebuffer writes and
 * display commands.
 */
esp_err_t oled_sh1107_init(i2c_master_bus_handle_t bus);

/** @brief Clears the complete visible display area. */
esp_err_t oled_sh1107_clear(void);

/**
 * @brief Draws one 5x7 ASCII character.
 *
 * @param page SH1107 page in the range 0..15.
 * @param col Visible pixel column in the range 0..63.
 */
esp_err_t oled_sh1107_draw_char(uint8_t page, uint8_t col, char character);

/**
 * @brief Draws a clipped ASCII string using a 5x7 font plus one blank column.
 */
esp_err_t oled_sh1107_draw_text(uint8_t page, uint8_t col, const char *text);

/** @brief Clears one 8-pixel-high page. */
esp_err_t oled_sh1107_clear_page(uint8_t page);

/** @brief Changes display contrast. */
esp_err_t oled_sh1107_set_contrast(uint8_t contrast);

/* Bench-validation helpers. */
esp_err_t oled_sh1107_print_test(void);
esp_err_t oled_sh1107_print_ok(void);
esp_err_t oled_sh1107_print_ambient_test(void);
esp_err_t oled_sh1107_print_geometry_test(void);

#ifdef __cplusplus
}
#endif
