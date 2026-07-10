#include "tab5_platform.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_ldo_regulator.h"

#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_st7121.h"

static const char *TAG = "tab5-platform";

#define TAB5_I2C_NUM 0
#define TAB5_I2C_SDA GPIO_NUM_31
#define TAB5_I2C_SCL GPIO_NUM_32

#define I2C_DEV_ADDR_PI4IOE1 0x43
#define I2C_MASTER_TIMEOUT_MS 50

#define PI4IO_REG_CHIP_RESET 0x01
#define PI4IO_REG_IO_DIR     0x03
#define PI4IO_REG_OUT_SET    0x05
#define PI4IO_REG_OUT_H_IM   0x07
#define PI4IO_REG_PULL_EN    0x0B
#define PI4IO_REG_PULL_SEL   0x0D

#define TAB5_LCD_BACKLIGHT GPIO_NUM_22
#define LCD_LEDC_CH LEDC_CHANNEL_1

#define TAB5_LCD_W 720
#define TAB5_LCD_H 1280

#define SET_BIT(x, y) ((x) |= (uint8_t)(0x01U << (y)))
#define CLR_BIT(x, y) ((x) &= (uint8_t)~(0x01U << (y)))

static i2c_master_bus_handle_t tab5_i2c_bus = nullptr;
static i2c_master_dev_handle_t pi4ioe1 = nullptr;

static esp_ldo_channel_handle_t phy_pwr_chan = nullptr;
static esp_lcd_dsi_bus_handle_t dsi_bus = nullptr;
static esp_lcd_panel_io_handle_t dbi_io = nullptr;
static esp_lcd_panel_handle_t panel = nullptr;
static i2c_master_bus_handle_t port_a_i2c_bus = nullptr;


static bool platform_initialized = false;
static esp_err_t tab5_port_a_i2c_init(void);

static esp_err_t tab5_i2c_init(void)
{
    ESP_LOGI(TAG, "init internal I2C bus SDA=%d SCL=%d", TAB5_I2C_SDA, TAB5_I2C_SCL);

    i2c_master_bus_config_t cfg = {};
    cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    cfg.sda_io_num = TAB5_I2C_SDA;
    cfg.scl_io_num = TAB5_I2C_SCL;
    cfg.i2c_port = TAB5_I2C_NUM;
    cfg.flags.enable_internal_pullup = true;

    return i2c_new_master_bus(&cfg, &tab5_i2c_bus);
}

static esp_err_t tab5_pi4ioe1_init(void)
{
    ESP_LOGI(TAG, "init PI4IOE1 at I2C address 0x%02X", I2C_DEV_ADDR_PI4IOE1);

    uint8_t write_buf[2] = {0};

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = I2C_DEV_ADDR_PI4IOE1;
    dev_cfg.scl_speed_hz = 400000;

    ESP_RETURN_ON_ERROR(
        i2c_master_bus_add_device(tab5_i2c_bus, &dev_cfg, &pi4ioe1),
        TAG,
        "add PI4IOE1 failed"
    );

    write_buf[0] = PI4IO_REG_CHIP_RESET;
    write_buf[1] = 0xFF;
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "PI4IOE1 reset failed");

    write_buf[0] = PI4IO_REG_IO_DIR;
    write_buf[1] = 0b01111111;
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "PI4IOE1 dir failed");

    write_buf[0] = PI4IO_REG_OUT_H_IM;
    write_buf[1] = 0b00000000;
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "PI4IOE1 hi-z failed");

    write_buf[0] = PI4IO_REG_PULL_SEL;
    write_buf[1] = 0b01111111;
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "PI4IOE1 pull select failed");

    write_buf[0] = PI4IO_REG_PULL_EN;
    write_buf[1] = 0b01111111;
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "PI4IOE1 pull enable failed");

    // P1=SPK_EN, P2=EXT5V_EN, P4=LCD_RST, P5=TP_RST, P6=CAM_RST high.
    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = 0b01110110;
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "PI4IOE1 output set failed");

    return ESP_OK;
}

