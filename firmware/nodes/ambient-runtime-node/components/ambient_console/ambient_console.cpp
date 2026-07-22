#include "ambient_console.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "fonts/font8x8_basic.h"
#include "tab5_platform.h"

static const char *TAG = "ambient-console";

#define CONSOLE_W 720
#define CONSOLE_H 1280

#define COLOR_BACKGROUND 0x0000
#define COLOR_TEXT       0xFFFF

static uint16_t *frame = nullptr;

static void clear_frame(uint16_t color)
{
    const size_t pixel_count =
        (size_t)CONSOLE_W * (size_t)CONSOLE_H;

    for (size_t index = 0; index < pixel_count; index++) {
        frame[index] = color;
    }
}

static void draw_pixel(
    int x,
    int y,
    uint16_t color
)
{
    if (x < 0 || x >= CONSOLE_W ||
        y < 0 || y >= CONSOLE_H) {
        return;
    }

    frame[(y * CONSOLE_W) + x] = color;
}

static void draw_char(
    int x,
    int y,
    char character,
    int scale,
    uint16_t color
)
{
    if (scale <= 0) {
        return;
    }

    const unsigned char code =
        (unsigned char)character;

    const unsigned char safe_code =
        (code < 128) ? code : (unsigned char)'?';

    for (int row = 0; row < 8; row++) {
        const uint8_t row_bits =
            (uint8_t)font8x8_basic[safe_code][row];

        for (int column = 0; column < 8; column++) {

            /*
             * A fonte dhepper/font8x8 usa o bit menos
             * significativo como a primeira coluna.
             */
            const bool pixel_enabled =
                (row_bits & (1U << column)) != 0;

            if (!pixel_enabled) {
                continue;
            }

            for (int scale_y = 0; scale_y < scale; scale_y++) {
                for (int scale_x = 0; scale_x < scale; scale_x++) {
                    draw_pixel(
                        x + (column * scale) + scale_x,
                        y + (row * scale) + scale_y,
                        color
                    );
                }
            }
        }
    }
}

static void draw_text(
    int x,
    int y,
    const char *text,
    int scale,
    uint16_t color
)
{
    if (text == nullptr || scale <= 0) {
        return;
    }

    int cursor_x = x;

    while (*text != '\0') {
        draw_char(
            cursor_x,
            y,
            *text,
            scale,
            color
        );

        /*
         * 8 colunas do caractere + 1 coluna de espaço.
         */
        cursor_x += 9 * scale;
        text++;
    }
}

static void draw_horizontal_line(
    int x,
    int y,
    int width,
    int thickness,
    uint16_t color
)
{
    if (width <= 0 || thickness <= 0) {
        return;
    }

    for (int offset_y = 0; offset_y < thickness; offset_y++) {
        for (int offset_x = 0; offset_x < width; offset_x++) {
            draw_pixel(
                x + offset_x,
                y + offset_y,
                color
            );
        }
    }
}


