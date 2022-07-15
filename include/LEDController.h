#ifndef _LEDCONTROLLER_H
#define _LEDCONTROLLER_H

#include "Objects.h"

void Serial_Println(int data);
void Serial_Println(const char* data);
void Serial_Print(int data);
void Serial_Print(const char* data);

void ChangeState(State* newState);
void ChangeLED(int number, CRGB color, bool show = true);
void _setup();
void _loop();
void CheckSerial();


extern States defaultState;
extern bool physicalBrightnessControlEnabled;

#endif
