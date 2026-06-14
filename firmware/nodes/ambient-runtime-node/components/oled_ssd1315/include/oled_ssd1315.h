#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

esp_err_t oled_ssd1315_init(i2c_master_bus_handle_t bus);
esp_err_t oled_ssd1315_clear(void);
esp_err_t oled_ssd1315_print_demo(void);
esp_err_t oled_ssd1315_print_status(void);