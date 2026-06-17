/*
 * Identity Console V1
 *
 * Ambient Physical AI
 *
 * Responsibilities:
 * - Profile selection
 * - Context selection
 * - User feedback
 * - Identity package generation
 *
 * Current phase:
 * Stable M5Dial baseline without NFC driver dependency.
 */

#include "M5Unified.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bsp/m5dial.h"
#include "iot_knob.h"

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
    M5.Display.println("Touch = ID pkg");
}

static void generate_identity_package()
{
    ESP_LOGI(TAG,
             "{\"type\":\"identity_package\",\"profile\":{\"id\":\"%s\",\"name\":\"%s\",\"role\":\"%s\"},\"context\":\"%s\",\"source\":\"m5dial_identity_console_v1\"}",
             profiles[current_profile].id,
             profiles[current_profile].name,
             profiles[current_profile].role,
             contexts[current_context]);

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
    ESP_LOGI(TAG, "Core version without NFC driver dependency");

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

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}