/******************************************************************************
 * @file    ui_manager.h
 * @brief   Public interface for the Cognitive Runtime Console UI Manager.
 *
 * The UI Manager owns the persistent shell, screen lifecycle, navigation and
 * synchronized presentation of runtime snapshots.
 ******************************************************************************/

#pragma once

#include <stdbool.h>

#include "runtime/runtime_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the complete console interface.
 */
void ui_manager_init(void);

/**
 * @brief Replaces the currently presented runtime snapshot.
 *
 * The function obtains the BSP display lock before updating LVGL widgets and
 * may therefore be called later by a transport task outside the LVGL context.
 *
 * @param status Immutable runtime snapshot to present.
 *
 * @return true when the interface is updated successfully.
 */
bool ui_manager_update_runtime_status(
    const runtime_status_t *status);

#ifdef __cplusplus
}
#endif