/******************************************************************************
 * @file    screen_log.h
 * @brief   Public interface for the recent runtime event view.
 ******************************************************************************/

#pragma once

#include "lvgl.h"
#include "runtime/runtime_status.h"

#ifdef __cplusplus
extern "C" {
#endif

void screen_log_create(lv_obj_t *parent);
void screen_log_update(const runtime_status_t *status);

#ifdef __cplusplus
}
#endif