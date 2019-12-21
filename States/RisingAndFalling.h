#ifndef _RISINGANDFALLING_H
#define _RISINGANDFALLING_H

#include "../Objects.h"

class State_RisingAndFalling : public State
{
	Array<CRGB> colors;
	int pos = 0;
	double mil = 0;

	int color = 0;

	int delay = 1000;

	bool rising = true;

public:
	

	State_RisingAndFalling(CRGB* ptr, int num, Array<CRGB> clrs, int _delay = 1000)
		:State(ptr, num)
	{
		colors = clrs;

		delay = _delay;
	}

	void NextColor()
	{
		color++;

		if (color >= colors.length)
		{
			color = 0;
		}
	}

	void Update(double deltaTime)
	{

		mil += deltaTime;

		if(mil > delay)
		{
			leds[pos] = colors[color];
			if (rising)
			{
				pos++;
				if (pos >= leds_count - 1)
				{
					rising = false;
					pos = leds_count - 1;

					NextColor();
				}

				
			}
			else
			{
				pos--;
				if (pos <= 0)
				{
					rising = true;
					pos = 0;

					NextColor();
				}
			}

			mil -= delay;
			updateLeds = true;
		}

		
	}
};

#endif
