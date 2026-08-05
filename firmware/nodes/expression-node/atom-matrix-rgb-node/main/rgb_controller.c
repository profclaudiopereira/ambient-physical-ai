#include "rgb_controller.h"

#include <stdbool.h>

#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"

/*
 * Atom Matrix hardware configuration.
 *
 * The integrated 5 x 5 WS2812 matrix contains 25 addressable LEDs and is
 * connected internally to GPIO 27.
 *
 * Brightness is intentionally limited to reduce current consumption and avoid
 * excessive visual intensity when all matrix pixels are active simultaneously.
 */
#define RGB_GPIO       27
#define RGB_COUNT      25
#define RGB_BRIGHTNESS 170

static const char *TAG = "rgb_controller";

static led_strip_handle_t s_led_strip = NULL;
static bool s_initialized = false;

/**
 * @brief Applies the configured brightness limit to one RGB channel.
 *
 * @param value Original channel intensity in the range 0 to 255.
 *
 * @return Scaled intensity according to RGB_BRIGHTNESS.
 */
static uint8_t rgb_controller_scale(uint8_t value)
{
    return (uint8_t)(((uint16_t)value * RGB_BRIGHTNESS) / 255U);
}

int rgb_controller_init(void)
{
    if (s_initialized) {
        ESP_LOGW(TAG, "RGB controller already initialized");
        return 0;
    }

    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_GPIO,
        .max_leds = RGB_COUNT,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = {
            .invert_out = false,
        },
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 64,
        .flags = {
            .with_dma = false,
        },
    };

    esp_err_t err = led_strip_new_rmt_device(
        &strip_config,
        &rmt_config,
        &s_led_strip
    );

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to create LED matrix device: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    err = led_strip_clear(s_led_strip);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to clear LED matrix: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    s_initialized = true;

    ESP_LOGI(
        TAG,
        "RGB matrix controller initialized: GPIO=%d, LEDs=%d, brightness=%d",
        RGB_GPIO,
        RGB_COUNT,
        RGB_BRIGHTNESS
    );

    return 0;
}

int rgb_controller_set_all(
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    if (!s_initialized || s_led_strip == NULL) {
        ESP_LOGE(TAG, "RGB controller is not initialized");
        return (int)ESP_ERR_INVALID_STATE;
    }

    const uint8_t scaled_red = rgb_controller_scale(red);
    const uint8_t scaled_green = rgb_controller_scale(green);
    const uint8_t scaled_blue = rgb_controller_scale(blue);

    for (int index = 0; index < RGB_COUNT; index++) {
        esp_err_t err = led_strip_set_pixel(
            s_led_strip,
            index,
            scaled_red,
            scaled_green,
            scaled_blue
        );

        if (err != ESP_OK) {
            ESP_LOGE(
                TAG,
                "Failed to set matrix pixel %d: %s",
                index,
                esp_err_to_name(err)
            );
            return (int)err;
        }
    }

    esp_err_t err = led_strip_refresh(s_led_strip);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to refresh LED matrix: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    return 0;
}

int rgb_controller_clear(void)
{
    if (!s_initialized || s_led_strip == NULL) {
        ESP_LOGE(TAG, "RGB controller is not initialized");
        return (int)ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = led_strip_clear(s_led_strip);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to clear LED matrix: %s",
            esp_err_to_name(err)
        );
        return (int)err;
    }

    return 0;
}