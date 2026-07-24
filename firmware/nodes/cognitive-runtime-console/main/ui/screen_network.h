/******************************************************************************
 * @file    screen_network.h
 * @brief   Public interface for the Runtime Network view.
 ******************************************************************************/

#pragma once

#include "lvgl.h"
#include "runtime/runtime_status.h"

#ifdef __cplusplus
extern "C" {
#endif

void screen_network_create(lv_obj_t *parent);
void screen_network_update(const runtime_status_t *status);

#ifdef __cplusplus
}
#endif