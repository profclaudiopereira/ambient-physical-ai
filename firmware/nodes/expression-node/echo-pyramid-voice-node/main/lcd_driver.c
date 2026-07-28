/**
 * @file lcd_driver.c
 * @brief Native ESP-IDF LCD driver and minimal text renderer for AtomS3R.
 *
 * The hardware initialization preserves the validated SPI and LP5562
 * backlight baseline. Text rendering is intentionally lightweight and avoids
 * Arduino, M5Unified and LVGL so the display remains ready for the upcoming
 * wake-word and voice-command integration.
 */

#include "lcd_driver.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "atoms3r_lcd";

#define LCD_PIN_MOSI       GPIO_NUM_21
#define LCD_PIN_SCLK       GPIO_NUM_15
#define LCD_PIN_CS         GPIO_NUM_14
#define LCD_PIN_DC         GPIO_NUM_42
#define LCD_PIN_RESET      GPIO_NUM_48
#define LCD_SPI_HOST       SPI2_HOST
#define LCD_SPI_CLOCK_HZ   (20 * 1000 * 1000)

#define SYS_I2C_PORT       I2C_NUM_1
#define SYS_I2C_SDA        GPIO_NUM_45
#define SYS_I2C_SCL        GPIO_NUM_0
#define SYS_I2C_FREQ_HZ    400000
#define LP5562_ADDRESS     0x30

#define LP5562_REG_ENABLE      0x00
#define LP5562_REG_OP_MODE     0x01
#define LP5562_REG_CONFIG      0x08
#define LP5562_REG_RESET       0x0D
#define LP5562_REG_WHITE_PWM   0x0E
#define LP5562_REG_WHITE_I     0x0F
#define LP5562_REG_LED_MAP     0x70

static spi_device_handle_t s_lcd = NULL;
static i2c_master_bus_handle_t s_system_i2c = NULL;
static i2c_master_dev_handle_t s_lp5562 = NULL;
static bool s_initialized = false;

/*
 * Compact 5x7 font. Each byte represents one vertical column; bit 0 is the
 * top pixel. Unsupported bytes are rendered as '?'.
 */
static const uint8_t FONT_5X7[96][5] = {
{0,0,0,0,0},{0,0,95,0,0},{0,7,0,7,0},{20,127,20,127,20},{36,42,127,42,18},{35,19,8,100,98},{54,73,85,34,80},{0,5,3,0,0},{0,28,34,65,0},{0,65,34,28,0},{20,8,62,8,20},{8,8,62,8,8},{0,80,48,0,0},{8,8,8,8,8},{0,96,96,0,0},{32,16,8,4,2},
{62,81,73,69,62},{0,66,127,64,0},{66,97,81,73,70},{33,65,69,75,49},{24,20,18,127,16},{39,69,69,69,57},{60,74,73,73,48},{1,113,9,5,3},{54,73,73,73,54},{6,73,73,41,30},{0,54,54,0,0},{0,86,54,0,0},{8,20,34,65,0},{20,20,20,20,20},{0,65,34,20,8},{2,1,81,9,6},
{50,73,121,65,62},{126,17,17,17,126},{127,73,73,73,54},{62,65,65,65,34},{127,65,65,34,28},{127,73,73,73,65},{127,9,9,9,1},{62,65,73,73,122},{127,8,8,8,127},{0,65,127,65,0},{32,64,65,63,1},{127,8,20,34,65},{127,64,64,64,64},{127,2,12,2,127},{127,4,8,16,127},{62,65,65,65,62},
{127,9,9,9,6},{62,65,81,33,94},{127,9,25,41,70},{70,73,73,73,49},{1,1,127,1,1},{63,64,64,64,63},{31,32,64,32,31},{63,64,56,64,63},{99,20,8,20,99},{3,4,120,4,3},{97,81,73,69,67},{0,127,65,65,0},{2,4,8,16,32},{0,65,65,127,0},{4,2,1,2,4},{64,64,64,64,64},{0,1,2,4,0},
{32,84,84,84,120},{127,72,68,68,56},{56,68,68,68,32},{56,68,68,72,127},{56,84,84,84,24},{8,126,9,1,2},{12,82,82,82,62},{127,8,4,4,120},{0,68,125,64,0},{32,64,68,61,0},{127,16,40,68,0},{0,65,127,64,0},{124,4,24,4,120},{124,8,4,4,120},{56,68,68,68,56},
{124,20,20,20,8},{8,20,20,24,124},{124,8,4,4,8},{72,84,84,84,32},{4,63,68,64,32},{60,64,64,32,124},{28,32,64,32,28},{60,64,48,64,60},{68,40,16,40,68},{12,80,80,80,60},{68,100,84,76,68},{0,8,54,65,0},{0,0,127,0,0},{0,65,54,8,0},{8,4,8,16,8},{0,0,0,0,0}
};

