# Identity Profile Images V1

## Ambient Physical AI

### Engineering Discovery

---

# Purpose

This document records the engineering decisions adopted during the implementation of profile photographs in the Identity Node V1.

Its purpose is to explain:

- why the current implementation was adopted;
- how it works;
- its limitations;
- and the planned migration path to the definitive architecture.

This document is intentionally separated from the main architecture documentation because it describes an engineering milestone rather than a permanent architectural definition.

---

# Background

One of the objectives defined during the project evolution was to make the Identity Node capable of presenting a visual representation of the identified person.

Originally, the display presented only textual information:

```text
Name
Role
Context
```

Although functionally correct, this solution reduced the visual impact of the demonstration and did not fully represent the concept of physical identity proposed by Ambient Physical AI.

The project therefore evolved to include profile photographs associated with each identity.

---

# Engineering Challenge

At the current project stage, the Cognitive Runtime (AX630C) is already capable of processing identity information.

However, there is not yet a complete infrastructure responsible for:

- centralized profile storage;
- image synchronization;
- local caching;
- profile updates;
- remote profile management.

Waiting for that infrastructure would delay validation of the Identity Layer.

Therefore, an intermediate solution was adopted.

---

# Adopted Solution

A dedicated component named:

```text
ProfileImageManager
```

was introduced.

This component became the single abstraction responsible for providing profile images to the graphical interface.

The UI never accesses image data directly.

Instead, it simply requests:

```cpp
ProfileImageManager::drawProfile(profile.id, x, y);
```

This abstraction completely isolates the display layer from the image storage mechanism.

---

# Current Image Storage

For Version 1, images are embedded directly into firmware.

Each profile photograph is converted to:

```text
96 x 96 pixels
RGB565
uint16_t array
```

and stored as:

```text
main/profile_images/

claudio.h
herminio.h
mariana.h
student.h
unknown.h
```

Advantages:

- extremely fast rendering;
- zero filesystem dependency;
- deterministic execution;
- no network dependency;
- ideal for laboratory validation.

---

# INICIO

---

## Image Conversion Pipeline: From Photograph to Embedded RGB565 Header

This section documents the reproducible process used to transform ordinary profile photographs into C/C++ header files compiled directly into the Identity Node firmware.

The objective is to convert a conventional JPEG or PNG image into a fixed-size array of RGB565 pixels that the M5Dial can render immediately, without depending on an SD card, filesystem, network connection, or runtime JPEG/PNG decoder.

---

### 1. Input image

The process begins with an ordinary photograph, for example:

```text
claudio.jpg
herminio.jpg
mariana.jpg
student.png
```

The source image may have any practical resolution or orientation. However, the final avatar quality depends more on framing than on the original resolution.

Recommended source-image characteristics:

- the face is clearly visible;
- the subject is reasonably centered;
- the lighting is uniform;
- the background is not visually dominant;
- the image is not blurred;
- the face occupies a significant portion of the frame.

A high-resolution photograph with a distant subject can produce a poorer 96 × 96 result than a lower-resolution photograph with a well-framed face.

---

### 2. Cropping and framing

Before conversion, the image is cropped so that the face and upper body occupy most of the useful area.

Recommended crop:

```text
Aspect ratio: 1:1
Subject: centered
Face: dominant
Background: minimized
```

Conceptually:

```text
Original photograph
        ↓
Face-centered square crop
        ↓
96 × 96 avatar
```

This step is critical because the final image contains only:

```text
96 × 96 = 9,216 pixels
```

Every pixel used for unnecessary background is a pixel no longer available to preserve facial detail.

The crop may be performed manually in any image editor or programmatically in Python. Manual cropping usually gives the best result because the face is not always located in the geometric center of the photograph.

---

### 3. Image normalization

The image is normalized before conversion:

- orientation is corrected;
- the image is converted to RGB;
- palette, grayscale, CMYK, or alpha-channel formats are removed;
- the selected crop is applied;
- the result is resized to the target resolution.

Using Pillow:

```python
from PIL import Image

image = Image.open("claudio.jpg")
image = image.convert("RGB")
```

The explicit conversion to RGB guarantees that every pixel can be read consistently as:

```text
Red, Green, Blue
```

---

### 4. Target size and memory cost

Identity Node V1 uses:

```text
96 × 96 pixels
```

Each RGB565 pixel occupies:

```text
16 bits = 2 bytes
```

Therefore, one uncompressed avatar occupies:

