#ifndef VL53L0X_H
#define VL53L0X_H

#include "esp_err.h"
#include "driver/i2c_master.h"

#define VL53L0X_I2C_ADDRESS 0x29

esp_err_t vl53l0x_is_alive(i2c_master_dev_handle_t dev_handle);

#endif