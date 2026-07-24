/******************************************************************************
 * @file    cognitive_runtime_console.c
 * @brief   Application entry point for the Cognitive Runtime Console.
 *
 * This module performs only the platform-level initialization required by the
 * console application. All LVGL screen construction and presentation logic
 * remain isolated in the UI subsystem.
 *
 * Architectural contract:
 *  - Hardware initialization is owned by this module.
 *  - Screen creation and navigation are owned by ui_manager.
 *  - Runtime data acquisition will be introduced in a separate subsystem.
 *
 * This separation prevents the application entry point from becoming coupled
 * to individual screens or future communication transports.
 ******************************************************************************/

#include "esp_log.h"
#include "bsp/esp-bsp.h"

#include "ui/ui_manager.h"
#include "transport/runtime_uart.h"

static const char *TAG = "cognitive_console";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting Cognitive Runtime Console");

    /*
     * The official CoreS3 BSP owns display, touch controller and LVGL
     * initialization. Application modules must not initialize these devices
     * independently, as that could create conflicting driver ownership.
     */
    bsp_display_start();
    bsp_display_backlight_on();

    /*
     * ui_manager_init() creates the initial operational screen and establishes
     * the single entry point for future navigation and screen updates.
     */
    ui_manager_init();

if (!runtime_uart_start()) {
    ESP_LOGE(TAG, "Unable to start Cognitive Runtime UART transport");
    return;
}

ESP_LOGI(TAG, "Cognitive Runtime UART transport ready");


    ESP_LOGI(TAG, "Cognitive Runtime Console initialized");
}