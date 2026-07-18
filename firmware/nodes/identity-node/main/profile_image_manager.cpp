#include "profile_image_manager.h"

#include <cstring>
#include "M5Unified.h"

#include "profile_images/claudio.h"
#include "profile_images/herminio.h"
#include "profile_images/mariana.h"
#include "profile_images/student.h"
#include "profile_images/unknown.h"

namespace
{
constexpr int IMAGE_WIDTH = 96;
constexpr int IMAGE_HEIGHT = 96;
constexpr int IMAGE_RADIUS = 48;

const uint16_t* resolveProfileImage(const char* profile_id)
{
    if (profile_id == nullptr) {
        return profile_image_unknown;
    }

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

void init()
{
    // Imagens embarcadas em Flash: nenhuma montagem de filesystem é necessária.
}

void drawProfile(const char* profile_id, int center_x, int center_y)
{
    const uint16_t* image = resolveProfileImage(profile_id);

    const int x = center_x - (IMAGE_WIDTH / 2);
    const int y = center_y - (IMAGE_HEIGHT / 2);

    M5.Display.pushImage(
        x,
        y,
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        image
    );

    // Moldura fina para integrar o avatar ao layout.
    M5.Display.drawCircle(center_x, center_y, IMAGE_RADIUS, WHITE);
}

}
