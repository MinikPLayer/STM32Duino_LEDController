#ifndef _BURNINGDOT_H
#define _BURNINGDOT_H

#include "../Objects.h"
#include "../Util.h"

class State_BurningDot : public State
{
	double mil = 0;



	int pos = 0;


	int color = 0;

public:
	float delay = 0;
	float dimming = 0.25;

	const static int MAX_COLORS = 10;
	CRGB colors[MAX_COLORS];
	int colors_size;


	State_BurningDot(CRGB* l, int n, CRGB clrs[], int clrs_size, float _delay = 100, float _dimming = 0.25)
		:State(l,n, 4, "burning")
	{

		delay = _delay;
		dimming = _dimming;

		//colors = clrs;
		for (int i = 0; i < clrs_size && i < MAX_COLORS; i++)
		{
			colors[i] = clrs[i];
		}
		colors_size = clrs_size;

		if (dimming > 1) dimming = 1;
		if (dimming < 0) dimming = 0;
	}

	void Start()
	{
		if (colors_size != 0)
		{
			fill_solid(leds, leds_count, CRGB::Red);
		}
		
	}

	void NextColor()
	{
		color++;
		if (color >= colors_size)
		{
			color = 0;
		}
	}

	void Update(double deltaTime)
	{
		mil += deltaTime;

		if (mil > delay)
		{

			fadeToBlackBy(leds, leds_count, dimming * 256);

			mil -= delay;

			leds[pos] = colors[color];

			pos++;

			if (pos >= leds_count)
			{
				NextColor();
				pos = 0;
			}

			updateLeds = true;
		}

		
	}

	bool GetParameter(Argument param)
	{
		int colorSize = param.nameSize; if(colorSize > 6) colorSize = 6;
		if(IsCommand("colors", 6, param.name, colorSize))
		{
			/*if(param.nameSize <= 6)
			{
				Serial.println("#!Color argument not complete");
				return false;
			}
			
			int n = StringToInt(param.name + 6, param.nameSize - 6);
			
			if(n >= MAX_COLORS)
			{
				Serial.println("#!Outside of colors range");
				return false;
			}
			
			char arr[] = "             ";
			Argument a(arr, 13);
			ParseColorToString(colors[n], a);
			Serial.print("| ");
			a.Println();
			
			return true;*/
			Serial.print("| ");
			for(int i = 0;i<colors_size;i++)
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
		else if(IsCommand("delay", 5, param.name, param.nameSize))
		{
			char arr[] = "   ";
			Argument a(arr, 3);

			ByteToString(delay, a);
			Serial.print("| ");
			a.Println();
			return true;
		}
		else if(IsCommand("dimming", 7, param.name, param.nameSize))
		{
			char arr[] = "   ";
			Argument a(arr, 3);

			ByteToString(delay * 100.f, a);
			Serial.print("| ");
			a.Println();
			return true;
		}

		return false;
	}

	bool SetParameter(Argument param, Argument value)
	{
		/*if (IsCommand("-addcolor", 9, param.name, param.nameSize))
		{
			CRGB c = FindColor(value.name, value.nameSize);

			if (c.r == 0 && c.b == 0 && c.g == 0)
			{
				return false;
			}

			if (colors_size >= MAX_COLORS)
			{
				Serial.println("#!Too much colors already");
				return false;
			}

			colors[colors_size] = c;
			colors_size++;

			return true;
		}*/
		if(IsCommand("colors", 6, param.name, param.nameSize))
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

			colors_size = it;
			return true;
		}
		else if (IsCommand("delay", 5, param.name, param.nameSize))
		{
			/*int val = StringToInt(value.name, value.nameSize);

			delay = val / 100.0;

			return true;*/
			int val = StringToInt(value.name, value.nameSize);
			if (val == -1)
			{
				return false;
			}
			delay = val;
			return true;
		}
		else if(IsCommand("dimming", 7, param.name, param.nameSize))
		{
			int val = StringToInt(value.name, value.nameSize);
			if (val == -1)
			{
				return false;
			}
			dimming = val / 100.f;
			return true;
		}

		Serial.println("#!Parameter not found");

		return false;
	}
};

#endif // !_BURNINGDOT_H
