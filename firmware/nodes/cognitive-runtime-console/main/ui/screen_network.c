/******************************************************************************
 * @file    screen_network.c
 * @brief   Implements network and AX630C link visualization.
 *
 * This module displays connection state, address, signal level and heartbeat.
 * It does not own Wi-Fi, UART or runtime communication resources.
 ******************************************************************************/

#include "screen_network.h"

#include <stdio.h>

#define CARD_BACKGROUND_COLOR  0x161B22
#define PRIMARY_TEXT_COLOR     0xFFFFFF
#define SECONDARY_TEXT_COLOR   0xB8C0CC
#define STATUS_OK_COLOR        0x2EA043
#define STATUS_ERROR_COLOR     0xF85149

static lv_obj_t *s_network_value;
static lv_obj_t *s_ip_value;
static lv_obj_t *s_rssi_value;
static lv_obj_t *s_link_value;
static lv_obj_t *s_heartbeat_value;
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

static void update_boolean_value(lv_obj_t *label, bool value)
{
    lv_label_set_text(label, value ? "OK" : "OFFLINE");
    lv_obj_set_style_text_color(
        label,
        lv_color_hex(value ? STATUS_OK_COLOR : STATUS_ERROR_COLOR),
        0);
}

void screen_network_create(lv_obj_t *parent)
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
    style_card(card);

    s_network_value = create_field(card, "Network", 10);
    s_ip_value = create_field(card, "IP", 36);
    s_rssi_value = create_field(card, "RSSI", 62);
    s_link_value = create_field(card, "Runtime Link", 88);
    s_heartbeat_value = create_field(card, "Heartbeat", 114);

    s_created = true;
}

void screen_network_update(const runtime_status_t *status)
{
    if (!s_created || status == NULL) {
        return;
    }

    char buffer[24];

    update_boolean_value(s_network_value, status->network_connected);
    lv_label_set_text(s_ip_value, status->ip_address);

    snprintf(buffer, sizeof(buffer), "%d dBm", status->rssi_dbm);
    lv_label_set_text(s_rssi_value, buffer);

    lv_label_set_text(s_link_value, status->runtime_link);
    update_boolean_value(s_heartbeat_value, status->heartbeat_ok);
}