#include "ws1850s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define COMMAND_REG        0x01
#define COM_IRQ_REG        0x04
#define ERROR_REG          0x06
#define FIFO_DATA_REG      0x09
#define FIFO_LEVEL_REG     0x0A
#define CONTROL_REG        0x0C
#define BIT_FRAMING_REG    0x0D
#define MODE_REG           0x11
#define TX_CONTROL_REG     0x14
#define TX_AUTO_REG        0x15
#define T_MODE_REG         0x2A
#define T_PRESCALER_REG    0x2B
#define T_RELOAD_REG_H     0x2C
#define T_RELOAD_REG_L     0x2D
#define VERSION_REG        0x37

#define PCD_IDLE           0x00
#define PCD_TRANSCEIVE     0x0C
#define PCD_SOFTRESET      0x0F

#define PICC_CMD_REQA      0x26

esp_err_t ws1850s_probe(i2c_master_dev_handle_t dev_handle)
{
    uint8_t dummy = 0x00;
    return i2c_master_transmit(dev_handle, &dummy, 1, 1000);
}

esp_err_t ws1850s_read_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t *value)
{
    if (value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_transmit_receive(dev_handle, &reg, 1, value, 1, 1000);
}

esp_err_t ws1850s_write_register(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    return i2c_master_transmit(dev_handle, data, sizeof(data), 1000);
}

esp_err_t ws1850s_set_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask)
{
    uint8_t value = 0;
    esp_err_t ret = ws1850s_read_register(dev_handle, reg, &value);
    if (ret != ESP_OK) {
        return ret;
    }

    return ws1850s_write_register(dev_handle, reg, value | mask);
}

esp_err_t ws1850s_clear_bit_mask(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t mask)
{
    uint8_t value = 0;
    esp_err_t ret = ws1850s_read_register(dev_handle, reg, &value);
    if (ret != ESP_OK) {
        return ret;
    }

    return ws1850s_write_register(dev_handle, reg, value & (~mask));
}

esp_err_t ws1850s_read_version(i2c_master_dev_handle_t dev_handle, uint8_t *version)
{
    return ws1850s_read_register(dev_handle, VERSION_REG, version);
}

esp_err_t ws1850s_init(i2c_master_dev_handle_t dev_handle)
{
    esp_err_t ret;

    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_SOFTRESET);
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    ret = ws1850s_write_register(dev_handle, T_MODE_REG, 0x8D);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, T_PRESCALER_REG, 0x3E);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, T_RELOAD_REG_L, 30);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, T_RELOAD_REG_H, 0);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, TX_AUTO_REG, 0x40);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, MODE_REG, 0x3D);
    if (ret != ESP_OK) return ret;

    return ws1850s_set_bit_mask(dev_handle, TX_CONTROL_REG, 0x03);
}

esp_err_t ws1850s_card_present(i2c_master_dev_handle_t dev_handle, bool *present)
{
    if (present == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *present = false;

    esp_err_t ret;

    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_IDLE);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, COM_IRQ_REG, 0x7F);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_set_bit_mask(dev_handle, FIFO_LEVEL_REG, 0x80);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, FIFO_DATA_REG, PICC_CMD_REQA);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, BIT_FRAMING_REG, 0x07);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_TRANSCEIVE);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_set_bit_mask(dev_handle, BIT_FRAMING_REG, 0x80);
    if (ret != ESP_OK) return ret;

    for (int i = 0; i < 50; i++) {
        uint8_t irq = 0;
        ret = ws1850s_read_register(dev_handle, COM_IRQ_REG, &irq);
        if (ret != ESP_OK) return ret;

        if (irq & 0x30) {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(2));
    }

    uint8_t error = 0;
    ret = ws1850s_read_register(dev_handle, ERROR_REG, &error);
    if (ret != ESP_OK) return ret;

    if (error & 0x13) {
        return ESP_OK;
    }

    uint8_t fifo_level = 0;
    ret = ws1850s_read_register(dev_handle, FIFO_LEVEL_REG, &fifo_level);
    if (ret != ESP_OK) return ret;

    uint8_t control = 0;
    ret = ws1850s_read_register(dev_handle, CONTROL_REG, &control);
    if (ret != ESP_OK) return ret;

    uint8_t valid_bits = control & 0x07;

    if (fifo_level == 2 && valid_bits == 0) {
        *present = true;
    }

    return ESP_OK;
}