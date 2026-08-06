/**
 * @file profile_image_manager.cpp
 * @brief Profile image rendering implementation for the Identity Node.
 *
 * This module maps authenticated profile identifiers to embedded image
 * resources and renders the selected image on the M5Stack display.
 *
 * Images are compiled into the firmware and therefore do not require a
 * filesystem or external storage at runtime.
 */

#include "profile_image_manager.h"

#include <cstring>
#include "M5Unified.h"

#include "profile_images/claudio.h"
#include "profile_images/herminio.h"
#include "profile_images/mariana.h"
#include "profile_images/student.h"
#include "profile_images/unknown.h"

/**
 * @brief Internal implementation details.
 *
 * This anonymous namespace contains rendering constants and helper functions
 * that are intentionally private to this translation unit.
 */
namespace
{
constexpr int IMAGE_WIDTH = 96;
constexpr int IMAGE_HEIGHT = 96;
constexpr int IMAGE_RADIUS = 48;

/**
 * @brief Resolves a profile identifier to its embedded image resource.
 *
 * Unknown or null identifiers are mapped to the default placeholder image,
 * ensuring that rendering can always proceed safely.
 *
 * @param profile_id Null-terminated profile identifier.
 *
 * @return Pointer to the RGB565 image stored in program memory.
 */
const uint16_t* resolveProfileImage(const char* profile_id)
{
    if (profile_id == nullptr) {
        return profile_image_unknown;
    }
    // Match known profile identifiers to their corresponding embedded images.
    if (std::strcmp(profile_id, "claudio") == 0) {
        return profile_image_claudio;
    }

    if (std::strcmp(profile_id, "herminio") == 0) {
        return profile_image_herminio;
    }

    if (std::strcmp(profile_id, "mariana") == 0) {
        return profile_image_mariana;
    }

    if (std::strcmp(profile_id, "student") == 0) {
        return profile_image_student;
    }

    return profile_image_unknown;
}
}

namespace ProfileImageManager
{

/**
 * @brief Initializes the profile image subsystem.
 *
 * Images are statically linked into the firmware, so initialization currently
 * consists only of preserving a stable public interface for future expansion.
 */
void init()
{
    // Images are embedded in flash memory; no filesystem initialization is required.
}

/**
 * @brief Draws the profile image centered at the specified coordinates.
 *
 * The corresponding image is selected from the embedded resource table.
 * Unknown identifiers automatically fall back to the default placeholder.
 *
 * After rendering the image, a circular border is drawn to visually integrate
 * the avatar with the Identity Node user interface.
 *
 * @param profile_id Null-terminated profile identifier.
 * @param center_x Horizontal center coordinate.
 * @param center_y Vertical center coordinate.
 */
void drawProfile(const char* profile_id, int center_x, int center_y)
{
    const uint16_t* image = resolveProfileImage(profile_id);

    // Convert center coordinates into the upper-left corner expected by
    // pushImage().
    const int x = center_x - (IMAGE_WIDTH / 2);
    const int y = center_y - (IMAGE_HEIGHT / 2);

    M5.Display.pushImage(
        x,
        y,
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        image
    );

    // Draw a thin circular border to visually integrate the avatar with the UI.
    M5.Display.drawCircle(center_x, center_y, IMAGE_RADIUS, WHITE);
}

}
