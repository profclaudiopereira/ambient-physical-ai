/*
 * Identity Console V1
 *
 * Ambient Physical AI
 *
 * Current phase:
 * - Stable Identity Console Core
 * - WS1850S NFC hardware probe
 * - WS1850S continuous card presence polling
 */

#include "M5Unified.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bsp/m5dial.h"
#include "iot_knob.h"

#include "driver/i2c_master.h"
#include "ws1850s.h"

static const char *TAG = "identity-node";

struct Profile {
    const char *id;
    const char *name;
    const char *role;
};

static const Profile profiles[] = {
    {"unknown", "Unknown", "visitor"},
    {"claudio", "Claudio", "owner"},
    {"student", "Student", "learner"},
};

static const char *contexts[] = {
    "Lab",
    "Classroom",
    "Demo",
    "Meeting",
};

static const int PROFILE_COUNT = sizeof(profiles) / sizeof(profiles[0]);
static const int CONTEXT_COUNT = sizeof(contexts) / sizeof(contexts[0]);

static int current_profile = 0;
static int current_context = 0;

static bool nfc_detected = false;
static bool nfc_card_present = false;

static int nfc_yes_count = 0;
static int nfc_no_count = 0;

static i2c_master_bus_handle_t nfc_bus_handle = NULL;
static i2c_master_dev_handle_t nfc_dev_handle = NULL;

static int nfc_poll_counter = 0;

static void draw_console()
{
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(2);

    M5.Display.setCursor(20, 25);
    M5.Display.println("Identity Console");

    M5.Display.setCursor(20, 70);
    M5.Display.printf("Name: %s", profiles[current_profile].name);

    M5.Display.setCursor(20, 105);
    M5.Display.printf("Role: %s", profiles[current_profile].role);

    M5.Display.setCursor(20, 155);
    M5.Display.printf("Ctx: %s", contexts[current_context]);

    M5.Display.setCursor(20, 205);
    M5.Display.printf("NFC: %s", nfc_detected ? "OK" : "NO");

    M5.Display.setCursor(20, 225);
    M5.Display.printf("Card: %s", nfc_card_present ? "YES" : "NO");
}

static bool setup_nfc()
{
    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = I2C_NUM_0;
    bus_cfg.sda_io_num = BSP_I2C_SDA;
    bus_cfg.scl_io_num = BSP_I2C_SCL;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.flags.enable_internal_pullup = true;

    esp_err_t ret = i2c_new_master_bus(&bus_cfg, &nfc_bus_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC I2C bus init failed: %s", esp_err_to_name(ret));
        nfc_bus_handle = NULL;
        return false;
    }

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = WS1850S_I2C_ADDRESS;
    dev_cfg.scl_speed_hz = 100000;

    ret = i2c_master_bus_add_device(nfc_bus_handle, &dev_cfg, &nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC add device failed: %s", esp_err_to_name(ret));
        i2c_del_master_bus(nfc_bus_handle);
        nfc_bus_handle = NULL;
        nfc_dev_handle = NULL;
        return false;
    }

    ret = ws1850s_probe(nfc_dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC/WS1850S not detected at 0x28: %s", esp_err_to_name(ret));
        i2c_master_bus_rm_device(nfc_dev_handle);
        i2c_del_master_bus(nfc_bus_handle);
        nfc_bus_handle = NULL;
        nfc_dev_handle = NULL;
        return false;
    }

    uint8_t version = 0;
    esp_err_t version_ret = ws1850s_read_version(nfc_dev_handle, &version);

    if (version_ret == ESP_OK) {
        ESP_LOGI(TAG, "NFC/WS1850S VersionReg: 0x%02X", version);
    } else {
        ESP_LOGW(TAG, "NFC/WS1850S VersionReg read failed: %s", esp_err_to_name(version_ret));
    }

    esp_err_t init_ret = ws1850s_init(nfc_dev_handle);

    if (init_ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC/WS1850S init failed: %s", esp_err_to_name(init_ret));
        return false;
    }

    ESP_LOGI(TAG, "NFC/WS1850S initialized");
    ESP_LOGI(TAG, "NFC/WS1850S detected at 0x28");

    return true;
}


