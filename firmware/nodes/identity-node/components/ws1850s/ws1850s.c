#include "ws1850s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

#define COMMAND_REG        0x01
#define COM_IRQ_REG        0x04
#define ERROR_REG          0x06
#define FIFO_DATA_REG      0x09
#define FIFO_LEVEL_REG     0x0A
#define CONTROL_REG        0x0C
#define BIT_FRAMING_REG    0x0D
#define COLL_REG           0x0E
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
#define PICC_CMD_SEL_CL1   0x93

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
    if (ret != ESP_OK) return ret;

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

static esp_err_t ws1850s_transceive(
    i2c_master_dev_handle_t dev_handle,
    const uint8_t *send_data,
    uint8_t send_len,
    uint8_t *back_data,
    uint8_t *back_len,
    uint8_t *valid_bits)
{
    if (send_data == NULL || send_len == 0 || back_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;

    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_IDLE);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_write_register(dev_handle, COM_IRQ_REG, 0x7F);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_set_bit_mask(dev_handle, FIFO_LEVEL_REG, 0x80);
    if (ret != ESP_OK) return ret;

    for (uint8_t i = 0; i < send_len; i++) {
        ret = ws1850s_write_register(dev_handle, FIFO_DATA_REG, send_data[i]);
        if (ret != ESP_OK) return ret;
    }

    ret = ws1850s_write_register(dev_handle, COMMAND_REG, PCD_TRANSCEIVE);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_set_bit_mask(dev_handle, BIT_FRAMING_REG, 0x80);
    if (ret != ESP_OK) return ret;

    for (int i = 0; i < 100; i++) {
        uint8_t irq = 0;
        ret = ws1850s_read_register(dev_handle, COM_IRQ_REG, &irq);
        if (ret != ESP_OK) return ret;

        if (irq & 0x30) {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(2));
    }

    ws1850s_clear_bit_mask(dev_handle, BIT_FRAMING_REG, 0x80);

    uint8_t error = 0;
    ret = ws1850s_read_register(dev_handle, ERROR_REG, &error);
    if (ret != ESP_OK) return ret;

    if (error & 0x13) {
        return ESP_FAIL;
    }

    uint8_t fifo_level = 0;
    ret = ws1850s_read_register(dev_handle, FIFO_LEVEL_REG, &fifo_level);
    if (ret != ESP_OK) return ret;

    uint8_t control = 0;
    ret = ws1850s_read_register(dev_handle, CONTROL_REG, &control);
    if (ret != ESP_OK) return ret;

    if (valid_bits != NULL) {
        *valid_bits = control & 0x07;
    }

    uint8_t count = fifo_level;

    if (back_data != NULL) {
        if (count > *back_len) {
            count = *back_len;
        }

        for (uint8_t i = 0; i < count; i++) {
            ret = ws1850s_read_register(dev_handle, FIFO_DATA_REG, &back_data[i]);
            if (ret != ESP_OK) return ret;
        }
    }

    *back_len = count;

    printf("TRANSCEIVE DEBUG: send_len=%u fifo=%u valid_bits=%u data=",
           send_len,
           *back_len,
           valid_bits ? *valid_bits : 0);

    if (back_data != NULL) {
        for (int i = 0; i < *back_len; i++) {
            printf("%02X", back_data[i]);
        }
    }

    printf("\n");

    return ESP_OK;
}

esp_err_t ws1850s_card_present(i2c_master_dev_handle_t dev_handle, bool *present)
{
    if (present == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *present = false;

    uint8_t send_data[1] = {PICC_CMD_REQA};
    uint8_t back_data[2] = {0};
    uint8_t back_len = sizeof(back_data);
    uint8_t valid_bits = 0;

    esp_err_t ret = ws1850s_write_register(dev_handle, BIT_FRAMING_REG, 0x07);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_transceive(
        dev_handle,
        send_data,
        sizeof(send_data),
        back_data,
        &back_len,
        &valid_bits);

    if (ret != ESP_OK) {
        return ret;
    }

    if (back_len == 2 && valid_bits == 0) {
        *present = true;
    }

    return ESP_OK;
}

esp_err_t ws1850s_read_uid(i2c_master_dev_handle_t dev_handle, uint8_t *uid, uint8_t *uid_len)
{
    if (uid == NULL || uid_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *uid_len = 0;

    esp_err_t ret = ws1850s_init(dev_handle);
    if (ret != ESP_OK) return ret;

    bool present = false;
    ret = ws1850s_card_present(dev_handle, &present);
    if (ret != ESP_OK) return ret;

    if (!present) {
        return ESP_ERR_NOT_FOUND;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    ret = ws1850s_write_register(dev_handle, COLL_REG, 0x80);
    if (ret != ESP_OK) return ret;

    uint8_t send_data[2] = {PICC_CMD_SEL_CL1, 0x20};
    uint8_t back_data[10] = {0};
    uint8_t back_len = sizeof(back_data);
    uint8_t valid_bits = 0;

    ret = ws1850s_write_register(dev_handle, BIT_FRAMING_REG, 0x00);
    if (ret != ESP_OK) return ret;

    ret = ws1850s_transceive(
        dev_handle,
        send_data,
        sizeof(send_data),
        back_data,
        &back_len,
        &valid_bits);

    printf("UID DEBUG: ret=%s back_len=%u valid_bits=%u data=",
           esp_err_to_name(ret),
           back_len,
           valid_bits);

    for (int i = 0; i < back_len; i++) {
        printf("%02X", back_data[i]);
    }

    printf("\n");

    if (ret != ESP_OK) {
        return ret;
    }

    if (back_len < 5) {
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t bcc = back_data[0] ^ back_data[1] ^ back_data[2] ^ back_data[3];

    if (bcc != back_data[4]) {
        return ESP_ERR_INVALID_CRC;
    }

    uid[0] = back_data[0];
    uid[1] = back_data[1];
    uid[2] = back_data[2];
    uid[3] = back_data[3];
    *uid_len = 4;

    return ESP_OK;
}