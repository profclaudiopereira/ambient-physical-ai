#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include <string.h>
#include <sys/socket.h>

static const char *TAG = "presence-tof";

#define I2C_PORT I2C_NUM_0
#define I2C_SDA  GPIO_NUM_2
#define I2C_SCL  GPIO_NUM_1

#define VL53L0X_ADDR 0x29

#define WIFI_SSID "CAZAZUL"
#define WIFI_PASS "H10CAZAZUL"

#define UDP_BROADCAST_IP "192.168.0.255"
#define UDP_PORT 3333

#define PRESENCE_THRESHOLD_MM     600
#define NOT_PRESENT_THRESHOLD_MM  800

#define SYSRANGE_START                  0x00
#define SYSTEM_SEQUENCE_CONFIG          0x01
#define SYSTEM_INTERRUPT_CONFIG_GPIO    0x0A
#define SYSTEM_INTERRUPT_CLEAR          0x0B
#define RESULT_INTERRUPT_STATUS         0x13
#define RESULT_RANGE_STATUS             0x14
#define GPIO_HV_MUX_ACTIVE_HIGH         0x84
#define MSRC_CONFIG_CONTROL             0x60
#define FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT 0x44
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_0 0xB0

static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t tof_handle = NULL;
static uint8_t stop_variable = 0;

static int udp_socket_fd = -1;
static struct sockaddr_in udp_dest_addr = {};
static bool presence_state = false;
static volatile bool wifi_connected = false;
static bool pending_presence_udp = false;
static uint16_t pending_presence_distance_mm = 0;

// -----------------------------------------------------------------------------
// Wi-Fi STA + UDP Broadcast
// -----------------------------------------------------------------------------

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    (void)arg;
    (void)event_data;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_connected = false;
        ESP_LOGW(TAG, "Wi-Fi disconnected, reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        wifi_connected = true;
        ESP_LOGI(TAG, "Wi-Fi connected. IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

static void wifi_init_sta()
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    } else {
        ESP_ERROR_CHECK(ret);
    }

    ESP_ERROR_CHECK(esp_netif_init());

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(ret);
    }

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        NULL
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL,
        NULL
    ));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi STA started. SSID: %s", WIFI_SSID);
}

static void udp_broadcast_init()
{
    udp_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (udp_socket_fd < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket");
        return;
    }

    int broadcast_enable = 1;
    int ret = setsockopt(
        udp_socket_fd,
        SOL_SOCKET,
        SO_BROADCAST,
        &broadcast_enable,
        sizeof(broadcast_enable)
    );

    if (ret < 0) {
        ESP_LOGW(TAG, "Failed to enable UDP broadcast");
    }

    udp_dest_addr = {};
    udp_dest_addr.sin_family = AF_INET;
    udp_dest_addr.sin_port = htons(UDP_PORT);
    udp_dest_addr.sin_addr.s_addr = inet_addr(UDP_BROADCAST_IP);

    ESP_LOGI(TAG, "UDP broadcast configured: %s:%d", UDP_BROADCAST_IP, UDP_PORT);
}

static bool send_presence_event_udp(uint16_t distance_mm)
{
    if (!wifi_connected) {
        ESP_LOGW(TAG, "UDP send postponed: Wi-Fi not connected yet");
        return false;
    }

    if (udp_socket_fd < 0) {
        ESP_LOGW(TAG, "UDP socket not ready");
        return false;
    }

    char payload[160] = {0};

    snprintf(
        payload,
        sizeof(payload),
        "{\"type\":\"presence_event\",\"state\":\"PRESENT\",\"distance_mm\":%u,\"source\":\"presence_node_v1\"}",
        distance_mm
    );

    int sent = sendto(
        udp_socket_fd,
        payload,
        strlen(payload),
        0,
        (struct sockaddr *)&udp_dest_addr,
        sizeof(udp_dest_addr)
    );

    if (sent < 0) {
        ESP_LOGW(TAG, "UDP send failed");
        return false;
    }

    ESP_LOGI(TAG, "UDP presence_event sent: %s", payload);
    return true;
}

static esp_err_t write8(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    return i2c_master_transmit(tof_handle, data, 2, 100);
}

static esp_err_t read8(uint8_t reg, uint8_t *value)
{
    return i2c_master_transmit_receive(tof_handle, &reg, 1, value, 1, 100);
}

