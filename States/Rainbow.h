#ifndef _RAINBOW_H
#define _RAINBOW_H

#include "../Objects.h"

class State_Rainbow : public State
{
	uint8_t hue = 0;
	float speed = 0;

	double fraction = 0;

public:
	State_Rainbow(CRGB* l, int num, float _speed = 0.1)
		:State(l, num, "Rainbow")
	{
		speed = _speed;
	}

	void Start()
	{
		fill_rainbow(leds, leds_count, hue, 255 / leds_count);

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
		fill_rainbow(leds, leds_count, hue, 255 / leds_count);

		updateLeds = true;
	}
};

#endif // !_RAINBOW_H
