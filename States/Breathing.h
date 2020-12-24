#ifndef _STATE_BREATHING_H
#define _STATE_BREATHING_H

#include "Rainbow.h"
#include "../Objects.h"

class State_Breathing : public State
{

public:
	State_Rainbow rainbow;

	float speed = 0.1f;

	State_Breathing(CRGB* l, int n, float _speed = 0.1f)
		:State(l,n, 5, "breathing"), rainbow(l,n,_speed, 1)
	{
		speed = _speed;

		params[0] = "speed";
	}

	void Start()
	{
		rainbow.Start();

		updateLeds = rainbow.updateLeds;
	}

	void Update(double deltaTime)
	{
		rainbow.Update(deltaTime);

		updateLeds = rainbow.updateLeds;
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

			rainbow.speed = val / 100.0f;
			speed = rainbow.speed;
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

			ByteToString(rainbow.speed * 100, a);
			Serial.print("| ");
			a.Println();
			return true;
		}

		return false;
	}
};

#endif // !_STATE_BREATHING_H

