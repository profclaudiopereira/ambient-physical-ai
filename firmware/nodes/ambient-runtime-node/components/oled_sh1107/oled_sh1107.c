#include "oled_sh1107.h"

#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
 * Hardware contract:
 * - M5Stack Unit OLED U119
 * - SH1107 controller
 * - I2C address 0x3C
 * - visible RAM window: controller columns 32..95
 *
 * The initialization sequence below follows the command order used by the
 * official M5Stack M5GFX Panel_SH110x implementation. In particular, the
 * multiplex ratio is 0x7F because the native SH1107 organization is 64x128.
 */
#define OLED_ADDR              0x3C
#define OLED_WIDTH             OLED_SH1107_WIDTH_PIXELS
#define OLED_PAGES             OLED_SH1107_PAGE_COUNT
#define OLED_COL_OFFSET        32
#define OLED_I2C_TIMEOUT_MS    100

static i2c_master_dev_handle_t s_oled_dev = NULL;

/*
 * Compact 5x7 ASCII font covering characters 0x20 through 0x7F.
 * Each byte represents one vertical column; bit 0 is the top pixel.
 */
static const uint8_t s_font5x7[96][5] = {
    {0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x5F,0x00,0x00},
    {0x00,0x07,0x00,0x07,0x00},{0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12},{0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1C,0x22,0x41,0x00},{0x00,0x41,0x22,0x1C,0x00},
    {0x14,0x08,0x3E,0x08,0x14},{0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},
    {0x00,0x60,0x60,0x00,0x00},{0x20,0x10,0x08,0x04,0x02},
    {0x3E,0x51,0x49,0x45,0x3E},{0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10},{0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1E},
    {0x00,0x36,0x36,0x00,0x00},{0x00,0x56,0x36,0x00,0x00},
    {0x08,0x14,0x22,0x41,0x00},{0x14,0x14,0x14,0x14,0x14},
    {0x00,0x41,0x22,0x14,0x08},{0x02,0x01,0x51,0x09,0x06},
    {0x32,0x49,0x79,0x41,0x3E},{0x7E,0x11,0x11,0x11,0x7E},
    {0x7F,0x49,0x49,0x49,0x36},{0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C},{0x7F,0x49,0x49,0x49,0x41},
    {0x7F,0x09,0x09,0x09,0x01},{0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F},{0x00,0x41,0x7F,0x41,0x00},
    {0x20,0x40,0x41,0x3F,0x01},{0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40},{0x7F,0x02,0x0C,0x02,0x7F},
    {0x7F,0x04,0x08,0x10,0x7F},{0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06},{0x3E,0x41,0x51,0x21,0x5E},
    {0x7F,0x09,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01},{0x3F,0x40,0x40,0x40,0x3F},
    {0x1F,0x20,0x40,0x20,0x1F},{0x3F,0x40,0x38,0x40,0x3F},
    {0x63,0x14,0x08,0x14,0x63},{0x07,0x08,0x70,0x08,0x07},
    {0x61,0x51,0x49,0x45,0x43},{0x00,0x7F,0x41,0x41,0x00},
    {0x02,0x04,0x08,0x10,0x20},{0x00,0x41,0x41,0x7F,0x00},
    {0x04,0x02,0x01,0x02,0x04},{0x40,0x40,0x40,0x40,0x40},
    {0x00,0x01,0x02,0x04,0x00},{0x20,0x54,0x54,0x54,0x78},
    {0x7F,0x48,0x44,0x44,0x38},{0x38,0x44,0x44,0x44,0x20},
    {0x38,0x44,0x44,0x48,0x7F},{0x38,0x54,0x54,0x54,0x18},
    {0x08,0x7E,0x09,0x01,0x02},{0x0C,0x52,0x52,0x52,0x3E},
    {0x7F,0x08,0x04,0x04,0x78},{0x00,0x44,0x7D,0x40,0x00},
    {0x20,0x40,0x44,0x3D,0x00},{0x7F,0x10,0x28,0x44,0x00},
    {0x00,0x41,0x7F,0x40,0x00},{0x7C,0x04,0x18,0x04,0x78},
    {0x7C,0x08,0x04,0x04,0x78},{0x38,0x44,0x44,0x44,0x38},
    {0x7C,0x14,0x14,0x14,0x08},{0x08,0x14,0x14,0x18,0x7C},
    {0x7C,0x08,0x04,0x04,0x08},{0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3F,0x44,0x40,0x20},{0x3C,0x40,0x40,0x20,0x7C},
    {0x1C,0x20,0x40,0x20,0x1C},{0x3C,0x40,0x30,0x40,0x3C},
    {0x44,0x28,0x10,0x28,0x44},{0x0C,0x50,0x50,0x50,0x3C},
    {0x44,0x64,0x54,0x4C,0x44},{0x00,0x08,0x36,0x41,0x00},
    {0x00,0x00,0x7F,0x00,0x00},{0x00,0x41,0x36,0x08,0x00},
    {0x10,0x08,0x08,0x10,0x08},{0x00,0x06,0x09,0x09,0x06}
};