static esp_err_t lcd_tx(bool data_mode, const void *data, size_t size)
{
    if (s_lcd == NULL || data == NULL || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_set_level(LCD_PIN_DC, data_mode ? 1 : 0);

    spi_transaction_t transaction = {
        .length = size * 8,
        .tx_buffer = data,
    };

    return spi_device_polling_transmit(s_lcd, &transaction);
}

static esp_err_t lcd_command(uint8_t command)
{
    return lcd_tx(false, &command, sizeof(command));
}

static esp_err_t lcd_data(const void *data, size_t size)
{
    return lcd_tx(true, data, size);
}

static esp_err_t lcd_command_data(uint8_t command,
                                  const void *data,
                                  size_t size)
{
    ESP_RETURN_ON_ERROR(lcd_command(command), TAG,
                        "LCD command 0x%02X failed", command);

    if (data != NULL && size > 0) {
        ESP_RETURN_ON_ERROR(lcd_data(data, size), TAG,
                            "LCD data for command 0x%02X failed", command);
    }

    return ESP_OK;
}

static esp_err_t lp5562_write(uint8_t reg, uint8_t value)
{
    uint8_t payload[2] = {reg, value};
    return i2c_master_transmit(s_lp5562, payload, sizeof(payload), 100);
}

static esp_err_t backlight_controller_init(void)
{
    if (s_system_i2c == NULL) {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = SYS_I2C_PORT,
            .sda_io_num = SYS_I2C_SDA,
            .scl_io_num = SYS_I2C_SCL,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
        };

        ESP_RETURN_ON_ERROR(
            i2c_new_master_bus(&bus_config, &s_system_i2c),
            TAG, "Failed to initialize AtomS3R internal I2C bus");
    }

    if (s_lp5562 == NULL) {
        i2c_device_config_t device_config = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = LP5562_ADDRESS,
            .scl_speed_hz = SYS_I2C_FREQ_HZ,
        };

        ESP_RETURN_ON_ERROR(
            i2c_master_bus_add_device(
                s_system_i2c, &device_config, &s_lp5562),
            TAG, "Failed to attach LP5562 backlight controller");
    }

    ESP_RETURN_ON_ERROR(lp5562_write(LP5562_REG_RESET, 0xFF),
                        TAG, "LP5562 reset failed");
    vTaskDelay(pdMS_TO_TICKS(2));
    ESP_RETURN_ON_ERROR(lp5562_write(LP5562_REG_ENABLE, 0x40),
                        TAG, "LP5562 enable failed");
    vTaskDelay(pdMS_TO_TICKS(1));
    ESP_RETURN_ON_ERROR(lp5562_write(LP5562_REG_CONFIG, 0x01),
                        TAG, "LP5562 configuration failed");
    ESP_RETURN_ON_ERROR(lp5562_write(LP5562_REG_OP_MODE, 0x00),
                        TAG, "LP5562 operation mode failed");
    ESP_RETURN_ON_ERROR(lp5562_write(LP5562_REG_LED_MAP, 0x00),
                        TAG, "LP5562 LED mapping failed");
    ESP_RETURN_ON_ERROR(lp5562_write(LP5562_REG_WHITE_I, 0x19),
                        TAG, "LP5562 white current failed");
    ESP_RETURN_ON_ERROR(lp5562_write(LP5562_REG_WHITE_PWM, 0x00),
                        TAG, "LP5562 initial PWM failed");

    return ESP_OK;
}

