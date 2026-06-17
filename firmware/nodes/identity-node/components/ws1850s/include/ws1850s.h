#ifndef WS1850S_H
#define WS1850S_H

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WS1850S_I2C_ADDRESS 0x28

esp_err_t ws1850s_probe(i2c_master_dev_handle_t dev_handle);

#ifdef __cplusplus
}
#endif

#endif