#include "oled_sh1107.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define OLED_ADDR        0x3C
#define OLED_WIDTH       64
#define OLED_PAGES       16
#define OLED_COL_OFFSET  32

static i2c_master_dev_handle_t oled_dev = NULL;

static esp_err_t oled_cmd(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd};
    return i2c_master_transmit(oled_dev, data, sizeof(data), 100);
}

static esp_err_t oled_set_page_col(uint8_t page, uint8_t col)
{
    uint8_t real_col = col + OLED_COL_OFFSET;

    oled_cmd(0xB0 + page);
    oled_cmd(0x00 | (real_col & 0x0F));
    oled_cmd(0x10 | (real_col >> 4));

    return ESP_OK;
}

static esp_err_t oled_data(const uint8_t *data, size_t len)
{
    uint8_t buffer[OLED_WIDTH + 1];

    if (len > OLED_WIDTH) {
        len = OLED_WIDTH;
    }

    buffer[0] = 0x40;
    memcpy(&buffer[1], data, len);

    return i2c_master_transmit(oled_dev, buffer, len + 1, 100);
}

esp_err_t oled_sh1107_init(i2c_master_bus_handle_t bus)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = OLED_ADDR,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_cfg, &oled_dev));

    vTaskDelay(pdMS_TO_TICKS(50));

    oled_cmd(0xAE);        // Display OFF
    oled_cmd(0xD5);        // Display clock divide
    oled_cmd(0x50);

    oled_cmd(0xA8);        // Multiplex ratio
    oled_cmd(0x3F);        // 64 mux

    oled_cmd(0xD3);        // Display offset
    oled_cmd(0x00);

    oled_cmd(0x40);        // Display start line

    //oled_cmd(0xA1);        // Segment remap
    //oled_cmd(0xC8);        // COM scan direction

    oled_cmd(0xA0);
    oled_cmd(0xC0);

    oled_cmd(0x81);        // Contrast
    oled_cmd(0x7F);

    oled_cmd(0xA4);        // Resume RAM content display
    oled_cmd(0xA6);        // Normal display

    oled_cmd(0xD9);        // Pre-charge
    oled_cmd(0x22);

    oled_cmd(0xDB);        // VCOM deselect
    oled_cmd(0x35);

    oled_cmd(0xAD);        // DC-DC control
    oled_cmd(0x8B);

    oled_cmd(0xAF);        // Display ON

    vTaskDelay(pdMS_TO_TICKS(50));

    return oled_sh1107_clear();
}

esp_err_t oled_sh1107_clear(void)
{
    uint8_t empty[OLED_WIDTH];
    memset(empty, 0x00, sizeof(empty));

    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        oled_set_page_col(page, 0);
        oled_data(empty, OLED_WIDTH);
    }

    return ESP_OK;
}

esp_err_t oled_sh1107_print_test(void)
{
    oled_sh1107_clear();

    uint8_t line[OLED_WIDTH];

    for (uint8_t page = 0; page < OLED_PAGES; page++) {

        if (page % 2 == 0) {
            memset(line, 0xFF, sizeof(line));
        } else {
            memset(line, 0x00, sizeof(line));
        }

        oled_set_page_col(page, 0);
        oled_data(line, OLED_WIDTH);
    }

    return ESP_OK;
}

static esp_err_t oled_draw_glyph(uint8_t page, uint8_t col, const uint8_t glyph[5])
{
    oled_set_page_col(page, col);

    uint8_t data[6] = {
        glyph[0], glyph[1], glyph[2], glyph[3], glyph[4], 0x00
    };

    return oled_data(data, sizeof(data));
}

esp_err_t oled_sh1107_print_ok(void)
{
    oled_sh1107_clear();

    const uint8_t O[5] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
    const uint8_t K[5] = {0x7F, 0x08, 0x14, 0x22, 0x41};

    oled_draw_glyph(4, 20, O);
    oled_draw_glyph(4, 28, K);

    return ESP_OK;
}


esp_err_t oled_sh1107_print_ambient_test(void)
{
    oled_sh1107_clear();

    const uint8_t A[5] = {0x7E, 0x11, 0x11, 0x11, 0x7E};
    const uint8_t I[5] = {0x00, 0x41, 0x7F, 0x41, 0x00};

    const uint8_t R[5] = {0x7F, 0x09, 0x19, 0x29, 0x46};
    const uint8_t U[5] = {0x3F, 0x40, 0x40, 0x40, 0x3F};
    const uint8_t N[5] = {0x7F, 0x02, 0x04, 0x08, 0x7F};

    const uint8_t T[5] = {0x01, 0x01, 0x7F, 0x01, 0x01};
    const uint8_t E[5] = {0x7F, 0x49, 0x49, 0x49, 0x41};
    const uint8_t S[5] = {0x46, 0x49, 0x49, 0x49, 0x31};

    // Line 1: AI
    oled_draw_glyph(2, 8, A);
    oled_draw_glyph(2, 16, I);

    // Line 2: RUN
    oled_draw_glyph(5, 8, R);
    oled_draw_glyph(5, 16, U);
    oled_draw_glyph(5, 24, N);

    // Line 3: TEST
    oled_draw_glyph(8, 8, T);
    oled_draw_glyph(8, 16, E);
    oled_draw_glyph(8, 24, S);
    oled_draw_glyph(8, 32, T);

    return ESP_OK;
}