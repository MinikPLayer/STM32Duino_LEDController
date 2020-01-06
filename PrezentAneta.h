#ifndef _PREZENTANETA_H
#define _PREZENTANETA_H

#include "Objects.h"



void ChangeState(State* newState);
void ChangeLED(int number, CRGB color, bool show = true);
void _setup();
void _loop();
void CheckSerial();


extern States defaultState;
extern bool physicalBrightnessControlEnabled;

#endif
