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

  State(CRGB* leds_ptr, int num)
  {
    leds_count = num;
    leds = leds_ptr;
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
