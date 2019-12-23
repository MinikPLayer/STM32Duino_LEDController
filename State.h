#ifndef _STATE_H
#define _STATE_H
#include <FastLED.h>
#include "Commands.h"


class State
{
public:
  int leds_count;
  CRGB* leds; // pointer to array

  bool updateLeds = false;

  Argument name;

  State(CRGB* leds_ptr, int num, const char* _name = "undefinied")
  {
    leds_count = num;
    leds = leds_ptr;

	name = Argument(_name);
  }

  virtual void Start()
  {
    
  }

  virtual void Update(double deltaTime)
  {
    Serial.println("Default");
  }

  virtual bool SendParameter(Argument parameter, Argument value)
  {
	  return false;
  }
};
#endif