esp_err_t ambient_console_init(void)
{
    ESP_LOGI(TAG, "Ambient Runtime Console init");

    if (frame != nullptr) {
        return ESP_OK;
    }

    const size_t frame_size =
        (size_t)CONSOLE_W *
        (size_t)CONSOLE_H *
        sizeof(uint16_t);

    frame = static_cast<uint16_t *>(
        heap_caps_malloc(
            frame_size,
            MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
        )
    );

    if (frame == nullptr) {
        ESP_LOGE(
            TAG,
            "Failed to allocate console framebuffer"
        );

        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(
        TAG,
        "Console framebuffer allocated: %u bytes",
        (unsigned)frame_size
    );

    return ESP_OK;
}

esp_err_t ambient_console_render_static(void)
{
    if (frame == nullptr) {
        ESP_LOGE(TAG, "Console framebuffer not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "Render static Ambient Runtime Console");

    clear_frame(COLOR_BACKGROUND);

    draw_text(
        40,
        60,
        "Ambient Physical AI",
        3,
        COLOR_TEXT
    );

    draw_text(
        40,
        105,
        "Ambient Runtime Console",
        3,
        COLOR_TEXT
    );

    return tab5_platform_draw_bitmap(
        0,
        0,
        CONSOLE_W,
        CONSOLE_H,
        frame
    );
}

esp_err_t ambient_console_render(
    const ambient_console_data_t *data
)
{
    if (data == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (frame == nullptr) {
        ESP_LOGE(TAG, "Console framebuffer not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(
        TAG,
        "Console data: T=%.2f C H=%.2f %% P=%.2f hPa L=%.2f lx",
        data->temperature_c,
        data->humidity_percent,
        data->pressure_hpa,
        data->light_lux
    );

    clear_frame(COLOR_BACKGROUND);

    char value[64];

    /*
     * Header
     */
    draw_text(
        40,
        40,
        "AMBIENT PHYSICAL AI",
        3,
        COLOR_TEXT
    );

    draw_text(
        40,
        82,
        "AMBIENT RUNTIME CONSOLE",
        2,
        COLOR_TEXT
    );

    draw_horizontal_line(
        40,
        118,
        640,
        2,
        COLOR_TEXT
    );

    /*
     * SYSTEM
     */
    draw_text(
        40,
        150,
        "SYSTEM",
        2,
        COLOR_TEXT
    );

snprintf(
    value,
    sizeof(value),
    "WI-FI ............ %s",
    data->wifi_connected ? "CONNECTED" : "PENDING"
);

draw_text(40, 190, value, 2, COLOR_TEXT);

snprintf(
    value,
    sizeof(value),
    "IP ............... %s",
    data->ipv4
);

draw_text(40, 225, value, 2, COLOR_TEXT);

snprintf(
    value,
    sizeof(value),
    "MASK ............. %s",
    data->netmask
);

draw_text(40, 260, value, 2, COLOR_TEXT);
   draw_horizontal_line(
    40,
    300,
    640,
    2,
    COLOR_TEXT
);
    /*
     * ENVIRONMENT
     */
    draw_text(
        40,
        330,
        "ENVIRONMENT",
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "TEMPERATURE ....... %.1f C",
        data->temperature_c
    );

    draw_text(
        40,
        370,
        value,
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "HUMIDITY .......... %.1f %%",
        data->humidity_percent
    );

    draw_text(
        40,
        405,
        value,
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "PRESSURE .......... %.1f HPA",
        data->pressure_hpa
    );

    draw_text(
        40,
        440,
        value,
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "LIGHT ............. %.1f LX",
        data->light_lux
    );

    draw_text(
        40,
        475,
        value,
        2,
        COLOR_TEXT
    );

    draw_horizontal_line(
        40,
        515,
        640,
        2,
        COLOR_TEXT
    );

    /*
     * I2C NETWORK
     */
    draw_text(
        40,
        545,
        "I2C NETWORK",
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "PAHUB ............. %s",
        data->pahub_ok ? "OK" : "ERROR"
    );

    draw_text(
        40,
        585,
        value,
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "ENV-IV ............ %s",
        data->env_iv_ok ? "OK" : "ERROR"
    );

    draw_text(
        40,
        620,
        value,
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "DLIGHT ............ %s",
        data->dlight_ok ? "OK" : "ERROR"
    );

    draw_text(
        40,
        655,
        value,
        2,
        COLOR_TEXT
    );

    snprintf(
        value,
        sizeof(value),
        "MINI OLED ......... %s",
        data->mini_oled_ok ? "OK" : "ERROR"
    );

    draw_text(
        40,
        690,
        value,
        2,
        COLOR_TEXT
    );

    draw_horizontal_line(
        40,
        730,
        640,
        2,
        COLOR_TEXT
    );

    /*
     * STATUS
     */
    draw_text(
        40,
        760,
        "STATUS",
        2,
        COLOR_TEXT
    );

    const bool hardware_ready =
    data->pahub_ok &&
    data->env_iv_ok &&
    data->dlight_ok &&
    data->mini_oled_ok;

const bool runtime_ready =
    hardware_ready &&
    data->network_ready;

draw_text(
    40,
    805,
    runtime_ready ? "NETWORK READY" :
    hardware_ready ? "LOCAL READY" : "DEGRADED",
    4,
    COLOR_TEXT
);

snprintf(
    value,
    sizeof(value),
    "GW %s  RSSI %d DBM",
    data->gateway,
    (int)data->rssi_dbm
);

draw_text(
    40,
    860,
    value,
    2,
    COLOR_TEXT
);

/*
 * COGNITIVE
 */
draw_horizontal_line(
    40,
    920,
    640,
    2,
    COLOR_TEXT
);

draw_text(
    40,
    950,
    "COGNITIVE",
    2,
    COLOR_TEXT
);

snprintf(
    value,
    sizeof(value),
    "STATE ............ %s",
    data->cognitive_connected
        ? "CONNECTED"
        : "WAITING"
);

draw_text(
    40,
    995,
    value,
    2,
    COLOR_TEXT
);

/*
 * The event and target fields provide integration visibility without
 * exposing transport details to the console. They represent only the
 * most recently consumed Semantic Event.
 */
/*
 * Semantic identifiers may be longer than the available console row.
 * Limit the rendered value explicitly so truncation is deterministic
 * and remains compatible with builds that treat warnings as errors.
 */
snprintf(
    value,
    sizeof(value),
    "EVENT ............ %.44s",
    data->cognitive_event
);

draw_text(
    40,
    1035,
    value,
    2,
    COLOR_TEXT
);

snprintf(
    value,
    sizeof(value),
    "TARGET ........... %.44s",
    data->cognitive_target
);

draw_text(
    40,
    1070,
    value,
    2,
    COLOR_TEXT
);
    return tab5_platform_draw_bitmap(
        0,
        0,
        CONSOLE_W,
        CONSOLE_H,
        frame
    );
}