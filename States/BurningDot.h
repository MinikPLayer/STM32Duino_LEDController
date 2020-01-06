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
	float speed = 0;
	float dimming = 0.25;

	const static int MAX_COLORS = 10;
	CRGB colors[MAX_COLORS];
	int colors_size;


	State_BurningDot(CRGB* l, int n, CRGB clrs[], int clrs_size, float _speed = 100, float _dimming = 0.25)
		:State(l,n, 4, "Burning dot")
	{

		speed = _speed;
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

		if (mil > speed)
		{

			fadeToBlackBy(leds, leds_count, dimming * 256);

			mil -= speed;

			leds[pos] = colors[color];

			pos++;

			if (pos >= leds_count)
			{
				NextColor();
				pos = 0;
			}
		}

		updateLeds = true;
	}

	bool SetParameter(Argument param, Argument value)
	{
		if (IsCommand("-addcolor", 9, param.name, param.nameSize))
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
		}
		else if (IsCommand("-speed", 6, param.name, param.nameSize))
		{
			int val = StringToInt(value.name, value.nameSize);

			speed = val / 100.0;

			return true;
		}

		Serial.println("#!Parameter not found");

		return false;
	}
};

#endif // !_BURNINGDOT_H