static esp_err_t oled_validate_ready(void)
{
    return (s_oled_dev != NULL) ? ESP_OK : ESP_ERR_INVALID_STATE;
}

/*
 * Sends one or more SH1107 command bytes in a single I2C transaction.
 * The leading 0x00 control byte selects the command stream.
 */
static esp_err_t oled_cmd_list(const uint8_t *commands, size_t count)
{
    if (commands == NULL || count == 0U) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = oled_validate_ready();
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t tx[32];

    if (count > (sizeof(tx) - 1U)) {
        return ESP_ERR_INVALID_SIZE;
    }

    tx[0] = 0x00;
    memcpy(&tx[1], commands, count);

    return i2c_master_transmit(
        s_oled_dev,
        tx,
        count + 1U,
        OLED_I2C_TIMEOUT_MS
    );
}

static esp_err_t oled_cmd(uint8_t command)
{
    return oled_cmd_list(&command, 1U);
}

/*
 * Selects one native SH1107 page and one visible column.
 *
 * M5Stack's official panel configuration uses panel_width = 64 and
 * offset_x = 32. Therefore logical column zero maps to controller RAM
 * column 32; no experimental offset calibration is required.
 */
static esp_err_t oled_set_page_col(uint8_t page, uint8_t col)
{
    if (page >= OLED_PAGES || col >= OLED_WIDTH) {
        return ESP_ERR_INVALID_ARG;
    }

    const uint8_t real_col = (uint8_t)(col + OLED_COL_OFFSET);
    const uint8_t commands[] = {
        (uint8_t)(0xB0U | page),
        (uint8_t)(0x10U | (real_col >> 4)),
        (uint8_t)(real_col & 0x0FU),
    };

    return oled_cmd_list(commands, sizeof(commands));
}

static esp_err_t oled_data(const uint8_t *data, size_t length)
{
    if (data == NULL || length == 0U) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = oled_validate_ready();
    if (ret != ESP_OK) {
        return ret;
    }

    if (length > OLED_WIDTH) {
        length = OLED_WIDTH;
    }

    uint8_t tx[OLED_WIDTH + 1U];
    tx[0] = 0x40;
    memcpy(&tx[1], data, length);

    return i2c_master_transmit(
        s_oled_dev,
        tx,
        length + 1U,
        OLED_I2C_TIMEOUT_MS
    );
}

esp_err_t oled_sh1107_init(i2c_master_bus_handle_t bus)
{
    if (bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_oled_dev != NULL) {
        return ESP_OK;
    }

    const i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = OLED_ADDR,
        .scl_speed_hz = 400000,
    };

    esp_err_t ret = i2c_master_bus_add_device(bus, &dev_cfg, &s_oled_dev);
    if (ret != ESP_OK) {
        s_oled_dev = NULL;
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    /*
     * SH1107 initialization adapted from the official M5GFX Panel_SH110x
     * command list used by M5UnitOLED.
     *
     * Critical correction versus the previous laboratory driver:
     * 0xA8, 0x7F configures all 128 native multiplex rows. Using 0x3F
     * configured only 64 rows and produced an incomplete/cropped mapping.
     */
    const uint8_t init_sequence[] = {
        0xAE,       /* Display OFF */
        0x40,       /* Display start line */
        0xEE,       /* End read-modify-write */
        0x20,       /* Page addressing mode */
        0xDC, 0x00, /* Display start line = 0 */
        0xD5, 0x50, /* Display clock divide */
        0xA8, 0x7F, /* 128 multiplex rows */
        0xD3, 0x00, /* Display offset = 0 */
        0xAD, 0x8B, /* Internal DC-DC enabled */
        0xA0,       /* Segment remap: normal */
        0xC0,       /* COM scan direction: normal */
        0xD9, 0x20, /* Pre-charge period */
        0xDB, 0x35, /* VCOM deselect level */
        0xA4,       /* Resume display RAM content */
        0x81, 0x7F, /* Practical initial contrast */
        0xDA, 0x12, /* COM pins configuration */
        0xAF        /* Display ON */
    };

    ret = oled_cmd_list(init_sequence, sizeof(init_sequence));
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
    return oled_sh1107_clear();
}

