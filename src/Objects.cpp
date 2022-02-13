#include "Objects.h"

int brightness = 255;
CRGB leds[NUM_LEDS];
State* actualState;
bool physicalBrightnessControlEnabled = false;
int currentPreset = 0;