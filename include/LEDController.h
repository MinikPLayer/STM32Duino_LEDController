#ifndef _LEDCONTROLLER_H
#define _LEDCONTROLLER_H

#include "Objects.h"
#include "SerialWrappers/WifiSerial.hpp"

WifiSerial* wSerial = nullptr;

void ChangeState(State* newState);
void ChangeLED(int number, CRGB color, bool show = true);
void _setup();
void _loop();
void CheckSerial();


extern States defaultState;
extern bool physicalBrightnessControlEnabled;

#endif
