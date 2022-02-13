#ifndef _UTIL_H
#define _UTIL_H

//#include <FastLED.h>
#include "Objects.h"

void PrintParam(int parameter);

int ReadEEPROM(int slot, int defaultValue = 0, int maxValue = 254);
void LoadConfig();
bool SaveConfig();

CRGB ReadColorFromEEPROM(int& slot);
// Saves color value to EEPROM at specified slot and returns new slot value after writing values
int SaveColorToEEPROM(int slot, CRGB color);

int GetColorNumber(CRGB color, int errorValue = 0);
CRGB GetColor(int number, CRGB def = CRGB::Black);
CRGB FindColor(char* name, int length);
CRGB ParseStringToColor(Argument arg);
bool IsNumber(char c);
long long int pow(int a, int n);
int StringToInt(char* str, int size);

// You HAVE TO create an arg parameter as this: char arr[] = "const"; Argument arg(arr, size); It won't work and CRASH the other way
void ByteToString(unsigned char number, Argument& arg);

// You HAVE TO create an arg parameter as this: char arr[] = "            "; Argument arg(arr, 13); It won't work and CRASH the other way
bool ParseColorToString(CRGB color, Argument& ret);

#endif // !_UTIL_H
