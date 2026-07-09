#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t dlight_read_lux(
    i2c_master_bus_handle_t bus,
    float *lux
);

#ifdef __cplusplus
}
#endif