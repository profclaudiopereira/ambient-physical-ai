#include "rgb_controller.h"

#include <stdbool.h>
#include <stdint.h>

#include "M5Unified.h"
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "wearable_display_controller";

static bool s_initialized = false;


/**
 * @brief Selects a readable text color for the current background.
 *
 * Bright backgrounds use black text. Dark backgrounds use white text.
 */
static uint16_t select_text_color(
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    /*
     * Use 32 bits because the weighted sum can exceed uint16_t.
     *
     * Maximum:
     * 255 * (299 + 587 + 114) = 255000
     */
    const uint32_t luminance =
        static_cast<uint32_t>(red) * 299U +
        static_cast<uint32_t>(green) * 587U +
        static_cast<uint32_t>(blue) * 114U;

    return luminance >= 128000U ? BLACK : WHITE;
}


/**
 * @brief Updates the StickS3 display background and optional label.
 */
static void update_display(
    const char *label,
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    const uint16_t background =
        M5.Display.color565(red, green, blue);

    const uint16_t foreground =
        select_text_color(red, green, blue);

    M5.Display.fillScreen(background);

    if (label == nullptr || label[0] == '\0') {
        return;
    }

    M5.Display.setTextColor(foreground, background);
    M5.Display.setTextFont(1);
    M5.Display.setTextSize(2);
    M5.Display.setTextDatum(middle_center);

    M5.Display.drawString(
        label,
        M5.Display.width() / 2,
        M5.Display.height() / 2
    );
}


int rgb_controller_init(void)
{
    if (s_initialized) {
        ESP_LOGW(TAG, "Display controller already initialized");
        return 0;
    }

    auto cfg = M5.config();
    M5.begin(cfg);

    /*
     * Rotation inherited from the validated wearable firmware.
     */
    M5.Display.setRotation(1);

    update_display(nullptr, 0, 0, 0);

    vTaskDelay(pdMS_TO_TICKS(50));

    s_initialized = true;

    ESP_LOGI(TAG, "StickS3 display controller initialized");

    return 0;
}


int rgb_controller_present_state(
    const char *label,
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Display controller is not initialized");
        return static_cast<int>(ESP_ERR_INVALID_STATE);
    }

    if (label == nullptr || label[0] == '\0') {
        ESP_LOGE(TAG, "Runtime State label is invalid");
        return static_cast<int>(ESP_ERR_INVALID_ARG);
    }

    update_display(
        label,
        red,
        green,
        blue
    );

    return 0;
}


int rgb_controller_set_all(
    uint8_t red,
    uint8_t green,
    uint8_t blue
)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Display controller is not initialized");
        return static_cast<int>(ESP_ERR_INVALID_STATE);
    }

    update_display(
        nullptr,
        red,
        green,
        blue
    );

    return 0;
}


int rgb_controller_clear(void)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Display controller is not initialized");
        return static_cast<int>(ESP_ERR_INVALID_STATE);
    }

    update_display(nullptr, 0, 0, 0);

    return 0;
}