#pragma once

#include "esp_err.h"
#include "semantic_event_receiver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Renders the latest normalized global and personal context.
 *
 * The caller must select the Mini OLED PaHub channel before invoking this
 * function. The presenter does not own I2C routing or business decisions.
 */
esp_err_t oled_context_presenter_render(
    const ambient_context_snapshot_t *context
);

/**
 * Forces the next render even when the effective content is unchanged.
 */
void oled_context_presenter_invalidate(void);

#ifdef __cplusplus
}
#endif