static esp_err_t read16(uint8_t reg, uint16_t *value)
{
    uint8_t data[2] = {0};
    esp_err_t ret = i2c_master_transmit_receive(tof_handle, &reg, 1, data, 2, 100);
    if (ret != ESP_OK) return ret;
    *value = ((uint16_t)data[0] << 8) | data[1];
    return ESP_OK;
}

static esp_err_t write16(uint8_t reg, uint16_t value)
{
    uint8_t data[3] = {reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF)};
    return i2c_master_transmit(tof_handle, data, 3, 100);
}

static esp_err_t read_multi(uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(tof_handle, &reg, 1, data, len, 100);
}

static esp_err_t write_multi(uint8_t reg, uint8_t *data, size_t len)
{
    uint8_t buffer[16] = {0};
    if (len + 1 > sizeof(buffer)) return ESP_ERR_INVALID_SIZE;

    buffer[0] = reg;
    for (size_t i = 0; i < len; i++) buffer[i + 1] = data[i];

    return i2c_master_transmit(tof_handle, buffer, len + 1, 100);
}

static esp_err_t get_spad_info(uint8_t *count, bool *type_is_aperture)
{
    uint8_t tmp = 0;

    write8(0x80, 0x01);
    write8(0xFF, 0x01);
    write8(0x00, 0x00);

    read8(0x83, &tmp);
    write8(0x83, tmp | 0x04);

    write8(0xFF, 0x07);
    write8(0x81, 0x01);
    write8(0x80, 0x01);
    write8(0x94, 0x6b);
    write8(0x83, 0x00);

    for (int i = 0; i < 100; i++) {
        read8(0x83, &tmp);
        if (tmp != 0x00) break;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    write8(0x83, 0x01);
    read8(0x92, &tmp);

    *count = tmp & 0x7f;
    *type_is_aperture = (tmp >> 7) & 0x01;

    write8(0x81, 0x00);
    write8(0xFF, 0x06);

    read8(0x83, &tmp);
    write8(0x83, tmp & ~0x04);

    write8(0xFF, 0x01);
    write8(0x00, 0x01);

    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    return ESP_OK;
}

static void load_default_tuning_settings()
{
    write8(0xFF, 0x01);
    write8(0x00, 0x00);

    write8(0xFF, 0x00);
    write8(0x09, 0x00);
    write8(0x10, 0x00);
    write8(0x11, 0x00);
    write8(0x24, 0x01);
    write8(0x25, 0xFF);
    write8(0x75, 0x00);

    write8(0xFF, 0x01);
    write8(0x4E, 0x2C);
    write8(0x48, 0x00);
    write8(0x30, 0x20);

    write8(0xFF, 0x00);
    write8(0x30, 0x09);
    write8(0x54, 0x00);
    write8(0x31, 0x04);
    write8(0x32, 0x03);
    write8(0x40, 0x83);
    write8(0x46, 0x25);
    write8(0x60, 0x00);
    write8(0x27, 0x00);
    write8(0x50, 0x06);
    write8(0x51, 0x00);
    write8(0x52, 0x96);
    write8(0x56, 0x08);
    write8(0x57, 0x30);
    write8(0x61, 0x00);
    write8(0x62, 0x00);
    write8(0x64, 0x00);
    write8(0x65, 0x00);
    write8(0x66, 0xA0);

    write8(0xFF, 0x01);
    write8(0x22, 0x32);
    write8(0x47, 0x14);
    write8(0x49, 0xFF);
    write8(0x4A, 0x00);

    write8(0xFF, 0x00);
    write8(0x7A, 0x0A);
    write8(0x7B, 0x00);
    write8(0x78, 0x21);

    write8(0xFF, 0x01);
    write8(0x23, 0x34);
    write8(0x42, 0x00);
    write8(0x44, 0xFF);
    write8(0x45, 0x26);
    write8(0x46, 0x05);
    write8(0x40, 0x40);
    write8(0x0E, 0x06);
    write8(0x20, 0x1A);
    write8(0x43, 0x40);

    write8(0xFF, 0x00);
    write8(0x34, 0x03);
    write8(0x35, 0x44);

    write8(0xFF, 0x01);
    write8(0x31, 0x04);
    write8(0x4B, 0x09);
    write8(0x4C, 0x05);
    write8(0x4D, 0x04);

    write8(0xFF, 0x00);
    write8(0x44, 0x00);
    write8(0x45, 0x20);
    write8(0x47, 0x08);
    write8(0x48, 0x28);
    write8(0x67, 0x00);
    write8(0x70, 0x04);
    write8(0x71, 0x01);
    write8(0x72, 0xFE);
    write8(0x76, 0x00);
    write8(0x77, 0x00);

    write8(0xFF, 0x01);
    write8(0x0D, 0x01);

    write8(0xFF, 0x00);
    write8(0x80, 0x01);
    write8(0x01, 0xF8);

    write8(0xFF, 0x01);
    write8(0x8E, 0x01);
    write8(0x00, 0x01);

    write8(0xFF, 0x00);
    write8(0x80, 0x00);
}

static esp_err_t perform_single_ref_calibration(uint8_t vhv_init_byte)
{
    write8(SYSRANGE_START, 0x01 | vhv_init_byte);

    for (int i = 0; i < 100; i++) {
        uint8_t status = 0;
        read8(RESULT_INTERRUPT_STATUS, &status);
        if ((status & 0x07) != 0) {
            write8(SYSTEM_INTERRUPT_CLEAR, 0x01);
            write8(SYSRANGE_START, 0x00);
            return ESP_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return ESP_ERR_TIMEOUT;
}

static esp_err_t vl53l0x_init()
{
    uint8_t model = 0, module = 0, revision = 0;
    read8(0xC0, &model);
    read8(0xC1, &module);
    read8(0xC2, &revision);

    ESP_LOGI(TAG, "VL53L0X detected");
    ESP_LOGI(TAG, "MODEL_ID    = 0x%02X", model);
    ESP_LOGI(TAG, "MODULE_TYPE = 0x%02X", module);
    ESP_LOGI(TAG, "REVISION_ID = 0x%02X", revision);

    write8(0x88, 0x00);

    write8(0x80, 0x01);
    write8(0xFF, 0x01);
    write8(0x00, 0x00);
    read8(0x91, &stop_variable);
    write8(0x00, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    uint8_t val = 0;
    read8(MSRC_CONFIG_CONTROL, &val);
    write8(MSRC_CONFIG_CONTROL, val | 0x12);

    write16(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, 32); // 0.25 MCPS
    write8(SYSTEM_SEQUENCE_CONFIG, 0xFF);

    uint8_t spad_count = 0;
    bool spad_type_is_aperture = false;
    get_spad_info(&spad_count, &spad_type_is_aperture);

    uint8_t ref_spad_map[6] = {0};
    read_multi(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

    uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0;
    uint8_t spads_enabled = 0;

    for (uint8_t i = 0; i < 48; i++) {
        if (i < first_spad_to_enable || spads_enabled == spad_count) {
            ref_spad_map[i / 8] &= ~(1 << (i % 8));
        } else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1) {
            spads_enabled++;
        }
    }

    write_multi(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

    load_default_tuning_settings();

    write8(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);

    read8(GPIO_HV_MUX_ACTIVE_HIGH, &val);
    write8(GPIO_HV_MUX_ACTIVE_HIGH, val & ~0x10);

    write8(SYSTEM_INTERRUPT_CLEAR, 0x01);

    write8(SYSTEM_SEQUENCE_CONFIG, 0x01);
    esp_err_t ret = perform_single_ref_calibration(0x40);
    if (ret != ESP_OK) return ret;

    write8(SYSTEM_SEQUENCE_CONFIG, 0x02);
    ret = perform_single_ref_calibration(0x00);
    if (ret != ESP_OK) return ret;

    write8(SYSTEM_SEQUENCE_CONFIG, 0xE8);

    ESP_LOGI(TAG, "VL53L0X init complete");
    return ESP_OK;
}

static esp_err_t read_distance_mm(uint16_t *distance)
{
    esp_err_t ret = ESP_OK;

    ret = write8(0x80, 0x01);
    if (ret != ESP_OK) return ret;

    ret = write8(0xFF, 0x01);
    if (ret != ESP_OK) return ret;

    ret = write8(0x00, 0x00);
    if (ret != ESP_OK) return ret;

    ret = write8(0x91, stop_variable);
    if (ret != ESP_OK) return ret;

    ret = write8(0x00, 0x01);
    if (ret != ESP_OK) return ret;

    ret = write8(0xFF, 0x00);
    if (ret != ESP_OK) return ret;

    ret = write8(0x80, 0x00);
    if (ret != ESP_OK) return ret;

    ret = write8(SYSRANGE_START, 0x01);
    if (ret != ESP_OK) return ret;

    for (int i = 0; i < 100; i++) {
        uint8_t sysrange = 0;

        ret = read8(SYSRANGE_START, &sysrange);
        if (ret != ESP_OK) {
            return ret;
        }

        if ((sysrange & 0x01) == 0) {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));

        if (i == 99) {
            return ESP_ERR_TIMEOUT;
        }
    }

    for (int i = 0; i < 100; i++) {
        uint8_t status = 0;

        ret = read8(RESULT_INTERRUPT_STATUS, &status);
        if (ret != ESP_OK) {
            return ret;
        }

        if ((status & 0x07) != 0) {
            ret = read16(RESULT_RANGE_STATUS + 10, distance);
            if (ret != ESP_OK) {
                return ret;
            }

            ret = write8(SYSTEM_INTERRUPT_CLEAR, 0x01);
            if (ret != ESP_OK) {
                return ret;
            }

            return ESP_OK;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return ESP_ERR_TIMEOUT;
}

static esp_err_t setup_i2c()
{
    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = I2C_PORT;
    bus_cfg.sda_io_num = I2C_SDA;
    bus_cfg.scl_io_num = I2C_SCL;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.flags.enable_internal_pullup = true;

    esp_err_t ret = i2c_new_master_bus(&bus_cfg, &bus_handle);
    if (ret != ESP_OK) return ret;

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = VL53L0X_ADDR;
    dev_cfg.scl_speed_hz = 100000;

    return i2c_master_bus_add_device(bus_handle, &dev_cfg, &tof_handle);
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "PRESENCE_NODE_V1_MILESTONE_001");
    ESP_LOGI(TAG, "AtomS3 Lite + Unit Mini ToF-90 / VL53L0X");
    ESP_LOGI(TAG, "Goal: Distance(mm) in serial + UDP broadcast on PRESENT transition");
    ESP_LOGI(TAG, "I2C: SDA=GPIO2 SCL=GPIO1");
    ESP_LOGI(TAG, "Wi-Fi SSID: %s", WIFI_SSID);
    ESP_LOGI(TAG, "UDP broadcast: %s:%d", UDP_BROADCAST_IP, UDP_PORT);
    ESP_LOGI(TAG, "Presence threshold: PRESENT < %d mm, NOT_PRESENT > %d mm",
             PRESENCE_THRESHOLD_MM,
             NOT_PRESENT_THRESHOLD_MM);

    // Preservation-first boot order:
    // initialize the validated ToF/I2C baseline before enabling Wi-Fi radio.
    // Starting Wi-Fi before VL53L0X init caused early I2C NACK bursts on AtomS3 Lite.

    esp_err_t ret = setup_i2c();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C setup failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = vl53l0x_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "VL53L0X init failed: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "ToF baseline initialized. Starting Wi-Fi/UDP after VL53L0X init.");
    wifi_init_sta();
    udp_broadcast_init();

    while (true) {
        uint16_t distance = 0;

        ret = read_distance_mm(&distance);

        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Distance: %u mm", distance);

            if (!presence_state && distance < PRESENCE_THRESHOLD_MM) {
                presence_state = true;
                pending_presence_distance_mm = distance;
                ESP_LOGI(TAG, "PRESENT");

                if (!send_presence_event_udp(distance)) {
                    pending_presence_udp = true;
                }
            } else if (presence_state && distance > NOT_PRESENT_THRESHOLD_MM) {
                presence_state = false;
                pending_presence_udp = false;
                ESP_LOGI(TAG, "NOT_PRESENT");
            }

            if (presence_state && pending_presence_udp && wifi_connected) {
                if (send_presence_event_udp(pending_presence_distance_mm)) {
                    pending_presence_udp = false;
                }
            }
        } else {
            ESP_LOGW(TAG, "Distance read failed: %s", esp_err_to_name(ret));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}