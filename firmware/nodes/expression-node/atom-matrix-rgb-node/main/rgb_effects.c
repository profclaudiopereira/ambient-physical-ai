#include "rgb_effects.h"

#include "rgb_controller.h"

/*
 * Atom Matrix platform calibration.
 *
 * The semantic behavior is identical to the validated RGB Strip Node.
 * However, the 5 x 5 Atom Matrix requires platform-specific RGB values
 * to reproduce the same perceived ambient colors.
 *
 * All visual calibration values are centralized in this section so that
 * future adjustments can be performed without changing the effect logic.
 */

/*
 * Generic semantic-state levels.
 */
#define RGB_LEVEL_VERY_LOW     2
#define RGB_LEVEL_LOW          4
#define RGB_LEVEL_MEDIUM       8

/*
 * Atom Matrix calibrated user profiles.
 *
 * Claudio:
 * Neutral-cool, low-intensity soft blue.
 */
#define PROFILE_CLAUDIO_R      0
#define PROFILE_CLAUDIO_G      2
#define PROFILE_CLAUDIO_B     10

/*
 * Herminio:
 * Warm, medium-intensity yellow/amber.
 */
#define PROFILE_HERMINIO_R    10
#define PROFILE_HERMINIO_G     8
#define PROFILE_HERMINIO_B     0

/*
 * Mariana:
 * Neutral, medium-high warm white.
 */
#define PROFILE_MARIANA_R     12
#define PROFILE_MARIANA_G     11
#define PROFILE_MARIANA_B      9

/*
 * Visitor:
 * Neutral, medium-intensity soft cyan.
 */
#define PROFILE_VISITOR_R      0
#define PROFILE_VISITOR_G     10
#define PROFILE_VISITOR_B     12

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
 * Boot: neutral low-intensity white during initialization.
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
 * Generic authenticated-identity fallback.
 *
 * Normal profile-aware operation selects one of the dedicated user profile
 * functions through the Expression Processor.
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
 * Authenticated-user ambient lighting profiles
 */

int rgb_effects_profile_claudio(void)
{
    return rgb_controller_set_all(
        PROFILE_CLAUDIO_R,
        PROFILE_CLAUDIO_G,
        PROFILE_CLAUDIO_B
    );
}

int rgb_effects_profile_herminio(void)
{
    return rgb_controller_set_all(
        PROFILE_HERMINIO_R,
        PROFILE_HERMINIO_G,
        PROFILE_HERMINIO_B
    );
}

int rgb_effects_profile_mariana(void)
{
    return rgb_controller_set_all(
        PROFILE_MARIANA_R,
        PROFILE_MARIANA_G,
        PROFILE_MARIANA_B
    );
}

int rgb_effects_profile_visitor(void)
{
    return rgb_controller_set_all(
        PROFILE_VISITOR_R,
        PROFILE_VISITOR_G,
        PROFILE_VISITOR_B
    );
}
