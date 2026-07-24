/******************************************************************************
 * @file    screen_runtime.h
 * @brief   Public interface for the Runtime dashboard view.
 *
 * The Runtime view renders operational service status and active context
 * inside a parent container supplied by the UI Manager.
 *
 * Architectural contract:
 *  - The UI Manager owns screen navigation and visibility.
 *  - This module owns only its internal widgets.
 *  - Runtime values are updated without recreating the widget hierarchy.
 ******************************************************************************/

#pragma once

#include "lvgl.h"
#include "runtime/runtime_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the Runtime dashboard inside the supplied parent container.
 *
 * @param parent LVGL container owned by the UI Manager.
 *
 * Preconditions:
 *  - LVGL must be initialized.
 *  - The caller must own the BSP display lock.
 *  - The function must be called only once.
 */
void screen_runtime_create(lv_obj_t *parent);

/**
 * @brief Updates the Runtime dashboard from an immutable status model.
 *
 * @param status Runtime state to display.
 *
 * Preconditions:
 *  - screen_runtime_create() must already have completed.
 *  - The caller must own the BSP display lock.
 */
void screen_runtime_update(const runtime_status_t *status);

#ifdef __cplusplus
}
#endif