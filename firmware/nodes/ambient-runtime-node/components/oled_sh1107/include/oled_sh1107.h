#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

esp_err_t oled_sh1107_init(i2c_master_bus_handle_t bus);
esp_err_t oled_sh1107_clear(void);
esp_err_t oled_sh1107_print_test(void);
esp_err_t oled_sh1107_print_ok(void);
esp_err_t oled_sh1107_print_ambient_test(void);