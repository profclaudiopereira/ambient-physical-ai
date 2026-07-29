#include "oled_context_presenter.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "oled_sh1107.h"

static uint32_t s_last_signature = 0;
static bool s_force_render = true;

static uint32_t hash_bytes(
    uint32_t hash,
    const void *data,
    size_t length
)
{
    const unsigned char *bytes = (const unsigned char *)data;

    for (size_t index = 0; index < length; index++) {
        hash ^= bytes[index];
        hash *= 16777619U;
    }

    return hash;
}

static uint32_t context_signature(
    const ambient_context_snapshot_t *context
)
{
    uint32_t hash = 2166136261U;

    hash = hash_bytes(hash, context, sizeof(*context));

    return hash;
}

static void uppercase_ascii(char *text)
{
    if (text == NULL) {
        return;
    }

    while (*text != '\0') {
        if (*text >= 'a' && *text <= 'z') {
            *text = (char)(*text - ('a' - 'A'));
        }
        text++;
    }
}

static void format_line(
    char destination[OLED_SH1107_TEXT_COLUMNS + 1],
    const char *format,
    ...
)
{
    va_list args;
    va_start(args, format);

    char temporary[64];
    vsnprintf(temporary, sizeof(temporary), format, args);

    va_end(args);

    uppercase_ascii(temporary);

    snprintf(
        destination,
        OLED_SH1107_TEXT_COLUMNS + 1,
        "%.10s",
        temporary
    );
}

static esp_err_t draw_line(uint8_t page, const char *text)
{
    esp_err_t ret = oled_sh1107_clear_page(page);
    if (ret != ESP_OK) {
        return ret;
    }

    return oled_sh1107_draw_text(page, 0, text);
}

static esp_err_t render_waiting(void)
{
    esp_err_t ret = oled_sh1107_clear();
    if (ret != ESP_OK) return ret;

    ret = draw_line(1, "CONTEXT");
    if (ret != ESP_OK) return ret;

    ret = draw_line(4, "WAITING");
    if (ret != ESP_OK) return ret;

    return draw_line(7, "AX630C");
}

static esp_err_t render_stale(void)
{
    esp_err_t ret = oled_sh1107_clear();
    if (ret != ESP_OK) return ret;

    ret = draw_line(1, "CONTEXT");
    if (ret != ESP_OK) return ret;

    ret = draw_line(4, "STALE");
    if (ret != ESP_OK) return ret;

    return draw_line(7, "NO UPDATE");
}

esp_err_t oled_context_presenter_render(
    const ambient_context_snapshot_t *context
)
{
    if (context == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t signature = context_signature(context);

    if (!s_force_render && signature == s_last_signature) {
        return ESP_OK;
    }

    s_force_render = false;
    s_last_signature = signature;

    if (!context->context_received) {
        return render_waiting();
    }

    if (context->stale) {
        return render_stale();
    }

    char line[OLED_SH1107_TEXT_COLUMNS + 1];

    esp_err_t ret = oled_sh1107_clear();
    if (ret != ESP_OK) return ret;

    if (context->global_available) {
        format_line(
            line,
            "%s",
            context->location[0] != '\0'
                ? context->location
                : "WEATHER"
        );
        ret = draw_line(0, line);
        if (ret != ESP_OK) return ret;

        format_line(
            line,
            "%.0fC %s",
            context->temperature_c,
            context->weather_summary
        );
        ret = draw_line(2, line);
        if (ret != ESP_OK) return ret;

        format_line(
            line,
            "UV %.1f %s",
            context->uv_index,
            context->uv_label
        );
        ret = draw_line(4, line);
        if (ret != ESP_OK) return ret;
    } else {
        ret = draw_line(0, "WEATHER");
        if (ret != ESP_OK) return ret;

        ret = draw_line(2, "NO DATA");
        if (ret != ESP_OK) return ret;
    }

    ret = draw_line(6, "----------");
    if (ret != ESP_OK) return ret;

    if (!context->authenticated) {
        ret = draw_line(8, "GLOBAL");
        if (ret != ESP_OK) return ret;

        return draw_line(10, "NO USER");
    }

    if (!context->personal_available) {
        ret = draw_line(8, "PERSONAL");
        if (ret != ESP_OK) return ret;

        return draw_line(10, "NO DATA");
    }

    format_line(line, "%s", context->personal_title);
    ret = draw_line(8, line);
    if (ret != ESP_OK) return ret;

    format_line(line, "%s", context->personal_value);
    ret = draw_line(10, line);
    if (ret != ESP_OK) return ret;

    if (context->personal_secondary[0] != '\0') {
        format_line(line, "%s", context->personal_secondary);
        ret = draw_line(12, line);
        if (ret != ESP_OK) return ret;
    }

    if (context->profile_id[0] != '\0') {
        format_line(line, "@%s", context->profile_id);
        ret = draw_line(15, line);
        if (ret != ESP_OK) return ret;
    }

    return ESP_OK;
}

void oled_context_presenter_invalidate(void)
{
    s_force_render = true;
}
