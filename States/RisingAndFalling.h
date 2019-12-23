#ifndef _RISINGANDFALLING_H
#define _RISINGANDFALLING_H

#include "../Objects.h"

class State_RisingAndFalling : public State
{
	const static int MAX_COLORS = 10;

	CRGB colors[MAX_COLORS];
	int colorSize = 0;


	int pos = 0;
	double mil = 0;

	int color = 0;

	int delay = 1000;

	bool rising = true;

public:
	

	State_RisingAndFalling(CRGB* ptr, int num, CRGB clrs[], int clrs_size, int _delay = 1000)
		:State(ptr, num)
	{
		//colors = clrs;
		for (int i = 0; i < clrs_size && i < MAX_COLORS;i++)
		{
			colors[i] = clrs[i];
		}

		colorSize = clrs_size;
		if (colorSize > MAX_COLORS)
		{
			colorSize = MAX_COLORS;
		}

		delay = _delay;
	}

	void NextColor()
	{
		color++;

		if (color >= colorSize)
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
