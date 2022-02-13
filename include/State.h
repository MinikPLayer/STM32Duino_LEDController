#ifndef _STATE_H
#define _STATE_H
#include <FastLED.h>
#include "Commands.h"
#include "Util.h"
#include "Objects.h"



class State
{
public:
	Arguments<10> params;

  int leds_count;
  CRGB* leds; // pointer to array

  bool updateLeds = false;

  Argument name;
  int id;

  State(CRGB* leds_ptr, int num, int _id, const char* _name = "undefinied")
  {
    leds_count = num;
    leds = leds_ptr;

	name = Argument(_name);
	id = _id;
  }

  virtual void Start()
  {
    
  }

  virtual void Update(double deltaTime)
  {
    Serial.println("Default");
  }

  virtual bool SetParameter(Argument parameter, Argument value)
  {
	  return false;
  }

  virtual bool GetParameter(Argument parameter)
  {
		  //return Argument("#!NotImplemented");
		Serial.println("#!NotImplemented");
		return false;
	 


	  return false;
  }
};
#endif