```text
96 × 96 × 2 = 18,432 bytes
```

Approximately:

```text
18 KiB per avatar
```

Five avatars occupy approximately:

```text
5 × 18,432 = 92,160 bytes
```

This calculation explains why the firmware partition size had to be considered when the profile visualization pipeline was introduced.

The 96 × 96 resolution was selected as a compromise between:

- facial recognition;
- display area;
- firmware size;
- rendering speed;
- implementation simplicity;
- deterministic memory use.

---

### 5. Resizing

The cropped image is resized to exactly 96 × 96 pixels.

For photographs, a high-quality resampling filter should be used:

```python
image = image.resize(
    (96, 96),
    Image.Resampling.LANCZOS,
)
```

`LANCZOS` is appropriate for reducing photographs because it normally preserves more visual detail than nearest-neighbor resizing.

Nearest-neighbor scaling is useful for pixel art, but tends to make photographs appear blocky or jagged.

---

### 6. RGB888 to RGB565 conversion

Ordinary decoded photographs generally use RGB888:

```text
Red   = 8 bits
Green = 8 bits
Blue  = 8 bits
Total = 24 bits per pixel
```

The display pipeline used by the Identity Node receives RGB565:

```text
Red   = 5 bits
Green = 6 bits
Blue  = 5 bits
Total = 16 bits per pixel
```

Bit layout:

```text
RRRRR GGGGGG BBBBB
```

The RGB888 channels are reduced as follows:

```python
red_5   = (red >> 3) & 0x1F
green_6 = (green >> 2) & 0x3F
blue_5  = (blue >> 3) & 0x1F
```

The final 16-bit value is assembled with:

```python
rgb565 = (red_5 << 11) | (green_6 << 5) | blue_5
```

Equivalent compact form:

```python
rgb565 = (
    ((red & 0xF8) << 8)
    | ((green & 0xFC) << 3)
    | (blue >> 3)
)
```

Common reference values:

```text
Red   = 0xF800
Green = 0x07E0
Blue  = 0x001F
White = 0xFFFF
Black = 0x0000
```

---

### 7. Pixel ordering

Pixels are written in row-major order:

```text
left to right
top to bottom
```

Conceptually:

```text
(0,0), (1,0), (2,0), ... (95,0)
(0,1), (1,1), (2,1), ... (95,1)
...
(0,95) ...                 (95,95)
```

Typical iteration:

```python
for y in range(height):
    for x in range(width):
        red, green, blue = image.getpixel((x, y))
```

The resulting array contains exactly 9,216 RGB565 values.

---

### 8. Byte-order validation

RGB565 defines the logical color layout, but the displayed result may also depend on byte order.

For example:

```text
0xF800
```

contains the bytes:

```text
0xF8 0x00
```

Depending on the graphics library and rendering function, a buffer may be interpreted as:

- native 16-bit values;
- little-endian data;
- big-endian data;
- byte-swapped RGB565.

An incorrect interpretation may produce:

- red appearing blue;
- unnatural skin tones;
- purple or green artifacts;
- globally distorted colors.

The current Identity Node implementation was validated using a compatible RGB565 representation and display call. If the rendering API changes in the future, byte order must be revalidated before converting all profile images again.

A useful test pattern contains:

```text
red
green
blue
white
black
```

---

### 9. Header-file generation

After conversion, each pixel is written as a hexadecimal C/C++ value:

```cpp
0xFFFF, 0xFFDE, 0xF79D, 0xEF5B,
0xE71A, 0xDEDA, 0xD698, 0xCE57,
```

A simplified generated header looks like this:

```cpp
#pragma once

#include <stdint.h>

static const uint16_t claudio_profile_image[96 * 96] = {
    0xFFFF, 0xFFDE, 0xF79D, 0xEF5B,
    // Remaining pixels...
};
```

The Identity Node stores the generated files in:

```text
main/
└── profile_images/
    ├── claudio.h
    ├── herminio.h
    ├── mariana.h
    ├── student.h
    └── unknown.h
```

Each header contains the complete RGB565 pixel array for one profile.

---

### 10. Representative Python conversion script

The following script reproduces the essential pipeline:

