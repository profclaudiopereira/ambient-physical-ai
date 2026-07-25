#include "rgb_effects.h"

#include "rgb_controller.h"

#define RGB_INTENSITY 8

/*
 * Primitive visual effects
 */

int rgb_effects_red(void)
{
    return rgb_controller_set_all(
        RGB_INTENSITY,
        0,
        0
    );
}

int rgb_effects_green(void)
{
    return rgb_controller_set_all(
        0,
        RGB_INTENSITY,
        0
    );
}

int rgb_effects_blue(void)
{
    return rgb_controller_set_all(
        0,
        0,
        RGB_INTENSITY
    );
}

int rgb_effects_white(void)
{
    return rgb_controller_set_all(
        RGB_INTENSITY,
        RGB_INTENSITY,
        RGB_INTENSITY
    );
}

int rgb_effects_off(void)
{
    return rgb_controller_clear();
}

/*
 * Semantic visual effects
 *
 * Initial mappings are intentionally simple.
 * Future animations can replace these implementations without changing
 * callers such as the Semantic Consumer.
 */

int rgb_effects_boot(void)
{
    return rgb_effects_off();
}

int rgb_effects_idle(void)
{
    return rgb_effects_off();
}
int rgb_effects_presence_detected(void)
{
    return rgb_effects_green();
}

int rgb_effects_identity_authenticated(void)
{
    return rgb_effects_blue();
}

int rgb_effects_processing(void)
{
    return rgb_effects_white();
}

int rgb_effects_error(void)
{
    return rgb_effects_red();
}