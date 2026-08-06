/**
 * @file profile_image_manager.h
 * @brief Public interface for rendering Identity Node profile images.
 *
 * This module provides the application-level interface used to initialize
 * profile image resources and draw the image associated with an authenticated
 * user profile.
 *
 * Profile resolution and display positioning are handled by the implementation.
 * Higher-level identity logic remains responsible for supplying the profile
 * identifier and selecting when the image must be rendered.
 */

#pragma once

/**
 * @brief Manages profile image initialization and rendering.
 */

namespace ProfileImageManager
{
        /**
     * @brief Initializes the profile image rendering subsystem.
     *
     * This function must be called during application initialization before
     * profile images are drawn.
     */
    void init();

        /**
     * @brief Draws the image associated with a profile identifier.
     *
     * The image is positioned around the supplied center coordinates. The
     * implementation is responsible for resolving the profile identifier to
     * the corresponding image resource.
     *
     * @param profile_id Null-terminated profile identifier used to select the
     *                   image to render.
     * @param center_x Horizontal center coordinate of the rendered image.
     * @param center_y Vertical center coordinate of the rendered image.
     */
    void drawProfile(
        const char* profile_id,
        int center_x,
        int center_y
    );
}
