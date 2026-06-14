#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

typedef struct {
    float temperature_c;
    float humidity_percent;
} env_iv_sht40_data_t;

typedef struct {
    float temperature_c;
    float pressure_hpa;
} env_iv_bmp280_data_t;

esp_err_t env_iv_sht40_read(
    i2c_master_bus_handle_t bus,
    env_iv_sht40_data_t *data
);

esp_err_t env_iv_bmp280_read_id(
    i2c_master_bus_handle_t bus,
    uint8_t *chip_id
);

esp_err_t env_iv_bmp280_read(
    i2c_master_bus_handle_t bus,
    env_iv_bmp280_data_t *data
);