#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Primitive visual effects.
 */
int rgb_effects_red(void);
int rgb_effects_green(void);
int rgb_effects_blue(void);
int rgb_effects_white(void);
int rgb_effects_off(void);

/*
 * Semantic visual effects.
 */
int rgb_effects_boot(void);
int rgb_effects_idle(void);
int rgb_effects_presence_detected(void);
int rgb_effects_identity_authenticated(void);
int rgb_effects_processing(void);
int rgb_effects_error(void);

/*
 * User profile lighting effects.
 */
int rgb_effects_profile_claudio(void);
int rgb_effects_profile_herminio(void);
int rgb_effects_profile_mariana(void);
int rgb_effects_profile_visitor(void);

#ifdef __cplusplus
}
#endif
