/******************************************************************************
 * @file    ui_manager.c
 * @brief   Owns the console shell, navigation and screen lifecycle.
 *
 * The UI Manager is the single coordination point for all console views.
 * Screens are created once, updated from a shared immutable model and hidden
 * or revealed without reallocating their widget trees.
 ******************************************************************************/

#include "ui_manager.h"

#include <stdint.h>

#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "lvgl.h"

#include "runtime/runtime_status.h"
#include "screen_health.h"
#include "screen_log.h"
#include "screen_network.h"
#include "screen_runtime.h"
#include "screen_state.h"

#define SCREEN_BACKGROUND_COLOR  0x0D1117
#define CARD_BACKGROUND_COLOR    0x161B22
#define PRIMARY_TEXT_COLOR       0xFFFFFF
#define STATUS_READY_COLOR       0x2EA043
#define STATUS_ERROR_COLOR       0xF85149
#define NAV_ACTIVE_COLOR         0x1F6FEB

typedef enum
{
    UI_VIEW_RUNTIME = 0,
    UI_VIEW_STATE,
    UI_VIEW_HEALTH,
    UI_VIEW_NETWORK,
    UI_VIEW_LOG,
    UI_VIEW_COUNT
} ui_view_t;

static const char *TAG = "ui_manager";

static runtime_status_t s_runtime_status;

/*
 * Temporary integration packet used only to validate the complete JSON path.
 * The physical transport will replace this source without changing the parser,
 * runtime model or screen modules.
 */
static const char *DEVELOPMENT_RUNTIME_JSON =
    "{"
        "\"type\":\"runtime_status\","
        "\"services\":{"
            "\"linux\":true,"
            "\"stackflow\":true,"
            "\"mcp\":true,"
            "\"semantic\":true"
        "},"
        "\"identity\":{"
            "\"user\":\"Claudio\","
            "\"context\":\"Laboratory\""
        "},"
        "\"cognitive\":{"
            "\"state\":\"READY\","
            "\"last_event\":\"welcome_researcher\","
            "\"last_rx\":\"JSON CONTRACT\""
        "},"
        "\"health\":{"
            "\"cpu_percent\":31,"
            "\"ram_used_mb\":418,"
            "\"ram_total_mb\":1024,"
            "\"temperature_c\":50,"
            "\"uptime\":\"02h 26m\""
        "},"
        "\"network\":{"
            "\"connected\":true,"
            "\"heartbeat\":true,"
            "\"rssi_dbm\":-47,"
            "\"ip\":\"192.168.77.20\","
            "\"runtime_link\":\"AX630C JSON\""
        "},"
        "\"events\":["
            "\"09:31 Presence detected\","
            "\"09:31 Identity authenticated\","
            "\"09:31 Context built\","
            "\"09:31 Semantic event\","
            "\"09:31 Welcome dispatched\""
        "]"
    "}";

static lv_obj_t *s_view_containers[UI_VIEW_COUNT];
static lv_obj_t *s_navigation_buttons[UI_VIEW_COUNT];
static lv_obj_t *s_status_dot;
static lv_obj_t *s_status_label;
static ui_view_t s_active_view = UI_VIEW_RUNTIME;

static void style_shell_container(lv_obj_t *object)
{
    lv_obj_set_style_radius(object, 8, 0);
    lv_obj_set_style_bg_color(
        object,
        lv_color_hex(CARD_BACKGROUND_COLOR),
        0);
    lv_obj_set_style_bg_opa(object, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(object, 0, 0);
    lv_obj_set_style_pad_all(object, 0, 0);
    lv_obj_clear_flag(object, LV_OBJ_FLAG_SCROLLABLE);
}

static void update_header_status(const runtime_status_t *status)
{
    const bool overall_ready =
        status->linux_ok &&
        status->stackflow_ok &&
        status->mcp_ok &&
        status->semantic_ok;

    const uint32_t semantic_color =
        overall_ready ? STATUS_READY_COLOR : STATUS_ERROR_COLOR;

    lv_label_set_text(
        s_status_label,
        overall_ready ? "READY" : "DEGRADED");

    lv_obj_set_style_text_color(
        s_status_label,
        lv_color_hex(semantic_color),
        0);

    lv_obj_set_style_bg_color(
        s_status_dot,
        lv_color_hex(semantic_color),
        0);
}

static void update_navigation_style(void)
{
    for (size_t index = 0; index < UI_VIEW_COUNT; ++index) {
        lv_obj_set_style_bg_color(
            s_navigation_buttons[index],
            lv_color_hex(
                index == s_active_view
                    ? NAV_ACTIVE_COLOR
                    : CARD_BACKGROUND_COLOR),
            0);
    }
}

/**
 * @brief Activates one existing view without destroying any widgets.
 *
 * This lifecycle policy prevents repeated allocation, preserves references and
 * produces immediate transitions on resource-constrained hardware.
 */
static void show_view(ui_view_t view)
{
    if (view >= UI_VIEW_COUNT) {
        return;
    }

    s_active_view = view;

    for (size_t index = 0; index < UI_VIEW_COUNT; ++index) {
        if (index == view) {
            lv_obj_remove_flag(
                s_view_containers[index],
                LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(
                s_view_containers[index],
                LV_OBJ_FLAG_HIDDEN);
        }
    }

    update_navigation_style();
}

static void navigation_event_cb(lv_event_t *event)
{
    if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
        return;
    }

    const ui_view_t requested_view =
        (ui_view_t)(intptr_t)lv_event_get_user_data(event);

    show_view(requested_view);
}

static lv_obj_t *create_navigation_button(
    lv_obj_t *parent,
    const char *text,
    ui_view_t target_view,
    lv_coord_t x)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, 54, 32);
    lv_obj_align(button, LV_ALIGN_LEFT_MID, x, 0);
    lv_obj_set_style_radius(button, 6, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);

    lv_obj_add_event_cb(
        button,
        navigation_event_cb,
        LV_EVENT_CLICKED,
        (void *)(intptr_t)target_view);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(
        label,
        lv_color_hex(PRIMARY_TEXT_COLOR),
        0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_center(label);

    return button;
}

