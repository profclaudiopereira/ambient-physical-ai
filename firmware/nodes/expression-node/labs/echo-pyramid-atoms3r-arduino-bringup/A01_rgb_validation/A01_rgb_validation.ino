#include "M5Unified.h"
#include <M5EchoPyramid.h>

M5EchoPyramid ep;

int brightness = 0;
int step = 5;
bool rising = true;

int leds[4] = {0, 3, 7, 10};

void setup()
{
    M5.begin();
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("=== Ambient Physical AI ===");
    Serial.println("Test A01 - Echo Pyramid RGB LED");
    Serial.println("Initializing Echo Pyramid...");

    Wire1.end();

    ep.begin(&Wire1, 38, 39, 6, 8, 5, 7, 44100);

    ep.ctrl().setBrightness(1, 100);
    ep.ctrl().setBrightness(2, 100);

    Serial.println("Echo Pyramid RGB LED demo started.");
}

void loop()
{
    if (rising) {
        brightness += step;
        if (brightness >= 255) {
            brightness = 255;
            rising = false;
        }
    } else {
        brightness -= step;
        if (brightness <= 10) {
            brightness = 10;
            rising = true;
        }
    }

    for (int i = 0; i < 4; i++) {
        ep.ctrl().setRGB(1, leds[i], brightness, 0, 255 - brightness);
        ep.ctrl().setRGB(2, leds[i], brightness, 0, 255 - brightness);
    }

    delay(40);
}