static esp_err_t tab5_reset_lcd_touch(void)
{
    ESP_LOGI(TAG, "reset LCD_RST/TP_RST via PI4IOE1");

    gpio_reset_pin(GPIO_NUM_23);

    uint8_t write_buf[2] = {0};
    uint8_t read_buf[1] = {0};

    write_buf[0] = PI4IO_REG_OUT_SET;
    ESP_RETURN_ON_ERROR(
        i2c_master_transmit_receive(pi4ioe1, write_buf, 1, read_buf, 1, I2C_MASTER_TIMEOUT_MS),
        TAG,
        "PI4IOE1 read output state failed"
    );

    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = read_buf[0];
    CLR_BIT(write_buf[1], 4);
    CLR_BIT(write_buf[1], 5);
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "LCD/TP reset low failed");

    vTaskDelay(pdMS_TO_TICKS(100));

    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = read_buf[0];
    SET_BIT(write_buf[1], 4);
    SET_BIT(write_buf[1], 5);
    ESP_RETURN_ON_ERROR(i2c_master_transmit(pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS), TAG, "LCD/TP reset high failed");

    vTaskDelay(pdMS_TO_TICKS(100));

    return ESP_OK;
}

static esp_err_t tab5_backlight_init(void)
{
    ESP_LOGI(TAG, "init LCD backlight PWM on GPIO%d", TAB5_LCD_BACKLIGHT);

    const ledc_timer_config_t lcd_backlight_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ESP_RETURN_ON_ERROR(ledc_timer_config(&lcd_backlight_timer), TAG, "backlight LEDC timer config failed");

    const ledc_channel_config_t lcd_backlight_channel = {
        .gpio_num = TAB5_LCD_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LCD_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };

    ESP_RETURN_ON_ERROR(ledc_channel_config(&lcd_backlight_channel), TAG, "backlight LEDC channel config failed");

    return ESP_OK;
}

esp_err_t tab5_platform_backlight_set(int brightness_percent)
{
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    if (brightness_percent < 0) {
        brightness_percent = 0;
    }

    uint32_t duty_cycle = (4095 * brightness_percent) / 100;

    ESP_RETURN_ON_ERROR(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH, duty_cycle), TAG, "backlight set duty failed");
    ESP_RETURN_ON_ERROR(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH), TAG, "backlight update duty failed");

    return ESP_OK;
}

