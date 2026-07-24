/******************************************************************************
 * @file    screen_health.c
 * @brief   Implements Cognitive Runtime health visualization.
 *
 * This presentation-only module renders CPU, memory, temperature and uptime.
 * Telemetry acquisition remains the responsibility of the future AX630C data
 * provider.
 ******************************************************************************/

#include "screen_health.h"

#include <stdio.h>

#define CARD_BACKGROUND_COLOR  0x161B22
#define PRIMARY_TEXT_COLOR     0xFFFFFF
#define SECONDARY_TEXT_COLOR   0xB8C0CC
#define HEALTH_COLOR           0x2EA043

static lv_obj_t *s_cpu_value;
static lv_obj_t *s_ram_value;
static lv_obj_t *s_temperature_value;
static lv_obj_t *s_uptime_value;
static bool s_created;

static void style_card(lv_obj_t *card)
{
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(CARD_BACKGROUND_COLOR), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t *create_field(
    lv_obj_t *parent,
    const char *caption,
    lv_coord_t y)
{
    lv_obj_t *caption_label = lv_label_create(parent);
    lv_label_set_text(caption_label, caption);
    lv_obj_set_style_text_color(
        caption_label,
        lv_color_hex(SECONDARY_TEXT_COLOR),
        0);
    lv_obj_set_style_text_font(
        caption_label,
        &lv_font_montserrat_14,
        0);
    lv_obj_align(caption_label, LV_ALIGN_TOP_LEFT, 16, y);

    lv_obj_t *value_label = lv_label_create(parent);
    lv_label_set_text(value_label, "---");
    lv_obj_set_style_text_color(
        value_label,
        lv_color_hex(PRIMARY_TEXT_COLOR),
        0);
    lv_obj_set_style_text_font(value_label, &lv_font_montserrat_14, 0);
    lv_obj_align(value_label, LV_ALIGN_TOP_RIGHT, -16, y);

    return value_label;
}

void screen_health_create(lv_obj_t *parent)
{
    if (parent == NULL || s_created) {
        return;
    }

    lv_obj_set_style_bg_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(parent, 0, 0);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *summary = lv_obj_create(parent);
    lv_obj_set_size(summary, 296, 42);
    lv_obj_align(summary, LV_ALIGN_TOP_MID, 0, 4);
    style_card(summary);

    lv_obj_t *title = lv_label_create(summary);
    lv_label_set_text(title, "Runtime Health");
    lv_obj_set_style_text_color(title, lv_color_hex(PRIMARY_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);

    lv_obj_t *status = lv_label_create(summary);
    lv_label_set_text(status, "NOMINAL");
    lv_obj_set_style_text_color(status, lv_color_hex(HEALTH_COLOR), 0);
    lv_obj_set_style_text_font(status, &lv_font_montserrat_14, 0);
    lv_obj_align(status, LV_ALIGN_RIGHT_MID, -16, 0);

    lv_obj_t *details = lv_obj_create(parent);
    lv_obj_set_size(details, 296, 104);
    lv_obj_align(details, LV_ALIGN_BOTTOM_MID, 0, -4);
    style_card(details);

    s_cpu_value = create_field(details, "CPU", 10);
    s_ram_value = create_field(details, "RAM", 34);
    s_temperature_value = create_field(details, "Temperature", 58);
    s_uptime_value = create_field(details, "Uptime", 82);

    s_created = true;
}

void screen_health_update(const runtime_status_t *status)
{
    if (!s_created || status == NULL) {
        return;
    }

    char buffer[40];

    snprintf(buffer, sizeof(buffer), "%u %%", status->cpu_percent);
    lv_label_set_text(s_cpu_value, buffer);

    snprintf(
        buffer,
        sizeof(buffer),
        "%u / %u MB",
        status->ram_used_mb,
        status->ram_total_mb);
    lv_label_set_text(s_ram_value, buffer);

    snprintf(
        buffer,
        sizeof(buffer),
        "%d C",
        status->temperature_c);
    lv_label_set_text(s_temperature_value, buffer);

    lv_label_set_text(s_uptime_value, status->uptime);
}