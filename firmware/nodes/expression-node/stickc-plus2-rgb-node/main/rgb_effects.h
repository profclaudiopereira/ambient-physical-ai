#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Primitive visual effects.
 *
 * These functions represent basic colors and provide reusable building
 * blocks for the normalized Cognitive Runtime states.
 */
int rgb_effects_red(void);
int rgb_effects_green(void);
int rgb_effects_blue(void);
int rgb_effects_white(void);
int rgb_effects_amber(void);
int rgb_effects_yellow(void);
int rgb_effects_purple(void);
int rgb_effects_cyan(void);
int rgb_effects_off(void);

/*
 * Cognitive Runtime visual states.
 *
 * Each function defines the visual representation of one normalized runtime
 * state. The effect layer owns color and label decisions, while the hardware
 * controller remains responsible only for synchronized display and LED output.
 */
int rgb_effects_idle(void);
int rgb_effects_presence(void);
int rgb_effects_listening(void);
int rgb_effects_thinking(void);
int rgb_effects_responding(void);
int rgb_effects_alert(void);
int rgb_effects_error(void);
int rgb_effects_offline(void);
int rgb_effects_learning(void);

#ifdef __cplusplus
}
#endif