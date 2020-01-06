#ifndef _RISINGANDFALLING_H
#define _RISINGANDFALLING_H

#include "../Objects.h"

class State_RisingAndFalling : public State
{



	int pos = 0;
	double mil = 0;

	int color = 0;

	

	bool rising = true;

public:
	const static int MAX_COLORS = 10;

	CRGB colors[MAX_COLORS];
	int colorSize = 0;
	int delay = 1000;

	State_RisingAndFalling(CRGB* ptr, int num, CRGB clrs[], int clrs_size, int _delay = 50)
		:State(ptr, num, 3, "Rising And Falling")
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

		params[0] = "delay";
		params[1] = "colors";
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

	bool SetParameter(Argument param, Argument value)
	{

		if (param == params[0]) // Delay
		{
			int val = StringToInt(value.name, value.nameSize);
			if (val == -1)
			{
				return false;
			}
			delay = val * 10;
			return true;
		}
		if (param == params[1]) // colors
		{
			
		}
		return false;
	}

	bool GetParameter(Argument parameter)
	{
		if (parameter == params[0]) // Delay
		{
			char arr[] = "   ";
			Argument a(arr, 3);

			ByteToString(delay / 10, a);
			Serial.print("| ");
			a.Println();
			return true;
		}

		return false;
	}
};

#endif
