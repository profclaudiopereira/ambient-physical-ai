#include "rgb_controller.h"

#include <stdbool.h>

#include "M5Unified.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"

/*
 * Hardware configuration inherited from the validated
 * stickc-plus2-ws2812-status laboratory firmware.
 */
#define RGB_GPIO   32
#define RGB_COUNT  8

static const char *TAG = "stickc_rgb_controller";

static led_strip_handle_t s_led_strip = nullptr;
static bool s_initialized = false;

/**
 * @brief Updates the StickC Plus 2 display with the commanded RGB value.
 *
 * The display belongs to the hardware presentation layer. Keeping its
 * implementation inside this controller prevents semantic and networking
 * modules from depending on M5Unified.
 */
static void update_display(
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    const uint16_t background =
        M5.Display.color565(red, green, blue);

    M5.Display.fillScreen(background);
    M5.Display.setTextColor(WHITE, background);
    M5.Display.setCursor(10, 10);
    M5.Display.setTextSize(2);

   M5.Display.setTextFont(1);
   M5.Display.setCursor(10, 10);
   M5.Display.println("RGB NODE");
}

int rgb_controller_init(void)
{
    if (s_initialized) {
        ESP_LOGW(TAG, "RGB controller already initialized");
        return 0;
    }

    /*
     * M5Unified initializes the StickC display and the board services
     * required by the validated laboratory implementation.
     */
    M5.begin();
    M5.Display.setRotation(0);

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
            "Failed to create WS2812 device: %s",
            esp_err_to_name(err)
        );
        return static_cast<int>(err);
    }

    err = led_strip_clear(s_led_strip);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to clear WS2812 output: %s",
            esp_err_to_name(err)
        );
        return static_cast<int>(err);
    }

    update_display(0, 0, 0);

    /*
     * The short stabilization delay is preserved from the validated
     * laboratory firmware before the first visual command is issued.
     */
    vTaskDelay(pdMS_TO_TICKS(50));

    s_initialized = true;

    ESP_LOGI(
        TAG,
        "StickC RGB controller initialized: GPIO=%d, LEDs=%d",
        RGB_GPIO,
        RGB_COUNT
    );

    return 0;
}

int rgb_controller_set_all(
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    if (!s_initialized || s_led_strip == nullptr) {
        ESP_LOGE(TAG, "RGB controller is not initialized");
        return static_cast<int>(ESP_ERR_INVALID_STATE);
    }

    for (int index = 0; index < RGB_COUNT; index++) {
        esp_err_t err = led_strip_set_pixel(
            s_led_strip,
            index,
            red,
            green,
            blue
        );

        if (err != ESP_OK) {
            ESP_LOGE(
                TAG,
                "Failed to set pixel %d: %s",
                index,
                esp_err_to_name(err)
            );
            return static_cast<int>(err);
        }
    }

    esp_err_t err = led_strip_refresh(s_led_strip);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to refresh WS2812 output: %s",
            esp_err_to_name(err)
        );
        return static_cast<int>(err);
    }

    update_display(red, green, blue);

    return 0;
}

int rgb_controller_clear(void)
{
    if (!s_initialized || s_led_strip == nullptr) {
        ESP_LOGE(TAG, "RGB controller is not initialized");
        return static_cast<int>(ESP_ERR_INVALID_STATE);
    }

    esp_err_t err = led_strip_clear(s_led_strip);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to clear WS2812 output: %s",
            esp_err_to_name(err)
        );
        return static_cast<int>(err);
    }

    update_display(0, 0, 0);

    return 0;
}