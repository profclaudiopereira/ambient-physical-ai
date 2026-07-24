/******************************************************************************
 * @file    screen_state.c
 * @brief   Implements the Cognitive State visualization.
 *
 * This view translates internal runtime information into an operator-readable
 * representation of the system's current cognitive activity.
 *
 * Current milestone:
 *  - Uses the shared mock runtime model.
 *
 * Future integration:
 *  - The same widgets will display real state transitions received from the
 *    AX630C without changing the screen architecture.
 ******************************************************************************/

#include "screen_state.h"

#define CARD_BACKGROUND_COLOR    0x161B22
#define PRIMARY_TEXT_COLOR       0xFFFFFF
#define SECONDARY_TEXT_COLOR     0xB8C0CC
#define STATE_ACTIVE_COLOR       0x58A6FF

static lv_obj_t *s_state_value;
static lv_obj_t *s_user_value;
static lv_obj_t *s_context_value;
static lv_obj_t *s_event_value;
static bool s_screen_created;

/**
 * @brief Applies the shared card appearance used by the State view.
 */
static void style_state_card(lv_obj_t *card)
{
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(CARD_BACKGROUND_COLOR), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
}

/**
 * @brief Creates a compact field with a caption and updateable value.
 */
static lv_obj_t *create_state_field(
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
    lv_obj_set_style_text_font(
        value_label,
        &lv_font_montserrat_14,
        0);
    lv_obj_align(value_label, LV_ALIGN_TOP_RIGHT, -16, y);

    return value_label;
}

void screen_state_create(lv_obj_t *parent)
{
    if (parent == NULL || s_screen_created) {
        return;
    }

    lv_obj_set_style_bg_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(parent, 0, 0);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *state_card = lv_obj_create(parent);
    lv_obj_set_size(state_card, 296, 58);
    lv_obj_align(state_card, LV_ALIGN_TOP_MID, 0, 4);
    style_state_card(state_card);

    lv_obj_t *state_caption = lv_label_create(state_card);
    lv_label_set_text(state_caption, "Current Cognitive State");
    lv_obj_set_style_text_color(
        state_caption,
        lv_color_hex(SECONDARY_TEXT_COLOR),
        0);
    lv_obj_set_style_text_font(
        state_caption,
        &lv_font_montserrat_14,
        0);
    lv_obj_align(state_caption, LV_ALIGN_TOP_LEFT, 16, 8);

    lv_obj_t *state_dot = lv_obj_create(state_card);
    lv_obj_set_size(state_dot, 12, 12);
    lv_obj_align(state_dot, LV_ALIGN_BOTTOM_LEFT, 16, -9);
    lv_obj_set_style_radius(state_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(
        state_dot,
        lv_color_hex(STATE_ACTIVE_COLOR),
        0);
    lv_obj_set_style_bg_opa(state_dot, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(state_dot, 0, 0);
    lv_obj_clear_flag(state_dot, LV_OBJ_FLAG_SCROLLABLE);

    s_state_value = lv_label_create(state_card);
    lv_label_set_text(s_state_value, "---");
    lv_obj_set_style_text_color(
        s_state_value,
        lv_color_hex(STATE_ACTIVE_COLOR),
        0);
    lv_obj_set_style_text_font(
        s_state_value,
        &lv_font_montserrat_14,
        0);
    lv_obj_align(s_state_value, LV_ALIGN_BOTTOM_LEFT, 36, -7);

    lv_obj_t *details_card = lv_obj_create(parent);
    lv_obj_set_size(details_card, 296, 94);
    lv_obj_align(details_card, LV_ALIGN_BOTTOM_MID, 0, -4);
    style_state_card(details_card);

    s_user_value =
        create_state_field(details_card, "User", 10);
    s_context_value =
        create_state_field(details_card, "Context", 36);
    s_event_value =
        create_state_field(details_card, "Last Event", 62);

    /*
     * Long semantic event names must remain inside the card rather than
     * expanding the layout beyond the display boundaries.
     */
    lv_label_set_long_mode(s_event_value, LV_LABEL_LONG_DOT);
    lv_obj_set_width(s_event_value, 150);

    s_screen_created = true;
}

void screen_state_update(const runtime_status_t *status)
{
    if (!s_screen_created || status == NULL) {
        return;
    }

    lv_label_set_text(s_state_value, status->cognitive_state);
    lv_label_set_text(s_user_value, status->user);
    lv_label_set_text(s_context_value, status->context);
    lv_label_set_text(s_event_value, status->last_event);
}