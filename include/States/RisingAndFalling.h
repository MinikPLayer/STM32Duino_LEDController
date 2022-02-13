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
		:State(ptr, num, 3, "risingandfalling")
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
			String color = "";
			int it = 0;
			for(int i = 0;i<value.nameSize;i++)
			{
				if(value.name[i] == ';')
				{
					CRGB clr = ParseStringToColor(color.c_str());
					colors[it] = clr;
					it++;
					if(it >= MAX_COLORS)
					{
						Serial.println("#!TooMuchColors");
						return false;
					}
					color.remove(0);
				}
				else
				{
					color += value.name[i];
				}
				
			}

			if(color.length() != 0)
			{
				CRGB clr = ParseStringToColor(color.c_str());
				colors[it] = clr;
				it++;
			}

			colorSize = it;

			return true;
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
		else if(parameter == params[1]) // colors
		{
			Serial.print("| ");
			for(int i = 0;i<colorSize;i++)
			{
				char arr[] = "            "; 
				Argument colStr(arr, 13);

				ParseColorToString(colors[i], colStr);

				colStr.Print();
				Serial.print(';');
			}
			Serial.println("");

			return true;
		}

		return false;
	}
};

#endif
