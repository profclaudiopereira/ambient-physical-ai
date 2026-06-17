#include "ws1850s.h"

esp_err_t ws1850s_probe(i2c_master_dev_handle_t dev_handle)
{
    uint8_t dummy = 0x00;

    return i2c_master_transmit(
        dev_handle,
        &dummy,
        1,
        1000
    );
}