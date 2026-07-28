/**
 * @file display_manager.c
 * @brief Local operational UI for the Echo Pyramid Voice Node.
 *
 * This V1 replaces the temporary color bring-up sequence with four logical
 * states: BOOT, READY, WELCOME and ERROR. It remains synchronous and small so
 * that wake-word and voice-command work can begin immediately after validation.
 */

#include "display_manager.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_check.h"
#include "esp_log.h"

#include "lcd_driver.h"

static const char *TAG = "display_manager";
static bool s_initialized = false;

static esp_err_t draw_centered(uint16_t y,
                               const char *text,
                               uint16_t color,
                               uint8_t scale)
{
    int width = lcd_driver_text_width(text, scale);
    uint16_t x = (width < LCD_WIDTH)
                     ? (uint16_t)((LCD_WIDTH - width) / 2)
                     : 0;

    return lcd_driver_draw_text(
        x, y, text, color, LCD_COLOR_BLACK, scale);
}

esp_err_t display_manager_init(void)
{
    ESP_LOGI(TAG, "Starting Local Status Display V1");

    esp_err_t err = lcd_driver_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LCD initialization failed: %s",
                 esp_err_to_name(err));
        return err;
    }

    s_initialized = true;
    ESP_LOGI(TAG, "Local Status Display V1 initialized");
    return ESP_OK;
}

esp_err_t display_show_boot(void)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(
        lcd_driver_fill(LCD_COLOR_BLACK),
        TAG, "Failed to clear BOOT screen");
    ESP_RETURN_ON_ERROR(
        draw_centered(18, "AMBIENT", LCD_COLOR_CYAN, 2),
        TAG, "Failed to draw BOOT title");
    ESP_RETURN_ON_ERROR(
        draw_centered(38, "PHYSICAL AI", LCD_COLOR_CYAN, 1),
        TAG, "Failed to draw project name");
    ESP_RETURN_ON_ERROR(
        draw_centered(68, "EXPRESSION NODE", LCD_COLOR_WHITE, 1),
        TAG, "Failed to draw node name");
    ESP_RETURN_ON_ERROR(
        draw_centered(98, "INITIALIZING...", LCD_COLOR_YELLOW, 1),
        TAG, "Failed to draw BOOT state");

    ESP_LOGI(TAG, "Display state: BOOT");
    return ESP_OK;
}

esp_err_t display_show_ready(const char *ip_address)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    const char *ip =
        (ip_address != NULL && ip_address[0] != '\0')
            ? ip_address : "0.0.0.0";

    ESP_RETURN_ON_ERROR(
        lcd_driver_fill(LCD_COLOR_BLACK),
        TAG, "Failed to clear READY screen");
    ESP_RETURN_ON_ERROR(
        draw_centered(8, "EXPRESSION NODE", LCD_COLOR_CYAN, 1),
        TAG, "Failed to draw READY title");
    ESP_RETURN_ON_ERROR(
        lcd_driver_draw_text(
            8, 31, "WI-FI: CONNECTED",
            LCD_COLOR_WHITE, LCD_COLOR_BLACK, 1),
        TAG, "Failed to draw Wi-Fi status");
    ESP_RETURN_ON_ERROR(
        lcd_driver_draw_text(
            8, 49, "IP:",
            LCD_COLOR_GRAY, LCD_COLOR_BLACK, 1),
        TAG, "Failed to draw IP label");
    ESP_RETURN_ON_ERROR(
        draw_centered(62, ip, LCD_COLOR_WHITE, 1),
        TAG, "Failed to draw IP address");
    ESP_RETURN_ON_ERROR(
        lcd_driver_draw_text(
            8, 82, "UDP: LISTENING",
            LCD_COLOR_WHITE, LCD_COLOR_BLACK, 1),
        TAG, "Failed to draw UDP status");
    ESP_RETURN_ON_ERROR(
        draw_centered(105, "READY", LCD_COLOR_GREEN, 2),
        TAG, "Failed to draw READY state");

    ESP_LOGI(TAG, "Display state: READY | IP=%s", ip);
    return ESP_OK;
}

esp_err_t display_show_welcome(const char *user_name)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    const char *name =
        (user_name != NULL && user_name[0] != '\0')
            ? user_name : "USER";

    char safe_name[20] = {0};
    snprintf(safe_name, sizeof(safe_name), "%.19s", name);

    ESP_RETURN_ON_ERROR(
        lcd_driver_fill(LCD_COLOR_BLACK),
        TAG, "Failed to clear WELCOME screen");
    ESP_RETURN_ON_ERROR(
        draw_centered(20, "WELCOME", LCD_COLOR_GREEN, 2),
        TAG, "Failed to draw WELCOME title");
    ESP_RETURN_ON_ERROR(
        draw_centered(58, safe_name, LCD_COLOR_WHITE, 1),
        TAG, "Failed to draw authenticated user");
    ESP_RETURN_ON_ERROR(
        draw_centered(86, "EXPRESSION LAYER", LCD_COLOR_CYAN, 1),
        TAG, "Failed to draw layer label");

    ESP_LOGI(TAG, "Display state: WELCOME | user=%s", safe_name);
    return ESP_OK;
}

esp_err_t display_show_error(const char *message)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    const char *text =
        (message != NULL && message[0] != '\0')
            ? message : "UNKNOWN ERROR";

    char safe_message[20] = {0};
    snprintf(safe_message, sizeof(safe_message), "%.19s", text);

    ESP_RETURN_ON_ERROR(
        lcd_driver_fill(LCD_COLOR_BLACK),
        TAG, "Failed to clear ERROR screen");
    ESP_RETURN_ON_ERROR(
        draw_centered(24, "ERROR", LCD_COLOR_RED, 2),
        TAG, "Failed to draw ERROR title");
    ESP_RETURN_ON_ERROR(
        draw_centered(68, safe_message, LCD_COLOR_WHITE, 1),
        TAG, "Failed to draw ERROR message");

    ESP_LOGI(TAG, "Display state: ERROR | message=%s", safe_message);
    return ESP_OK;
}
