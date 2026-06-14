#include "env_iv.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SHT40_ADDR      0x44

#define BMP280_ADDR     0x76
#define BMP280_REG_ID   0xD0
#define BMP280_REG_CTRL 0xF4
#define BMP280_REG_DATA 0xF7
#define BMP280_REG_CAL  0x88

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    int32_t  t_fine;
} bmp280_calib_t;

static uint16_t u16_le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static int16_t s16_le(const uint8_t *p)
{
    return (int16_t)u16_le(p);
}

static esp_err_t bmp280_add_device(
    i2c_master_bus_handle_t bus,
    i2c_master_dev_handle_t *dev_handle
)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP280_ADDR,
        .scl_speed_hz = 100000,
    };

    return i2c_master_bus_add_device(bus, &dev_cfg, dev_handle);
}

static esp_err_t bmp280_read_reg(
    i2c_master_dev_handle_t dev_handle,
    uint8_t reg,
    uint8_t *data,
    size_t len
)
{
    return i2c_master_transmit_receive(
        dev_handle,
        &reg,
        1,
        data,
        len,
        100
    );
}

static esp_err_t bmp280_write_reg(
    i2c_master_dev_handle_t dev_handle,
    uint8_t reg,
    uint8_t value
)
{
    uint8_t data[2] = {reg, value};
    return i2c_master_transmit(dev_handle, data, sizeof(data), 100);
}

esp_err_t env_iv_sht40_read(
    i2c_master_bus_handle_t bus,
    env_iv_sht40_data_t *data
)
{
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_dev_handle_t dev_handle;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT40_ADDR,
        .scl_speed_hz = 100000,
    };

    esp_err_t ret = i2c_master_bus_add_device(bus, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t cmd = 0xFD;

    ret = i2c_master_transmit(dev_handle, &cmd, 1, 100);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(dev_handle);
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t raw[6] = {0};

    ret = i2c_master_receive(dev_handle, raw, sizeof(raw), 100);

    i2c_master_bus_rm_device(dev_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    uint16_t raw_temp = ((uint16_t)raw[0] << 8) | raw[1];
    uint16_t raw_hum  = ((uint16_t)raw[3] << 8) | raw[4];

    data->temperature_c =
        -45.0f + 175.0f * ((float)raw_temp / 65535.0f);

    data->humidity_percent =
        -6.0f + 125.0f * ((float)raw_hum / 65535.0f);

    if (data->humidity_percent < 0.0f) {
        data->humidity_percent = 0.0f;
    }

    if (data->humidity_percent > 100.0f) {
        data->humidity_percent = 100.0f;
    }

    return ESP_OK;
}

esp_err_t env_iv_bmp280_read_id(
    i2c_master_bus_handle_t bus,
    uint8_t *chip_id
)
{
    if (chip_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_dev_handle_t dev_handle;

    esp_err_t ret = bmp280_add_device(bus, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = bmp280_read_reg(dev_handle, BMP280_REG_ID, chip_id, 1);

    i2c_master_bus_rm_device(dev_handle);

    return ret;
}

static esp_err_t bmp280_read_calibration(
    i2c_master_dev_handle_t dev_handle,
    bmp280_calib_t *calib
)
{
    uint8_t raw[24] = {0};

    esp_err_t ret = bmp280_read_reg(
        dev_handle,
        BMP280_REG_CAL,
        raw,
        sizeof(raw)
    );

    if (ret != ESP_OK) {
        return ret;
    }

    calib->dig_T1 = u16_le(&raw[0]);
    calib->dig_T2 = s16_le(&raw[2]);
    calib->dig_T3 = s16_le(&raw[4]);

    calib->dig_P1 = u16_le(&raw[6]);
    calib->dig_P2 = s16_le(&raw[8]);
    calib->dig_P3 = s16_le(&raw[10]);
    calib->dig_P4 = s16_le(&raw[12]);
    calib->dig_P5 = s16_le(&raw[14]);
    calib->dig_P6 = s16_le(&raw[16]);
    calib->dig_P7 = s16_le(&raw[18]);
    calib->dig_P8 = s16_le(&raw[20]);
    calib->dig_P9 = s16_le(&raw[22]);

    return ESP_OK;
}

static float bmp280_compensate_temperature(
    bmp280_calib_t *calib,
    int32_t adc_T
)
{
    int32_t var1 =
        ((((adc_T >> 3) - ((int32_t)calib->dig_T1 << 1))) *
         ((int32_t)calib->dig_T2)) >> 11;

    int32_t var2 =
        (((((adc_T >> 4) - ((int32_t)calib->dig_T1)) *
           ((adc_T >> 4) - ((int32_t)calib->dig_T1))) >> 12) *
         ((int32_t)calib->dig_T3)) >> 14;

    calib->t_fine = var1 + var2;

    float temperature =
        (float)((calib->t_fine * 5 + 128) >> 8) / 100.0f;

    return temperature;
}

static float bmp280_compensate_pressure(
    bmp280_calib_t *calib,
    int32_t adc_P
)
{
    int64_t var1 = ((int64_t)calib->t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)calib->dig_P6;

    var2 = var2 + ((var1 * (int64_t)calib->dig_P5) << 17);
    var2 = var2 + (((int64_t)calib->dig_P4) << 35);

    var1 =
        ((var1 * var1 * (int64_t)calib->dig_P3) >> 8) +
        ((var1 * (int64_t)calib->dig_P2) << 12);

    var1 =
        (((((int64_t)1) << 47) + var1)) *
        ((int64_t)calib->dig_P1) >> 33;

    if (var1 == 0) {
        return 0.0f;
    }

    int64_t p = 1048576 - adc_P;

    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)calib->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib->dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)calib->dig_P7) << 4);

    return ((float)p / 256.0f) / 100.0f;
}

esp_err_t env_iv_bmp280_read(
    i2c_master_bus_handle_t bus,
    env_iv_bmp280_data_t *data
)
{
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_dev_handle_t dev_handle;

    esp_err_t ret = bmp280_add_device(bus, &dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t chip_id = 0;
    ret = bmp280_read_reg(dev_handle, BMP280_REG_ID, &chip_id, 1);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(dev_handle);
        return ret;
    }

    if (chip_id != 0x58) {
        i2c_master_bus_rm_device(dev_handle);
        return ESP_ERR_INVALID_RESPONSE;
    }

    bmp280_calib_t calib = {0};

    ret = bmp280_read_calibration(dev_handle, &calib);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(dev_handle);
        return ret;
    }

    ret = bmp280_write_reg(dev_handle, BMP280_REG_CTRL, 0x27);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(dev_handle);
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    uint8_t raw[6] = {0};

    ret = bmp280_read_reg(dev_handle, BMP280_REG_DATA, raw, sizeof(raw));

    i2c_master_bus_rm_device(dev_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    int32_t adc_P =
        ((int32_t)raw[0] << 12) |
        ((int32_t)raw[1] << 4) |
        ((int32_t)raw[2] >> 4);

    int32_t adc_T =
        ((int32_t)raw[3] << 12) |
        ((int32_t)raw[4] << 4) |
        ((int32_t)raw[5] >> 4);

    data->temperature_c =
        bmp280_compensate_temperature(&calib, adc_T);

    data->pressure_hpa =
        bmp280_compensate_pressure(&calib, adc_P);

    return ESP_OK;
}