/******************************************************************************
 * @file    screen_log.c
 * @brief   Implements the recent Cognitive Runtime event list.
 *
 * The screen displays a bounded event history suitable for integration and
 * demonstration. Event storage and ordering remain responsibilities of the
 * runtime model provider.
 ******************************************************************************/

#include "screen_log.h"

#define CARD_BACKGROUND_COLOR  0x161B22
#define PRIMARY_TEXT_COLOR     0xFFFFFF
#define SECONDARY_TEXT_COLOR   0x8B949E

static lv_obj_t *s_log_labels[RUNTIME_LOG_ENTRY_COUNT];
static bool s_created;

void screen_log_create(lv_obj_t *parent)
{
    if (parent == NULL || s_created) {
        return;
    }

    lv_obj_set_style_bg_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(parent, 0, 0);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, 296, 150);
    lv_obj_align(card, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(CARD_BACKGROUND_COLOR), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(card);
    lv_label_set_text(title, "Recent Events");
    lv_obj_set_style_text_color(title, lv_color_hex(SECONDARY_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 14, 8);

    for (size_t index = 0; index < RUNTIME_LOG_ENTRY_COUNT; ++index) {
        s_log_labels[index] = lv_label_create(card);
        lv_label_set_text(s_log_labels[index], "---");
        lv_obj_set_style_text_color(
            s_log_labels[index],
            lv_color_hex(PRIMARY_TEXT_COLOR),
            0);
        lv_obj_set_style_text_font(
            s_log_labels[index],
            &lv_font_montserrat_14,
            0);
        lv_obj_set_width(s_log_labels[index], 266);
        lv_label_set_long_mode(s_log_labels[index], LV_LABEL_LONG_DOT);
        lv_obj_align(
            s_log_labels[index],
            LV_ALIGN_TOP_LEFT,
            14,
            34 + ((lv_coord_t)index * 22));
    }

    s_created = true;
}

void screen_log_update(const runtime_status_t *status)
{
    if (!s_created || status == NULL) {
        return;
    }

    for (size_t index = 0; index < RUNTIME_LOG_ENTRY_COUNT; ++index) {
        lv_label_set_text(
            s_log_labels[index],
            status->event_log[index]);
    }
}