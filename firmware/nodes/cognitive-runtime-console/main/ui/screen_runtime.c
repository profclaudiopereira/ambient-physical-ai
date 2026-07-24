/******************************************************************************
 * @file    screen_runtime.c
 * @brief   Implements the Cognitive Runtime operational dashboard.
 *
 * This view presents:
 *  - Runtime service availability.
 *  - Active user identity.
 *  - Active environmental context.
 *  - Communication freshness.
 *
 * The module is presentation-only. It does not communicate with the AX630C,
 * access hardware drivers or control navigation.
 ******************************************************************************/

#include "screen_runtime.h"

#define CARD_BACKGROUND_COLOR    0x161B22
#define PRIMARY_TEXT_COLOR       0xFFFFFF
#define SECONDARY_TEXT_COLOR     0xB8C0CC
#define STATUS_READY_COLOR       0x2EA043
#define STATUS_ERROR_COLOR       0xF85149

static lv_obj_t *s_linux_value;
static lv_obj_t *s_stackflow_value;
static lv_obj_t *s_mcp_value;
static lv_obj_t *s_semantic_value;
static lv_obj_t *s_user_value;
static lv_obj_t *s_context_value;
static lv_obj_t *s_last_rx_value;
static bool s_screen_created;

/**
 * @brief Applies the standard visual treatment used by dashboard cards.
 *
 * Centralizing this styling keeps visual behavior consistent and avoids
 * duplicated configuration across card instances.
 */
static void style_dashboard_card(lv_obj_t *card)
{
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(CARD_BACKGROUND_COLOR), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
}

/**
 * @brief Creates a label/value row and returns the updateable value widget.
 */
static lv_obj_t *create_value_row(
    lv_obj_t *parent,
    const char *label_text,
    const char *value_text,
    lv_coord_t y)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, label_text);
    lv_obj_set_style_text_color(label, lv_color_hex(SECONDARY_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 16, y);

    lv_obj_t *value = lv_label_create(parent);
    lv_label_set_text(value, value_text);
    lv_obj_set_style_text_color(value, lv_color_hex(PRIMARY_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_14, 0);
    lv_obj_align(value, LV_ALIGN_TOP_RIGHT, -16, y);

    return value;
}

/**
 * @brief Updates a service label with semantic text and color.
 */
static void update_service_value(lv_obj_t *label, bool available)
{
    if (label == NULL) {
        return;
    }

    lv_label_set_text(label, available ? "OK" : "ERROR");
    lv_obj_set_style_text_color(
        label,
        lv_color_hex(available ? STATUS_READY_COLOR : STATUS_ERROR_COLOR),
        0);
}

void screen_runtime_create(lv_obj_t *parent)
{
    if (parent == NULL || s_screen_created) {
        return;
    }

    lv_obj_set_style_bg_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(parent, 0, 0);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *services_card = lv_obj_create(parent);
    lv_obj_set_size(services_card, 296, 88);
    lv_obj_align(services_card, LV_ALIGN_TOP_MID, 0, 4);
    style_dashboard_card(services_card);

    s_linux_value =
        create_value_row(services_card, "Linux", "---", 8);
    s_stackflow_value =
        create_value_row(services_card, "StackFlow", "---", 28);
    s_mcp_value =
        create_value_row(services_card, "MCP", "---", 48);
    s_semantic_value =
        create_value_row(services_card, "Semantic", "---", 68);

    lv_obj_t *context_card = lv_obj_create(parent);
    lv_obj_set_size(context_card, 296, 66);
    lv_obj_align(context_card, LV_ALIGN_BOTTOM_MID, 0, -4);
    style_dashboard_card(context_card);

    s_user_value =
        create_value_row(context_card, "User", "---", 7);
    s_context_value =
        create_value_row(context_card, "Context", "---", 27);
    s_last_rx_value =
        create_value_row(context_card, "Last RX", "---", 47);

    s_screen_created = true;
}

void screen_runtime_update(const runtime_status_t *status)
{
    if (!s_screen_created || status == NULL) {
        return;
    }

    update_service_value(s_linux_value, status->linux_ok);
    update_service_value(s_stackflow_value, status->stackflow_ok);
    update_service_value(s_mcp_value, status->mcp_ok);
    update_service_value(s_semantic_value, status->semantic_ok);

    lv_label_set_text(s_user_value, status->user);
    lv_label_set_text(s_context_value, status->context);
    lv_label_set_text(s_last_rx_value, status->last_rx);
}