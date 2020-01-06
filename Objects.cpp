#include "Objects.h"

int brightness = 255;
CRGB leds[NUM_LEDS];
State* actualState;
bool physicalBrightnessControlEnabled = true;
int currentPreset = 0;