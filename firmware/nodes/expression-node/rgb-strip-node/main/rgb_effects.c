#include "rgb_effects.h"

#include "rgb_controller.h"

/*
 * Conservative brightness levels for indoor demonstration.
 */
#define RGB_LEVEL_VERY_LOW     2
#define RGB_LEVEL_LOW          4
#define RGB_LEVEL_MEDIUM       8
#define RGB_LEVEL_MEDIUM_HIGH 12

/*
 * Primitive visual effects
 */

int rgb_effects_red(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_MEDIUM,
        0,
        0
    );
}

int rgb_effects_green(void)
{
    return rgb_controller_set_all(
        0,
        RGB_LEVEL_MEDIUM,
        0
    );
}

int rgb_effects_blue(void)
{
    return rgb_controller_set_all(
        0,
        0,
        RGB_LEVEL_MEDIUM
    );
}

int rgb_effects_white(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_MEDIUM,
        RGB_LEVEL_MEDIUM,
        RGB_LEVEL_MEDIUM
    );
}

int rgb_effects_off(void)
{
    return rgb_controller_clear();
}

/*
 * Semantic visual effects
 */

/*
 * Boot: neutral white indication during initialization.
 */
int rgb_effects_boot(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_LOW,
        RGB_LEVEL_LOW,
        RGB_LEVEL_LOW
    );
}

/*
 * Idle without presence: very soft blue.
 */
int rgb_effects_idle(void)
{
    return rgb_controller_set_all(
        0,
        0,
        RGB_LEVEL_VERY_LOW
    );
}

/*
 * Presence detected before authentication: soft yellow.
 */
int rgb_effects_presence_detected(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_LOW,
        RGB_LEVEL_LOW,
        0
    );
}

/*
 * Generic fallback for authenticated identity events.
 * Normal operation should use one of the profile-specific effects.
 */
int rgb_effects_identity_authenticated(void)
{
    return rgb_effects_profile_visitor();
}

/*
 * Processing: neutral low-intensity white.
 */
int rgb_effects_processing(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_LOW,
        RGB_LEVEL_LOW,
        RGB_LEVEL_LOW
    );
}

/*
 * Error: persistent red with limited intensity.
 */
int rgb_effects_error(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_LOW,
        0,
        0
    );
}

/*
 * User profile lighting effects
 */

/*
 * Claudio:
 * Neutral-cool temperature, low intensity, soft blue.
 */
int rgb_effects_profile_claudio(void)
{
    return rgb_controller_set_all(
        0,
        1,
        RGB_LEVEL_LOW
    );
}

/*
 * Herminio:
 * Warm temperature, medium intensity, yellow/amber.
 */
int rgb_effects_profile_herminio(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_MEDIUM,
        RGB_LEVEL_LOW,
        0
    );
}

/*
 * Mariana:
 * Neutral temperature, medium-high intensity, warm white.
 */
int rgb_effects_profile_mariana(void)
{
    return rgb_controller_set_all(
        RGB_LEVEL_MEDIUM_HIGH,
        10,
        7
    );
}

/*
 * Visitor:
 * Neutral temperature, medium intensity, soft cyan.
 */
int rgb_effects_profile_visitor(void)
{
    return rgb_controller_set_all(
        0,
        RGB_LEVEL_MEDIUM,
        RGB_LEVEL_MEDIUM
    );
}
