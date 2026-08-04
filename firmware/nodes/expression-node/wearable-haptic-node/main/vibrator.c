#include "vibrator.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define VIB_PIN GPIO_NUM_26

static void vibrator_configure_output(void)
{
    gpio_hold_dis(VIB_PIN);
    gpio_reset_pin(VIB_PIN);

    gpio_config_t config = {
        .pin_bit_mask = (1ULL << VIB_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&config);
}

void vibrator_off(void)
{
    /*
     * Reassert the GPIO configuration because another board initialization
     * stage may have changed the pin configuration.
     */
    vibrator_configure_output();
    gpio_set_level(VIB_PIN, 0);
}

void vibrator_init(void)
{
    /*
     * Reproduce the hardware transition that was validated experimentally.
     * The final and persistent state is LOW: motor disabled.
     */
    vibrator_configure_output();

    gpio_set_level(VIB_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(50));

    gpio_set_level(VIB_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    gpio_set_level(VIB_PIN, 0);
}

void vibrator_alert(void)
{
    vibrator_configure_output();

    gpio_set_level(VIB_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(200));

    gpio_set_level(VIB_PIN, 0);
}