esp_err_t lcd_driver_set_backlight(uint8_t brightness)
{
    if (s_lp5562 == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return lp5562_write(LP5562_REG_WHITE_PWM, brightness);
}

static esp_err_t lcd_spi_init(void)
{
    gpio_config_t control_config = {
        .pin_bit_mask = (1ULL << LCD_PIN_DC) | (1ULL << LCD_PIN_RESET),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_RETURN_ON_ERROR(gpio_config(&control_config), TAG,
                        "Failed to configure LCD control GPIOs");

    spi_bus_config_t bus_config = {
        .mosi_io_num = LCD_PIN_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = LCD_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_WIDTH * LCD_HEIGHT * 2,
        .flags = SPICOMMON_BUSFLAG_MASTER |
                 SPICOMMON_BUSFLAG_MOSI |
                 SPICOMMON_BUSFLAG_SCLK,
    };

    esp_err_t err = spi_bus_initialize(
        LCD_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_RETURN_ON_ERROR(err, TAG, "Failed to initialize LCD SPI bus");
    }

    spi_device_interface_config_t device_config = {
        .clock_speed_hz = LCD_SPI_CLOCK_HZ,
        .mode = 0,
        .spics_io_num = LCD_PIN_CS,
        .queue_size = 1,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };

    ESP_RETURN_ON_ERROR(
        spi_bus_add_device(LCD_SPI_HOST, &device_config, &s_lcd),
        TAG, "Failed to attach LCD SPI device");

    return ESP_OK;
}

static void lcd_reset(void)
{
    gpio_set_level(LCD_PIN_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(LCD_PIN_RESET, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(LCD_PIN_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
}

static esp_err_t lcd_controller_init(void)
{
    ESP_RETURN_ON_ERROR(lcd_command(0x01), TAG, "Software reset failed");
    vTaskDelay(pdMS_TO_TICKS(150));
    ESP_RETURN_ON_ERROR(lcd_command(0x11), TAG, "Sleep-out failed");
    vTaskDelay(pdMS_TO_TICKS(120));

    const uint8_t pixel_format = 0x05;
    ESP_RETURN_ON_ERROR(
        lcd_command_data(0x3A, &pixel_format, sizeof(pixel_format)),
        TAG, "Pixel format setup failed");

    const uint8_t memory_access = 0x00;
    ESP_RETURN_ON_ERROR(
        lcd_command_data(0x36, &memory_access, sizeof(memory_access)),
        TAG, "Memory access setup failed");

    ESP_RETURN_ON_ERROR(lcd_command(0x21), TAG,
                        "Display inversion setup failed");
    ESP_RETURN_ON_ERROR(lcd_command(0x29), TAG, "Display-on failed");
    vTaskDelay(pdMS_TO_TICKS(100));

    return ESP_OK;
}

static esp_err_t lcd_set_window(uint16_t x0, uint16_t y0,
                                uint16_t x1, uint16_t y1)
{
    uint8_t columns[4] = {
        (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF),
        (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF),
    };
    uint8_t rows[4] = {
        (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF),
        (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF),
    };

    ESP_RETURN_ON_ERROR(
        lcd_command_data(0x2A, columns, sizeof(columns)),
        TAG, "Column window failed");
    ESP_RETURN_ON_ERROR(
        lcd_command_data(0x2B, rows, sizeof(rows)),
        TAG, "Row window failed");
    ESP_RETURN_ON_ERROR(lcd_command(0x2C), TAG,
                        "Memory write command failed");
    return ESP_OK;
}

esp_err_t lcd_driver_fill_rect(uint16_t x, uint16_t y,
                               uint16_t width, uint16_t height,
                               uint16_t rgb565)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (width == 0 || height == 0 || x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return ESP_ERR_INVALID_ARG;
    }

    if (x + width > LCD_WIDTH) {
        width = LCD_WIDTH - x;
    }
    if (y + height > LCD_HEIGHT) {
        height = LCD_HEIGHT - y;
    }

    ESP_RETURN_ON_ERROR(
        lcd_set_window(x, y, x + width - 1, y + height - 1),
        TAG, "Rectangle window failed");

    uint8_t row[LCD_WIDTH * 2];
    for (uint16_t i = 0; i < width; ++i) {
        row[i * 2] = (uint8_t)(rgb565 >> 8);
        row[i * 2 + 1] = (uint8_t)(rgb565 & 0xFF);
    }

    for (uint16_t line = 0; line < height; ++line) {
        ESP_RETURN_ON_ERROR(
            lcd_data(row, width * 2),
            TAG, "Rectangle pixel transfer failed");
    }

    return ESP_OK;
}

esp_err_t lcd_driver_fill(uint16_t rgb565)
{
    return lcd_driver_fill_rect(
        0, 0, LCD_WIDTH, LCD_HEIGHT, rgb565);
}

static const uint8_t *glyph_for(unsigned char character)
{
    if (character < 32 || character > 127) {
        character = '?';
    }
    return FONT_5X7[character - 32];
}

static esp_err_t draw_character(uint16_t x, uint16_t y,
                                unsigned char character,
                                uint16_t foreground,
                                uint16_t background,
                                uint8_t scale)
{
    if (scale == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    const uint8_t *glyph = glyph_for(character);

    for (uint8_t column = 0; column < 6; ++column) {
        uint8_t bits = (column < 5) ? glyph[column] : 0;

        for (uint8_t row = 0; row < 8; ++row) {
            uint16_t color =
                (column < 5 && row < 7 && (bits & (1U << row)))
                    ? foreground : background;

            ESP_RETURN_ON_ERROR(
                lcd_driver_fill_rect(
                    x + column * scale,
                    y + row * scale,
                    scale, scale, color),
                TAG, "Character rendering failed");
        }
    }

    return ESP_OK;
}

esp_err_t lcd_driver_draw_text(uint16_t x, uint16_t y,
                               const char *text,
                               uint16_t foreground,
                               uint16_t background,
                               uint8_t scale)
{
    if (text == NULL || scale == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    uint16_t cursor_x = x;
    uint16_t cursor_y = y;

    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        if (*p == '\n') {
            cursor_x = x;
            cursor_y += 8 * scale;
            continue;
        }

        if (cursor_x + 6 * scale > LCD_WIDTH ||
            cursor_y + 8 * scale > LCD_HEIGHT) {
            break;
        }

        ESP_RETURN_ON_ERROR(
            draw_character(cursor_x, cursor_y, *p,
                           foreground, background, scale),
            TAG, "Text rendering failed");

        cursor_x += 6 * scale;
    }

    return ESP_OK;
}

int lcd_driver_text_width(const char *text, uint8_t scale)
{
    if (text == NULL || scale == 0) {
        return 0;
    }
    return (int)(strlen(text) * 6U * scale);
}

esp_err_t lcd_driver_init(void)
{
    if (s_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing AtomS3R LCD hardware");

    ESP_RETURN_ON_ERROR(
        backlight_controller_init(),
        TAG, "Backlight controller initialization failed");
    ESP_RETURN_ON_ERROR(
        lcd_spi_init(),
        TAG, "LCD SPI initialization failed");

    lcd_reset();

    ESP_RETURN_ON_ERROR(
        lcd_controller_init(),
        TAG, "LCD controller initialization failed");

    s_initialized = true;

    ESP_RETURN_ON_ERROR(
        lcd_driver_fill(LCD_COLOR_BLACK),
        TAG, "Initial black frame failed");
    ESP_RETURN_ON_ERROR(
        lcd_driver_set_backlight(128),
        TAG, "Backlight enable failed");

    ESP_LOGI(TAG, "AtomS3R LCD hardware initialized");
    return ESP_OK;
}
