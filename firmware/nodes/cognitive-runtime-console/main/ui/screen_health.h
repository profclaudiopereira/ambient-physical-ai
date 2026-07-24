/******************************************************************************
 * @file    screen_health.h
 * @brief   Public interface for the Runtime Health view.
 *
 * The Health view presents resource and platform telemetry without accessing
 * operating-system or transport services directly.
 ******************************************************************************/

#pragma once

#include "lvgl.h"
#include "runtime/runtime_status.h"

#ifdef __cplusplus
extern "C" {
#endif

void screen_health_create(lv_obj_t *parent);
void screen_health_update(const runtime_status_t *status);

#ifdef __cplusplus
}
#endif