```python
from pathlib import Path
from PIL import Image

WIDTH = 96
HEIGHT = 96


def rgb888_to_rgb565(
    red: int,
    green: int,
    blue: int,
) -> int:
    red_5 = (red >> 3) & 0x1F
    green_6 = (green >> 2) & 0x3F
    blue_5 = (blue >> 3) & 0x1F

    return (
        (red_5 << 11)
        | (green_6 << 5)
        | blue_5
    )


def convert_image_to_header(
    input_path: str,
    output_path: str,
    array_name: str,
) -> None:
    source = Path(input_path)
    destination = Path(output_path)

    if not source.exists():
        raise FileNotFoundError(
            f"Input image not found: {source}"
        )

    image = Image.open(source)
    image = image.convert("RGB")
    image = image.resize(
        (WIDTH, HEIGHT),
        Image.Resampling.LANCZOS,
    )

    pixels: list[int] = []

    for y in range(HEIGHT):
        for x in range(WIDTH):
            red, green, blue = image.getpixel((x, y))

            pixels.append(
                rgb888_to_rgb565(
                    red,
                    green,
                    blue,
                )
            )

    destination.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    with destination.open(
        "w",
        encoding="utf-8",
        newline="\n",
    ) as header:
        header.write("#pragma once\n\n")
        header.write("#include <stdint.h>\n\n")

        header.write(
            f"static const uint16_t "
            f"{array_name}[{WIDTH * HEIGHT}] = {{\n"
        )

        values_per_line = 12

        for index, pixel in enumerate(pixels):
            if index % values_per_line == 0:
                header.write("    ")

            header.write(f"0x{pixel:04X}")

            if index < len(pixels) - 1:
                header.write(", ")

            if (
                index % values_per_line
                == values_per_line - 1
            ):
                header.write("\n")

        if len(pixels) % values_per_line != 0:
            header.write("\n")

        header.write("};\n")


if __name__ == "__main__":
    convert_image_to_header(
        input_path="claudio.jpg",
        output_path="claudio.h",
        array_name="claudio_profile_image",
    )
```

Install Pillow with:

```bash
python -m pip install Pillow
```

Run the converter with:

```bash
python convert_profile_image.py
```

---

### 11. Optional cropping in Python

A manually selected crop rectangle can be applied before resizing:

```python
image = Image.open("claudio.jpg")
image = image.convert("RGB")

left = 320
top = 80
right = 1040
bottom = 800

image = image.crop(
    (
        left,
        top,
        right,
        bottom,
    )
)

image = image.resize(
    (96, 96),
    Image.Resampling.LANCZOS,
)
```

The crop coordinates are expressed in source-image pixels:

```text
left, top, right, bottom
```

A centered square crop can also be calculated automatically:

```python
width, height = image.size
side = min(width, height)

left = (width - side) // 2
top = (height - side) // 2
right = left + side
bottom = top + side

image = image.crop(
    (
        left,
        top,
        right,
        bottom,
    )
)
```

Automatic cropping is convenient, but manual adjustment usually gives a better avatar.

---

### 12. Firmware integration

The generated headers are included by the profile-image backend:

```cpp
#include "profile_images/claudio.h"
#include "profile_images/herminio.h"
#include "profile_images/mariana.h"
#include "profile_images/student.h"
#include "profile_images/unknown.h"
```

The `ProfileImageManager` receives a profile identifier and selects the appropriate RGB565 array.

Conceptually:

```text
Profile ID
    ↓
ProfileImageManager
    ↓
RGB565 array
    ↓
Display rendering
```

The UI only calls:

```cpp
ProfileImageManager::drawProfile(
    profile.id,
    x,
    y
);
```

The UI does not need to know:

- the source filename;
- the generated array name;
- the pixel-conversion method;
- whether the image is embedded or loaded dynamically.

This separation is one of the most important architectural decisions in the implementation.

---

### 13. Why embedded headers were used in V1

Embedding the images as C/C++ headers provides:

- no filesystem dependency;
- no SD-card dependency;
- no network dependency;
- no runtime JPEG/PNG decoder;
- deterministic startup;
- deterministic memory use;
- immediate availability after boot;
- simple build integration;
- robust competition-demo behavior.

The main limitation is that changing an avatar requires:

```text
new photograph
    ↓
new conversion
    ↓
new header
    ↓
firmware rebuild
    ↓
firmware flash
```

This limitation is accepted in V1 because the embedded-image strategy is intentionally temporary.

---

### 14. Reproducibility checklist

```text
[ ] Install Python
[ ] Install Pillow
[ ] Select the source photograph
[ ] Crop around the face
[ ] Convert the image to RGB
[ ] Resize to 96 × 96
[ ] Convert RGB888 to RGB565
[ ] Preserve row-major pixel order
[ ] Generate the .h file
[ ] Place it in main/profile_images/
[ ] Register it in ProfileImageManager
[ ] Build the firmware
[ ] Flash the M5Dial
[ ] Validate colors on the physical display
[ ] Validate profile-to-avatar mapping
```