static void poll_nfc_card()
{
    if (!nfc_detected || nfc_dev_handle == NULL) {
        return;
    }

    bool raw_present = false;
ws1850s_init(nfc_dev_handle);
    esp_err_t ret = ws1850s_card_present(nfc_dev_handle, &raw_present);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "NFC polling failed: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "NFC raw: %s", raw_present ? "YES" : "NO");

    if (raw_present) {
        nfc_yes_count++;
        nfc_no_count = 0;

        if (!nfc_card_present) {
            nfc_card_present = true;
uint8_t uid[10] = {0};
uint8_t uid_len = 0;
ws1850s_init(nfc_dev_handle);
vTaskDelay(pdMS_TO_TICKS(50));
esp_err_t uid_ret = ws1850s_read_uid(nfc_dev_handle, uid, &uid_len);

if (uid_ret == ESP_OK) {
    char uid_text[32] = {0};

    for (int i = 0; i < uid_len; i++) {
        snprintf(uid_text + (i * 2), sizeof(uid_text) - (i * 2), "%02X", uid[i]);
    }

    ESP_LOGI(TAG, "NFC UID: %s", uid_text);
} else {
    ESP_LOGW(TAG, "NFC UID read failed: %s", esp_err_to_name(uid_ret));
}
            ESP_LOGI(TAG, "NFC card stable: YES");
            M5.Speaker.tone(2800, 80);
            draw_console();
        }
    } else {
        nfc_no_count++;

        if (nfc_no_count >= 6 && nfc_card_present) {
            nfc_card_present = false;
            nfc_yes_count = 0;
            ESP_LOGI(TAG, "NFC card stable: NO");
            M5.Speaker.tone(1800, 80);
            draw_console();
        }
    }
}

static void generate_identity_package()
{
    ESP_LOGI(TAG,
             "{\"type\":\"identity_package\",\"profile\":{\"id\":\"%s\",\"name\":\"%s\",\"role\":\"%s\"},\"context\":\"%s\",\"nfc\":{\"detected\":%s,\"card_present\":%s},\"source\":\"m5dial_identity_console_v1\"}",
             profiles[current_profile].id,
             profiles[current_profile].name,
             profiles[current_profile].role,
             contexts[current_context],
             nfc_detected ? "true" : "false",
             nfc_card_present ? "true" : "false");

    M5.Speaker.tone(3000, 120);

    M5.Display.fillRect(0, 205, 240, 35, BLACK);
    M5.Display.setCursor(20, 205);
    M5.Display.println("PACKAGE OK");

    vTaskDelay(pdMS_TO_TICKS(500));
    draw_console();
}

extern "C" void app_main(void)
{
    auto cfg = M5.config();
    M5.begin(cfg);

    ESP_LOGI(TAG, "Identity Console V1");
    ESP_LOGI(TAG, "Core version with NFC continuous polling");

    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = BSP_ENCODER_A,
        .gpio_encoder_b = BSP_ENCODER_B,
        .enable_power_save = false,
    };

    knob_handle_t knob = iot_knob_create(&knob_cfg);

    if (knob == NULL) {
        ESP_LOGE(TAG, "Failed to create knob encoder");
    } else {
        ESP_LOGI(TAG, "Knob encoder initialized");
    }

    nfc_detected = setup_nfc();

    draw_console();
    M5.Speaker.tone(2000, 150);

    int last_encoder_count = 0;
    int last_touch_state = 0;

    while (true)
    {
        M5.update();

        if (knob != NULL) {
            int encoder_count = iot_knob_get_count_value(knob);

            if (encoder_count != last_encoder_count) {
                if (encoder_count > last_encoder_count) {
                    current_context++;
                } else {
                    current_context--;
                }

                if (current_context >= CONTEXT_COUNT) {
                    current_context = 0;
                }

                if (current_context < 0) {
                    current_context = CONTEXT_COUNT - 1;
                }

                ESP_LOGI(TAG, "Context selected: %s", contexts[current_context]);

                last_encoder_count = encoder_count;
                draw_console();
            }
        }

        int touch_state = M5.Touch.getCount();

        if (touch_state > 0 && last_touch_state == 0) {
            current_profile++;

            if (current_profile >= PROFILE_COUNT) {
                current_profile = 0;
            }

            ESP_LOGI(TAG, "Profile selected: %s / %s",
                     profiles[current_profile].name,
                     profiles[current_profile].role);

            M5.Speaker.tone(2500, 80);
            draw_console();

            generate_identity_package();
        }

        last_touch_state = touch_state;

        nfc_poll_counter++;

        if (nfc_poll_counter >= 5) {
            nfc_poll_counter = 0;
            poll_nfc_card();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}