esp_err_t oled_sh1107_clear(void)
{
    uint8_t empty[OLED_WIDTH];
    memset(empty, 0x00, sizeof(empty));

    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        esp_err_t ret = oled_set_page_col(page, 0);
        if (ret != ESP_OK) {
            return ret;
        }

        ret = oled_data(empty, sizeof(empty));
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ESP_OK;
}

esp_err_t oled_sh1107_clear_page(uint8_t page)
{
    if (page >= OLED_PAGES) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t empty[OLED_WIDTH];
    memset(empty, 0x00, sizeof(empty));

    esp_err_t ret = oled_set_page_col(page, 0);
    if (ret != ESP_OK) {
        return ret;
    }

    return oled_data(empty, sizeof(empty));
}

esp_err_t oled_sh1107_draw_char(uint8_t page, uint8_t col, char character)
{
    if (page >= OLED_PAGES || col >= OLED_WIDTH) {
        return ESP_ERR_INVALID_ARG;
    }

    unsigned char code = (unsigned char)character;
    if (code < 0x20U || code > 0x7FU) {
        code = (unsigned char)'?';
    }

    const uint8_t glyph[6] = {
        s_font5x7[code - 0x20U][0],
        s_font5x7[code - 0x20U][1],
        s_font5x7[code - 0x20U][2],
        s_font5x7[code - 0x20U][3],
        s_font5x7[code - 0x20U][4],
        0x00
    };

    size_t writable = sizeof(glyph);
    if ((size_t)col + writable > OLED_WIDTH) {
        writable = OLED_WIDTH - col;
    }

    esp_err_t ret = oled_set_page_col(page, col);
    if (ret != ESP_OK) {
        return ret;
    }

    return oled_data(glyph, writable);
}

esp_err_t oled_sh1107_draw_text(uint8_t page, uint8_t col, const char *text)
{
    if (text == NULL || page >= OLED_PAGES || col >= OLED_WIDTH) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t cursor = col;

    while (*text != '\0' && cursor < OLED_WIDTH) {
        esp_err_t ret = oled_sh1107_draw_char(page, cursor, *text);
        if (ret != ESP_OK) {
            return ret;
        }

        text++;

        if ((uint16_t)cursor + 6U >= OLED_WIDTH) {
            break;
        }

        cursor = (uint8_t)(cursor + 6U);
    }

    return ESP_OK;
}

esp_err_t oled_sh1107_set_contrast(uint8_t contrast)
{
    const uint8_t commands[] = {0x81, contrast};
    return oled_cmd_list(commands, sizeof(commands));
}

esp_err_t oled_sh1107_print_test(void)
{
    esp_err_t ret = oled_sh1107_clear();
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t line[OLED_WIDTH];

    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        memset(line, (page % 2U == 0U) ? 0xFF : 0x00, sizeof(line));

        ret = oled_set_page_col(page, 0);
        if (ret != ESP_OK) {
            return ret;
        }

        ret = oled_data(line, sizeof(line));
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ESP_OK;
}

esp_err_t oled_sh1107_print_ok(void)
{
    esp_err_t ret = oled_sh1107_clear();
    if (ret != ESP_OK) {
        return ret;
    }

    return oled_sh1107_draw_text(4, 20, "OK");
}

esp_err_t oled_sh1107_print_ambient_test(void)
{
    esp_err_t ret = oled_sh1107_clear();
    if (ret != ESP_OK) {
        return ret;
    }

    ret = oled_sh1107_draw_text(2, 8, "AI");
    if (ret != ESP_OK) {
        return ret;
    }

    ret = oled_sh1107_draw_text(5, 8, "RUN");
    if (ret != ESP_OK) {
        return ret;
    }

    return oled_sh1107_draw_text(8, 8, "TEST");
}

/*
 * Calibration pattern used only to validate the complete 64-column window.
 * The first and last vertical markers must both be visible.
 */
esp_err_t oled_sh1107_print_geometry_test(void)
{
    esp_err_t ret = oled_sh1107_clear();
    if (ret != ESP_OK) {
        return ret;
    }

    ret = oled_sh1107_draw_text(1, 0, "1234567890");
    if (ret != ESP_OK) {
        return ret;
    }

    ret = oled_sh1107_draw_text(4, 0, "ABCDEFGHIJ");
    if (ret != ESP_OK) {
        return ret;
    }

    ret = oled_sh1107_draw_text(7, 0, "|        |");
    if (ret != ESP_OK) {
        return ret;
    }

    return oled_sh1107_draw_text(10, 0, "CONTEXT");
}
