#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Primitive visual effects.
 *
 * These functions directly represent basic colors and are kept available
 * for diagnostics and simple internal composition.
 */
int rgb_effects_red(void);
int rgb_effects_green(void);
int rgb_effects_blue(void);
int rgb_effects_white(void);
int rgb_effects_off(void);

/*
 * Semantic visual effects.
 *
 * These functions represent system states and hide RGB decisions from the
 * Semantic Consumer and Expression Processor.
 */
int rgb_effects_boot(void);
int rgb_effects_idle(void);
int rgb_effects_presence_detected(void);
int rgb_effects_identity_authenticated(void);
int rgb_effects_processing(void);
int rgb_effects_error(void);

/*
 * Authenticated-user ambient lighting profiles.
 */
int rgb_effects_profile_claudio(void);
int rgb_effects_profile_herminio(void);
int rgb_effects_profile_mariana(void);
int rgb_effects_profile_visitor(void);

#ifdef __cplusplus
}
#endif