esp_err_t tab5_platform_init(void)
{
    if (platform_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Tab5 platform init based on H020 baseline");

    ESP_RETURN_ON_ERROR(tab5_i2c_init(), TAG, "I2C init failed");
ESP_RETURN_ON_ERROR(tab5_port_a_i2c_init(), TAG, "PORT A I2C init failed");
    ESP_RETURN_ON_ERROR(tab5_pi4ioe1_init(), TAG, "PI4IOE1 init failed");
    ESP_RETURN_ON_ERROR(tab5_reset_lcd_touch(), TAG, "LCD/TP reset failed");

    ESP_RETURN_ON_ERROR(tab5_backlight_init(), TAG, "Backlight init failed");
    ESP_RETURN_ON_ERROR(tab5_platform_backlight_set(0), TAG, "Backlight off failed");

    ESP_LOGI(TAG, "Acquire MIPI DSI PHY LDO");

    esp_ldo_channel_config_t ldo_cfg = {};
    ldo_cfg.chan_id = 3;
    ldo_cfg.voltage_mv = 2500;

    ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_cfg, &phy_pwr_chan), TAG, "LDO failed");

    ESP_LOGI(TAG, "Create MIPI DSI bus");

    esp_lcd_dsi_bus_config_t bus_config = {};
    bus_config.bus_id = 0;
    bus_config.num_data_lanes = 2;
    bus_config.phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT;
    bus_config.lane_bit_rate_mbps = 965;

    ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&bus_config, &dsi_bus), TAG, "DSI bus failed");

    ESP_LOGI(TAG, "Create DBI panel IO");

    esp_lcd_dbi_io_config_t dbi_config = {};
    dbi_config.virtual_channel = 0;
    dbi_config.lcd_cmd_bits = 8;
    dbi_config.lcd_param_bits = 8;

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_dbi(dsi_bus, &dbi_config, &dbi_io), TAG, "DBI IO failed");

    ESP_LOGI(TAG, "Configure ST7121 DPI panel");

    esp_lcd_dpi_panel_config_t dpi_cfg = {};
    dpi_cfg.dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT;
    dpi_cfg.dpi_clock_freq_mhz = 70;
    dpi_cfg.virtual_channel = 0;
    dpi_cfg.pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565;
    dpi_cfg.num_fbs = 1;

    dpi_cfg.video_timing.h_size = TAB5_LCD_W;
    dpi_cfg.video_timing.v_size = TAB5_LCD_H;
    dpi_cfg.video_timing.hsync_pulse_width = 2;
    dpi_cfg.video_timing.hsync_back_porch = 40;
    dpi_cfg.video_timing.hsync_front_porch = 40;
    dpi_cfg.video_timing.vsync_pulse_width = 20;
    dpi_cfg.video_timing.vsync_back_porch = 24;
    dpi_cfg.video_timing.vsync_front_porch = 200;

    dpi_cfg.flags.use_dma2d = true;

    ESP_LOGI(TAG, "SPIRAM heap: %u", (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI(TAG, "Largest SPIRAM block: %u", (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));

    ESP_LOGI(TAG, "Create ST7121 panel");

    st7121_vendor_config_t vendor_config = {
        .init_cmds = NULL,
        .init_cmds_size = 0,
        .mipi_config = {
            .dsi_bus = dsi_bus,
            .dpi_config = &dpi_cfg,
        },
    };

    const esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
        .bits_per_pixel = 24,
        .vendor_config = &vendor_config,
    };

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st7121(dbi_io, &lcd_dev_config, &panel), TAG, "esp_lcd_new_panel_st7121 failed");

    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel), TAG, "panel reset failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel), TAG, "panel init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(panel, true), TAG, "display on failed");

    platform_initialized = true;

    ESP_LOGI(TAG, "Tab5 platform init OK");

    return ESP_OK;
}

esp_err_t tab5_platform_fill(uint16_t color)
{
    if (!platform_initialized || panel == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    const size_t frame_size = TAB5_LCD_W * TAB5_LCD_H * sizeof(uint16_t);

    uint16_t *frame = (uint16_t *)heap_caps_malloc(frame_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!frame) {
        ESP_LOGE(TAG, "Failed to allocate frame buffer");
        return ESP_ERR_NO_MEM;
    }

    for (int i = 0; i < TAB5_LCD_W * TAB5_LCD_H; i++) {
        frame[i] = color;
    }

    esp_err_t ret = esp_lcd_panel_draw_bitmap(panel, 0, 0, TAB5_LCD_W, TAB5_LCD_H, frame);

    heap_caps_free(frame);

    return ret;
}

esp_err_t tab5_platform_draw_bitmap(
    int x,
    int y,
    int w,
    int h,
    const uint16_t *pixels
)
{
    if (!platform_initialized || panel == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    if (pixels == nullptr || w <= 0 || h <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_lcd_panel_draw_bitmap(
        panel,
        x,
        y,
        x + w,
        y + h,
        pixels
    );
}




i2c_master_bus_handle_t tab5_platform_get_i2c_bus(void)
{
    return tab5_i2c_bus;
}


static esp_err_t tab5_port_a_i2c_init(void)
{
    ESP_LOGI(TAG, "init PORT A I2C bus SDA=53 SCL=54");

    i2c_master_bus_config_t cfg = {};
    cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    cfg.sda_io_num = GPIO_NUM_53;
    cfg.scl_io_num = GPIO_NUM_54;
    cfg.i2c_port = 1;
    cfg.glitch_ignore_cnt = 7;
    cfg.flags.enable_internal_pullup = true;

    return i2c_new_master_bus(&cfg, &port_a_i2c_bus);
}
i2c_master_bus_handle_t tab5_platform_get_port_a_i2c_bus(void)
{
    return port_a_i2c_bus;
}


