#ifndef _STATICCOLOR_H
#define _STATICCOLOR_H
#include "../Objects.h"

class State_StaticColor : public State
{
  float mil = 0;
  int pos = 0;

  CRGB color;
  
public:
	State_StaticColor(CRGB* l, int n, CRGB clr)
		:State(l, n, "Static color")
  {
    color = clr;
  }

  void Start()
  {
    for(int i = 0;i<leds_count;i++)
    {
      leds[i] = color;
    }

    updateLeds = true;
  }
  
  void Update(double deltaTime)
  {
    /*//Serial.println(deltaTime);

    mil += deltaTime;

    if(mil > 1000)
    {
      leds[pos] = CRGB(255, 0, 0);
      pos++;
      if(pos >= leds_count)
      {
        pos = 0;
        for(int i = 0;i<leds_count;i++)
        {
          leds[i] = CRGB(0,0,0);
        }
      }
      mil = 0;
    }*/
  }
};

#endif
