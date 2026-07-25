#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Primitive visual effects.
 *
 * These functions directly represent basic colors and are used internally
 * to compose higher-level semantic effects.
 */
int rgb_effects_red(void);
int rgb_effects_green(void);
int rgb_effects_blue(void);
int rgb_effects_white(void);
int rgb_effects_off(void);

/*
 * Semantic visual effects.
 *
 * These functions represent system states and semantic events.
 * They intentionally hide RGB color decisions from the rest of the node.
 */
int rgb_effects_boot(void);
int rgb_effects_idle(void);
int rgb_effects_presence_detected(void);
int rgb_effects_identity_authenticated(void);
int rgb_effects_processing(void);
int rgb_effects_error(void);

#ifdef __cplusplus
}
#endif