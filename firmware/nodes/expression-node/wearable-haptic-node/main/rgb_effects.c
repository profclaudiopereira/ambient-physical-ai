#include "rgb_effects.h"
#include "vibrator.h"
#include "rgb_controller.h"

#define RGB_INTENSITY 8

/*
 * Primitive visual effects.
 *
 * These reusable colors remain available for diagnostics and future effects.
 */

int rgb_effects_red(void)
{
    return rgb_controller_set_all(RGB_INTENSITY, 0, 0);
}

int rgb_effects_green(void)
{
    return rgb_controller_set_all(0, RGB_INTENSITY, 0);
}

int rgb_effects_blue(void)
{
    return rgb_controller_set_all(0, 0, RGB_INTENSITY);
}

int rgb_effects_white(void)
{
    return rgb_controller_set_all(
        RGB_INTENSITY,
        RGB_INTENSITY,
        RGB_INTENSITY
    );
}

int rgb_effects_amber(void)
{
    return rgb_controller_set_all(RGB_INTENSITY, RGB_INTENSITY / 2, 0);
}

int rgb_effects_yellow(void)
{
    return rgb_controller_set_all(RGB_INTENSITY, RGB_INTENSITY, 0);
}

int rgb_effects_purple(void)
{
    return rgb_controller_set_all(RGB_INTENSITY, 0, RGB_INTENSITY);
}

int rgb_effects_cyan(void)
{
    return rgb_controller_set_all(0, RGB_INTENSITY, RGB_INTENSITY);
}

int rgb_effects_off(void)
{
    return rgb_controller_clear();
}

/*
 * Runtime State visual mappings.
 */

int rgb_effects_idle(void)
{
    return rgb_controller_present_state(
        "IDLE",
        0,
        RGB_INTENSITY,
        0
    );
}

int rgb_effects_presence(void)
{
    return rgb_controller_present_state(
        "PRESENCE",
        RGB_INTENSITY,
        RGB_INTENSITY,
        0
    );
}
int rgb_effects_listening(void)
{
    return rgb_controller_present_state("LISTENING", RGB_INTENSITY, 0, RGB_INTENSITY);
}

int rgb_effects_thinking(void)
{
    return rgb_controller_present_state(
        "THINKING",
        RGB_INTENSITY,
        RGB_INTENSITY,
        RGB_INTENSITY
    );
}

int rgb_effects_responding(void)
{
    const int result = rgb_controller_present_state(
        "RESPONDING",
        0,
        0,
        RGB_INTENSITY
    );

    if (result != 0) {
        return result;
    }

    /*
     * Wearable-specific haptic feedback.
     *
     * A single short pulse is emitted only when the Cognitive Runtime
     * enters the responding state.
     */
    vibrator_alert();

    return 0;
}

int rgb_effects_alert(void)
{
    return rgb_controller_present_state(
        "ALERT",
        RGB_INTENSITY,
        RGB_INTENSITY / 2,
        0
    );
}

int rgb_effects_error(void)
{
    return rgb_controller_present_state(
        "ERROR",
        RGB_INTENSITY,
        0,
        0
    );
}

int rgb_effects_offline(void)
{
    return rgb_controller_present_state(
        "OFFLINE",
        RGB_INTENSITY / 2,
        RGB_INTENSITY / 2,
        RGB_INTENSITY / 2
    );
}

int rgb_effects_learning(void)
{
    return rgb_controller_present_state(
        "LEARNING",
        RGB_INTENSITY,
        0,
        RGB_INTENSITY
    );
}