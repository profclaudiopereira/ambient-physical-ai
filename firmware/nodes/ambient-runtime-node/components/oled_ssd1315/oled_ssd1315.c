#include "oled_ssd1315.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define OLED_ADDR 0x3C
#define OLED_WIDTH 72
#define OLED_HEIGHT 40
#define OLED_PAGES 5

static i2c_master_dev_handle_t oled_dev = NULL;

static esp_err_t oled_cmd(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd};
    return i2c_master_transmit(oled_dev, data, sizeof(data), 100);
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

esp_err_t oled_ssd1315_init(i2c_master_bus_handle_t bus)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = OLED_ADDR,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_cfg, &oled_dev));

    vTaskDelay(pdMS_TO_TICKS(50));

    oled_cmd(0xAE); // display off
    oled_cmd(0xD5); oled_cmd(0x80);
    oled_cmd(0xA8); oled_cmd(0x27); // 40px height
    oled_cmd(0xD3); oled_cmd(0x00);
    oled_cmd(0x40);
    oled_cmd(0x8D); oled_cmd(0x14);
    oled_cmd(0x20); oled_cmd(0x00);
    oled_cmd(0xA1);
    oled_cmd(0xC8);
    oled_cmd(0xDA); oled_cmd(0x12);
    oled_cmd(0x81); oled_cmd(0xCF);
    oled_cmd(0xD9); oled_cmd(0xF1);
    oled_cmd(0xDB); oled_cmd(0x40);
    oled_cmd(0xA4);
    oled_cmd(0xA6);
    oled_cmd(0xAF); // display on

    return oled_ssd1315_clear();
}

esp_err_t oled_ssd1315_clear(void)
{
    uint8_t empty[OLED_WIDTH];
    memset(empty, 0x00, sizeof(empty));

    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        oled_cmd(0xB0 + page);
        oled_cmd(0x00);
        oled_cmd(0x10);
        oled_data(empty, OLED_WIDTH);
    }

    return ESP_OK;
}

static esp_err_t oled_write_pattern_line(uint8_t page, uint8_t pattern)
{
    uint8_t block[OLED_WIDTH];
    memset(block, pattern, sizeof(block));

    oled_cmd(0xB0 + page);
    oled_cmd(0x00);
    oled_cmd(0x10);

    return oled_data(block, OLED_WIDTH);
}


esp_err_t oled_ssd1315_print_demo(void)
{
    uint8_t block[OLED_WIDTH];

    oled_ssd1315_clear();

    // Simple visual test: three horizontal filled bands
    memset(block, 0xFF, sizeof(block));

    oled_cmd(0xB0);
    oled_cmd(0x00);
    oled_cmd(0x10);
    oled_data(block, OLED_WIDTH);

    oled_cmd(0xB2);
    oled_cmd(0x00);
    oled_cmd(0x10);
    oled_data(block, OLED_WIDTH);

    oled_cmd(0xB4);
    oled_cmd(0x00);
    oled_cmd(0x10);
    oled_data(block, OLED_WIDTH);

    return ESP_OK;
}


esp_err_t oled_ssd1315_print_status(void)
{
    oled_ssd1315_clear();

    // Simple readable status pattern:
    // This is not a font yet, but creates a stable visual signature.
    oled_write_pattern_line(0, 0xFF);
    oled_write_pattern_line(1, 0x81);
    oled_write_pattern_line(2, 0xBD);
    oled_write_pattern_line(3, 0x81);
    oled_write_pattern_line(4, 0xFF);

    return ESP_OK;
}