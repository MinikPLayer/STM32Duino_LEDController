#ifndef _OBJECTS_H
#define _OBJECTS_H
#include <cstdarg>


#include <FastLED.h>
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

#define LED_PIN PA7
#define NUM_LEDS 10
#define BRIGHTNESS 255

#define MAX_CMND_SIZE 100


#include "State.h"
extern CRGB leds[NUM_LEDS];
extern State* actualState;

#include <initializer_list>

/*template<typename T>
class Array
{
	T* ptr;

public:
	int length;
	T& operator[](int index)
	{
		if (index >= length || index < 0)
		{
			Serial.println("ArrayOutOfBoundsException");
			return ptr[length - 1];
		}

		return ptr[index];
	}

	void Init(int size)
	{
		Serial.print("ptr before allocating: ");
		Serial.println((int)ptr);

		length = size;
		ptr = new T[size];

		Serial.print("ptr after allocating: ");
		Serial.println((int)ptr);
	}

	void Init(int size, std::initializer_list<T> args)
	{
		Init(size);

		for (int i = 0; i < args.size() && i < size; i++)
		{
			ptr[i] = *(args.begin() + i);
		}
	}

	void CopyFrom(Array<T> toCopy)
	{

		Init(toCopy.length);

		Serial.println("[Objects.h/Array/CopyFrom] Init complete!");

		for (int i = 0; i < length; i++)
		{
			ptr[i] = toCopy[i];
		}
	}

	void operator=(const Array<T>& toCopy)
	{
		CopyFrom(toCopy);
	}

	Array()
	{
		length = 0;
		ptr = nullptr;
	}

	Array(int size)
	{
		Init(size);
	}

	Array(int size, std::initializer_list<T> args)
	{
		Init(size, args);
	}

	Array(std::initializer_list<T> args)
	{
		Init(args.size(), args);
	}

	~Array()
	{
		if (ptr != nullptr)
		{
			Serial.println("Yeah, destructor, yeaaa");
			Serial.println((int)ptr);
			delete[] ptr;
			ptr = nullptr;
		}
		else
		{
			Serial.println("Wait what? ptr is NULL in destructor");
		}
	}
};*/


#endif