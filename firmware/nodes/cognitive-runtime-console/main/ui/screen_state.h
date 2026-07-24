/******************************************************************************
 * @file    screen_state.h
 * @brief   Public interface for the Cognitive State view.
 *
 * This view exposes the current cognitive state, active identity context and
 * most recent semantic event received by the console.
 ******************************************************************************/

#pragma once

#include "lvgl.h"
#include "runtime/runtime_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the Cognitive State view inside a parent container.
 *
 * @param parent LVGL container owned by the UI Manager.
 */
void screen_state_create(lv_obj_t *parent);

/**
 * @brief Updates Cognitive State widgets from the runtime model.
 *
 * @param status Immutable runtime state to display.
 */
void screen_state_update(const runtime_status_t *status);

#ifdef __cplusplus
}
#endif