#ifndef _OBJECTS_H
#define _OBJECTS_H

#include <FastLED.h>
#include "Device.h"
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

#pragma region LED_DEFINES
	#ifdef STM32
		#define LED_PIN PA7
		#define BRIGHTNESS_PIN PA0
		#define BRIGHTNESS_POTENTIOMETER_INPUT_PIN PC14
	#endif
	#ifdef ESP32
		#define LED_PIN 13 // D13
		#define BRIGHTNESS_PIN 12 // D12
		#define BRIGHTNESS_POTENTIOMETER_INPUT_PIN 14 // D14
	#endif
	#ifdef ESP8266
		#define LED_PIN 15 // D13
		#define BRIGHTNESS_PIN 12 // D12
		#define BRIGHTNESS_POTENTIOMETER_INPUT_PIN 14 // D14
	#endif

	#ifndef LED_PIN
		#error "Unsupported platform"
	#endif
#pragma endregion

#define POTENTIOMETER_ENABLED false

#define DEVICE_NAME "I Love You <3"
#define NUM_LEDS 22

#define BAUDRATE 57600

#define MAX_CMND_SIZE 150

#define VERSION "0.40b1"

#define MAX_PRESETS 5
#define PRESET_SEPARATION 100


#include "State.h"
extern int brightness;
extern CRGB leds[NUM_LEDS];
extern State* actualState;
extern int currentPreset;

enum States
{
	undefinied,
	StaticColor,
	Rainbow,
	RisingAndFalling,
	BurningDot,
	Breathing,
	Gradient,
};

#endif