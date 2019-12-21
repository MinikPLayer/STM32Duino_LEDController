#ifndef _UTIL_H
#define _UTIL_H

#include <FastLED.h>

CRGB FindColor(char* name, int length);
bool IsNumber(char c);
long long int pow(int a, int n);
int StringToInt(char* str, int size);

#endif // !_UTIL_H