Physical display validation is mandatory. A computer preview does not fully guarantee that color order, byte order, crop, and skin tones will appear correctly on the actual device.

---

### 15. Current and future pipeline

Current V1 pipeline:

```text
User photograph
        ↓
Manual crop
        ↓
Python + Pillow
        ↓
Resize to 96 × 96
        ↓
RGB888 to RGB565
        ↓
C/C++ header generation
        ↓
Firmware compilation
        ↓
Embedded Flash storage
        ↓
ProfileImageManager
        ↓
M5Dial display
```

Possible future pipeline:

```text
User photograph
        ↓
Profile service
        ↓
AX630C / Cognitive Runtime
        ↓
Network synchronization
        ↓
Identity Node cache
        ↓
Runtime image buffer
        ↓
ProfileImageManager
        ↓
M5Dial display
```

The architectural contract remains unchanged:

```text
The UI continues calling ProfileImageManager.
```

Only the image source changes.

Today, the source is an embedded `uint16_t` array. In a future version, it may come from:

- local filesystem;
- SPI flash partition;
- SD card;
- PSRAM cache;
- network download;
- decoded JPEG or PNG;
- AX630C-synchronized profile storage.

---

### 16. Engineering conclusion

The conversion from photograph to `.h` file is a deterministic image-processing pipeline:

```text
crop
resize
RGB565 conversion
array generation
firmware embedding
```

Python and Pillow make the process:

- understandable;
- repeatable;
- auditable;
- modifiable;
- independent of proprietary tools.

For Identity Node V1, this approach provided a reliable path to profile visualization while preserving a clean migration path toward dynamically synchronized profile images.

---

# FIM

---

# Technical Challenges Found

During implementation several engineering challenges were identified.

## Application Size

Embedding five RGB565 photographs increased the firmware size beyond the default 1 MB application partition.

The project was migrated from:

```text
Single Factory App
```

to

```text
Single Factory App (Large)
```

allowing the firmware to grow without requiring architectural modifications.

---

## RGB565 Rendering

Initial rendering presented corrupted colors due to RGB565 interpretation.

After validation, the rendering pipeline was corrected and stable profile visualization was achieved.

---

## Avatar Quality

The rendering pipeline proved correct.

However, portrait quality depends strongly on the original photograph.

Images containing:

- large backgrounds;
- distant faces;
- excessive visual detail;

produce poor results after reduction to 96 × 96 pixels.

This is not a software limitation but a consequence of image composition.

Future versions should use portraits specifically prepared for avatar rendering.

---

# Current Status

Validated features:

- ProfileImageManager abstraction
- Embedded profile images
- Five independent user profiles
- Five NFC cards mapped
- Automatic avatar selection
- Stable rendering
- Unknown profile fallback

The complete identity visualization flow is now operational.

---

# Why This Is Considered Temporary

Although fully functional, embedded profile images are **not** the intended final architecture.

They were intentionally adopted to validate:

- identity flow;
- user interface;
- profile abstraction;
- rendering pipeline;
- engineering integration.

before introducing the complete profile infrastructure.

---

# Planned Evolution

The future architecture will preserve the same application interface.

Only the image source will change.

Current implementation:

```text
NFC Card
        │
        ▼
UID Mapping
        │
        ▼
Profile Manager
        │
        ▼
ProfileImageManager
        │
        ▼
Embedded RGB565 Images
```

Future implementation:

```text
NFC Card
        │
        ▼
NDEF / UID
        │
        ▼
Identity Service
        │
        ▼
Cognitive Runtime (AX630C)
        │
        ▼
Profile Synchronization
        │
        ▼
Local Cache
        │
        ▼
ProfileImageManager
```

Notice that the graphical interface remains unchanged.

Only the image provider changes.

This validates the architectural decision of introducing the ProfileImageManager abstraction from the beginning.

---

# Conclusion

The implementation successfully achieved the objectives established for Identity Node V1.

The project now supports:

- visual identity;
- profile abstraction;
- avatar rendering;
- profile selection;
- future migration to synchronized profile storage.

The current implementation should therefore be considered an engineering milestone that validates the architectural direction while keeping the project simple, deterministic and fully reproducible during the current development stage.