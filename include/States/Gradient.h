#ifndef _GRADIENT_H
#define _GRADIENT_H
#include "../Objects.h"


class State_Gradient : public State
{
  float mil = 0;
  int pos = 0;

  
  
public:
	CRGB colors[2];
	bool smooth;

	State_Gradient(CRGB* l, int n, CRGB clr[2], bool smooth)
		:State(l, n, 6, "gradient")
  {
    colors[0] = clr[0];
    colors[1] = clr[1];

	params[0] = Argument("color1");
    params[1] = Argument("color2");
	params[2] = Argument("smooth");
	
	this->smooth = smooth;
  }

  void Start()
  {
    for(int i = 0;i<leds_count;i++)
    {
		CRGB clr = colors[0];
		if(smooth)
		{
			clr.r = colors[0].r * ((float)(leds_count - i) / leds_count) + colors[1].r * ((float)i / leds_count);
			clr.g = colors[0].g * ((float)(leds_count - i) / leds_count) + colors[1].g * ((float)i / leds_count);
			clr.b = colors[0].b * ((float)(leds_count - i) / leds_count) + colors[1].b * ((float)i / leds_count);
		}
		else
		{
			if(i > leds_count / 2)
				clr = colors[1];
			else
				clr = colors[0];
		}
      //leds[i] = colors[0] * ((float)i / leds_count) + colors[1] * ((float)(leds_count-i) / leds_count); //color;
		leds[i] = clr;
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

  bool SetParameter(Argument param, Argument value)
  {

	if (param == params[0]) // Color 1
	{
	  CRGB clr = ParseStringToColor(value);
	  colors[0] = clr;

	  Start();
	  return true;
	}
	else if (param == params[1]) // Color 1
	{
	  CRGB clr = ParseStringToColor(value);
	  colors[1] = clr;

	  Start();
	  return true;
	}
	else if(param == params[2]) // smooth
	{
		if(value.nameSize == 0) 
		{
			Serial.println("#!Empty argument");
			return false;
		}
		if(value.name[0] == '0')
		{
			smooth = false;
		}
		else
		{
			smooth = true;
		}
		
		Start();
		return true;
	}

	return false;
  }

	bool GetParameter(Argument parameter)
	{

		if (parameter == params[0]) // Color 1
		{
		  char arr[] = "             ";
		  Argument a(arr, 13);
		  ParseColorToString(colors[0], a);
		  Serial.print("| ");
		  a.Println();
		  return true;
		}
		else if (parameter == params[1]) // Color 2
		{
		  char arr[] = "             ";
		  Argument a(arr, 13);
		  ParseColorToString(colors[1], a);
		  Serial.print("| ");
		  a.Println();
		  return true;
		}
		else if(parameter == params[2]) // Smooth
		{
			if(smooth)
				Serial.println("| 1");
			else
				Serial.println("| 0");
			
			return true;
		}

		return false;
	}
};

#endif
