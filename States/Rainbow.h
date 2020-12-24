#ifndef _RAINBOW_H
#define _RAINBOW_H

#include "../Objects.h"

class State_Rainbow : public State
{
	uint8_t hue = 0;
	

	double fraction = 0;

public:
	int width = -1;
	float speed = 0;

	State_Rainbow(CRGB* l, int num, float _speed = 0.1, int rainbow_width = -1)
		:State(l, num, 2, "rainbow")
	{
		speed = _speed;
		width = rainbow_width;

		params[0] = "speed";
		params[1] = "width";
	}

	void Start()
	{
		if (width == -1)
		{
			width = leds_count;
		}
		fill_rainbow(leds, leds_count, hue, 255 / width);

		updateLeds = true;
	}

	void Update(double deltaTime)
	{
		fraction += speed * deltaTime;
		while (fraction > 0)
		{
			hue++;
			fraction -= 1;
		}
		fill_rainbow(leds, leds_count, hue, 255 / width);

		updateLeds = true;
	}

	bool SetParameter(Argument param, Argument value)
	{

		if (param == params[0]) // Speed
		{
			int val = StringToInt(value.name, value.nameSize);
			if (val == -1)
			{
				return false;
			}
			speed = val / 100.0f;
			return true;
		}
		else if (param == params[1]) // Width
		{
			width = StringToInt(value.name, value.nameSize);
			return true;
		}

		return false;
	}

	bool GetParameter(Argument parameter)
	{
		if (parameter == params[0]) // Speed
		{
			char arr[] = "   ";
			Argument a(arr, 3);

			ByteToString(speed * 100, a);
			Serial.print("| ");
			a.Println();
			return true;
		}
		else if (parameter == params[1]) // Width
		{
			char arr[] = "   ";
			Argument a(arr, 3);

			ByteToString(width, a);
			Serial.print("| ");
			a.Println();
			return true;
		}

		return false;
	}
};

#endif // !_RAINBOW_H