static lv_obj_t *create_view_container(lv_obj_t *screen)
{
    lv_obj_t *container = lv_obj_create(screen);
    lv_obj_set_size(container, 320, 158);
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    return container;
}

static void create_console_shell(void)
{
    static const char *button_labels[UI_VIEW_COUNT] = {
        "Run",
        "State",
        "Health",
        "Net",
        "Log"
    };

    static const lv_coord_t button_positions[UI_VIEW_COUNT] = {
        4,
        61,
        118,
        175,
        232
    };

    lv_obj_t *screen = lv_screen_active();

    lv_obj_clean(screen);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(
        screen,
        lv_color_hex(SCREEN_BACKGROUND_COLOR),
        0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, 296, 42);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 6);
    style_shell_container(header);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "Cognitive Runtime");
    lv_obj_set_style_text_color(
        title,
        lv_color_hex(PRIMARY_TEXT_COLOR),
        0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 12, 0);

    s_status_dot = lv_obj_create(header);
    lv_obj_set_size(s_status_dot, 10, 10);
    lv_obj_align(s_status_dot, LV_ALIGN_RIGHT_MID, -64, 0);
    lv_obj_set_style_radius(s_status_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(s_status_dot, 0, 0);
    lv_obj_set_style_bg_opa(s_status_dot, LV_OPA_COVER, 0);
    lv_obj_clear_flag(s_status_dot, LV_OBJ_FLAG_SCROLLABLE);

    s_status_label = lv_label_create(header);
    lv_obj_set_style_text_font(
        s_status_label,
        &lv_font_montserrat_14,
        0);
    lv_obj_align(s_status_label, LV_ALIGN_RIGHT_MID, -10, 0);

    for (size_t index = 0; index < UI_VIEW_COUNT; ++index) {
        s_view_containers[index] = create_view_container(screen);
    }

    lv_obj_t *navigation_bar = lv_obj_create(screen);
    lv_obj_set_size(navigation_bar, 296, 38);
    lv_obj_align(navigation_bar, LV_ALIGN_BOTTOM_MID, 0, -2);
    style_shell_container(navigation_bar);

    for (size_t index = 0; index < UI_VIEW_COUNT; ++index) {
        s_navigation_buttons[index] = create_navigation_button(
            navigation_bar,
            button_labels[index],
            (ui_view_t)index,
            button_positions[index]);
    }
}

void ui_manager_init(void)
{
    ESP_LOGI(TAG, "Initializing complete console navigation");

    if (!runtime_status_init_mock(&s_runtime_status)) {
        ESP_LOGE(TAG, "Unable to initialize runtime model");
        return;
    }

    if (!runtime_status_apply_json(
        &s_runtime_status,
        DEVELOPMENT_RUNTIME_JSON)) {
    ESP_LOGE(TAG, "Development runtime JSON rejected");
    return;
    }

    ESP_LOGI(TAG, "Runtime JSON contract validated");

    if (!bsp_display_lock(0)) {
        ESP_LOGE(TAG, "Unable to acquire display lock");
        return;
    }

    create_console_shell();

    screen_runtime_create(s_view_containers[UI_VIEW_RUNTIME]);
    screen_state_create(s_view_containers[UI_VIEW_STATE]);
    screen_health_create(s_view_containers[UI_VIEW_HEALTH]);
    screen_network_create(s_view_containers[UI_VIEW_NETWORK]);
    screen_log_create(s_view_containers[UI_VIEW_LOG]);

    screen_runtime_update(&s_runtime_status);
    screen_state_update(&s_runtime_status);
    screen_health_update(&s_runtime_status);
    screen_network_update(&s_runtime_status);
    screen_log_update(&s_runtime_status);

    update_header_status(&s_runtime_status);
    show_view(UI_VIEW_RUNTIME);

    bsp_display_unlock();

    ESP_LOGI(TAG, "Five console views ready");
}

bool ui_manager_update_runtime_status(
    const runtime_status_t *status)
{
    if (status == NULL) {
        return false;
    }

    if (!bsp_display_lock(1000)) {
        ESP_LOGE(TAG, "Unable to acquire display lock for runtime update");
        return false;
    }

    /*
     * The UI Manager owns its local snapshot so callers may reuse or release
     * their source buffer immediately after this function returns.
     */
    s_runtime_status = *status;

    screen_runtime_update(&s_runtime_status);
    screen_state_update(&s_runtime_status);
    screen_health_update(&s_runtime_status);
    screen_network_update(&s_runtime_status);
    screen_log_update(&s_runtime_status);
    update_header_status(&s_runtime_status);

    bsp_display_unlock();
    return true;
}

