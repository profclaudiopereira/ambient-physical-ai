#ifndef WS1850S_H
#define WS1850S_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WS1850S_I2C_ADDRESS 0x28

esp_err_t ws1850s_probe(i2c_master_dev_handle_t dev_handle);
esp_err_t ws1850s_read_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t *value);
esp_err_t ws1850s_write_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t value);
esp_err_t ws1850s_set_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask);
esp_err_t ws1850s_clear_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask);

esp_err_t ws1850s_read_version(i2c_master_dev_handle_t dev_handle, uint8_t *version);
esp_err_t ws1850s_init(i2c_master_dev_handle_t dev_handle);
esp_err_t ws1850s_card_present(i2c_master_dev_handle_t dev_handle, bool *present);
esp_err_t ws1850s_read_uid(i2c_master_dev_handle_t dev_handle, uint8_t *uid, uint8_t *uid_len);

#ifdef __cplusplus
}
#endif

#endif
