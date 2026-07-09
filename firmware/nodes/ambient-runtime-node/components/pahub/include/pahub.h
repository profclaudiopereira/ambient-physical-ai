#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t pahub_select_channel(
    i2c_master_bus_handle_t bus,
    uint8_t channel
);

#ifdef __cplusplus
}
#endif