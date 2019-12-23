#include "Util.h"
#include "Commands.h"

bool Equal(char* str1, int str1l, char* str2, int str2l)
{
	if (str1l != str2l)
	{
		return false;
	}

	for (int i = 0; i < str1l; i++)
	{
		if (str1[i] != str2[i])
		{
			return false;
		}
	}

	return true;
}

// Format: "(R,G,B)"
CRGB ParseStringToColor(char* str, int length)
{
	if (length < 7) // ( + R + , + G + , + B + )
	{
		return CRGB::Black;
	}

	if (str[length - 1] == '\0') length--;

	if (length < 7) // ( + R + , + G + , + B + )
	{
		return CRGB::Black;
	}

	str += 1; // (
	length -= 2; // ( and )


	CRGB color = CRGB(0, 0, 0);

	int cl = 0;
	char* argStart = str;
	int argL = 0;
	for (int i = 0; i < length; i++)
	{
		if (str[i] == ',')
		{
			int val = StringToInt(argStart, argL);

			if (val > 255)
			{
				Serial.println("#!Too big color passed");
				return CRGB::Black;
				//return color;
			}
			if (val < 0)
			{
				Serial.println("#!Too small number");
				return CRGB::Black;
				return color;
			}

			if (cl == 0)
			{
				color.r = val;
			}
			else if (cl == 1)
			{
				color.g = val;
			}
			else if (cl == 2)
			{
				color.b = val;
				break;
			}

			cl++;
			argStart = str + i + 1;
			argL = 0;
		}
		else
		{
			argL++;
		}
	}

	if (argL != 0)
	{
		if (cl != 2)
		{
			Serial.println("#!Not enough color struct parameters");
			return CRGB::Black;
		}

		int val = StringToInt(argStart, argL);
		if (val < 0 || val > 255)
		{
			Serial.println("#!Too big color passed");
			return CRGB::Black;
			//return color;
		}
		color.b = val;
	}

	return color;
}

struct Color
{
	CRGB color;
	Argument name;

	Color(const char* _name, CRGB _color)
	{
		name = Argument(_name);
		color = _color;
	}
};

const int colorsSize = 6;
Color colors[colorsSize] = { Color("red", CRGB::Red), Color("blue",CRGB::Blue), Color("green", CRGB::Green), Color("yellow", CRGB::Yellow), Color("pink", CRGB::Pink), Color("white", CRGB::White) };

CRGB FindColor(char* name, int length)
{
	if (length == 0)
	{
		return CRGB::Black;
	}

	if (name[0] == '(' && name[length - 1] == ')')
	{
		return ParseStringToColor(name, length);
	}

	/*if (Equal(name, length, "blue", 4))
	{
		return CRGB::Blue;
	}
	else if (Equal(name, length, "green", 5))
	{
		return CRGB::Green;
	}
	else if (Equal(name, length, "red", 3))
	{
		return CRGB::Red;
	}
	else if(Equal(name, leng)*/

	for (int i = 0; i < colorsSize; i++)
	{
		if (colors[i].name.Equals(name, length))
		{
			return colors[i].color;
		}
	}

	Serial.println("#!Color not found");

	return CRGB::Black;
}

bool IsNumber(char c)
{
	if (c >= '0' && c <= '9')
	{
		return true;
	}

	return false;
}

long long int pow(int a, int n)
{
	if (n == 0) return 1;
	if (n == 1) return a;

	int og = a;
	for (int i = 1; i < n; i++)
	{
		a *= og;
	}

	return a;
}

int StringToInt(char* str, int size)
{
	int val = 0;
	if (size <= 0)
	{
		Serial.println("#!Cannot convert empty number");
		return -1;
	}

	if (str[size - 1] == '\0') size--;

	if (size <= 0)
	{
		Serial.println("#!Cannot convert empty number");
		return -1;
	}

	if (!IsNumber(str[0]) && str[0] != '-')
	{
		Serial.println("#!Error converting number");
		return -1;
	}

	int start = 0;
	bool negative = false;
	if (str[0] == '-')
	{
		start = 1;
		negative = true;
	}

	for (int i = start; i < size; i++)
	{
		if (!IsNumber(str[i]))
		{
			Serial.print("Character at ");
			Serial.print(i);
			Serial.print("(");
			Serial.print(str[i]);
			Serial.println(") is not a number");
			return -1;
		}

		val += pow(10, (size - i - 1)) * (str[i] - '0');
	}

	return val;
}