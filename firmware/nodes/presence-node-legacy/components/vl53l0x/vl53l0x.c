#include "vl53l0x.h"

esp_err_t vl53l0x_is_alive(i2c_master_dev_handle_t dev_handle)
{
    uint8_t reg = 0xC0;
    uint8_t value = 0;

    return i2c_master_transmit_receive(
        dev_handle,
        &reg,
        1,
        &value,
        1,
        1000
    );
}