#include "Util.h"
#include "Commands.h"

#include "Configurator.h"
#include "Objects.h"
#include "LEDController.h"
#include "States.h"

void PrintParam(int parameter)
{
	Serial.print("| ");
	Serial.println(parameter);
}

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

// You HAVE TO create an arg parameter as this: char arr[] = "            "; Argument arg(arr, 13); It won't work and CRASH the other way
bool ParseColorToString(CRGB color, Argument& ret)
{
	// Max length: (255,255,255)
	if (ret.nameSize != 13)
	{
		Serial.println("#!TooSmallArgumentValue");
		return false;
	}
	ret.name[0] = '(';
	int pos = 1;

	char arr[] = "   ";
	Argument arg(arr, 3);

	ByteToString(color.r, arg);
	for (int i = 0; i < arg.nameSize; i++)
	{
		ret.name[pos++] = arg.name[i];
	}
	ret.name[pos++] = ',';
	arg.nameSize = 3;


	ByteToString(color.g, arg);
	for (int i = 0; i < arg.nameSize; i++)
	{
		ret.name[pos++] = arg.name[i];
	}
	ret.name[pos++] = ',';
	arg.nameSize = 3;

	ByteToString(color.b, arg);
	for (int i = 0; i < arg.nameSize; i++)
	{
		ret.name[pos++] = arg.name[i];
	}
	ret.name[pos++] = ')';

	ret.nameSize = pos;

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

int ReadEEPROM(int slot, int defaultValue, int maxValue)
{
	int val = Configurator::Read(slot);
	if (val > 255 || val > maxValue)
	{
		val = defaultValue;
		Configurator::Write(slot, defaultValue);
	}

	return val;
}

void LoadConfig()
{

	brightness = ReadEEPROM(EEPROM_SLOT_BRIGHTNESS + (currentPreset * PRESET_SEPARATION), 255);
	if (brightness == 0) brightness = 255;
	FastLED.setBrightness(brightness);

	defaultState = (States)ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE + (currentPreset * PRESET_SEPARATION), 1);

	physicalBrightnessControlEnabled = ReadEEPROM(EEPROM_SLOT_PHYSICAL_BRIGHTNESS_ENABLED + (currentPreset * PRESET_SEPARATION), 1);

	switch (defaultState)
	{
	case undefinied:
		Serial.println("#!No default mode selected");
		ChangeState(new State_StaticColor(leds, NUM_LEDS, CRGB::Black));
	case StaticColor:
	{
		//int color = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1, 0);
		//CRGB clr = GetColor(color, CRGB::Green);
		int ptr = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION);
		CRGB clr = ReadColorFromEEPROM(ptr);

		ChangeState(new State_StaticColor(leds, NUM_LEDS, clr));
		break;
	}
	case Rainbow:
	{
		float speed = (ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION), 1) / 100.0);
		int width = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION) + 1, NUM_LEDS);

		ChangeState(new State_Rainbow(leds, NUM_LEDS, speed, width));
		break;
	}
	case RisingAndFalling:
	{
		CRGB clrs[10] = { CRGB::Red, CRGB::Green, CRGB::Blue };
		int clrsSize = 3;

		int delay = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION), 50) * 10;
		int savedColors = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION) + 1);
		if (savedColors > 10)
		{
			savedColors = 10;
		}

		if (savedColors != 0)
		{
			clrsSize = savedColors;
		}

		int pointer = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION) + 2;
		for (int i = 0; i < savedColors; i++)
		{
			//int col = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + 2 + i, 0);
			clrs[i] = ReadColorFromEEPROM(pointer);//GetColor(col, CRGB::Red);
		}

		ChangeState(new State_RisingAndFalling(leds, NUM_LEDS, clrs, clrsSize, delay));

		break;
	}
	case BurningDot:
	{
		CRGB clrs[10] = { CRGB::Red, CRGB::Green, CRGB::Blue };
		int clrsSize = 3;
		float speed = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION), 100);
		float dimming = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION) + 1, 25) / 100.f;
		int savedColors = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION) + 2, 0);

		if (savedColors > 10)
		{
			savedColors = 10;
		}

		if (savedColors != 0)
		{
			clrsSize = savedColors;
		}

		int pointer = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION) + 3;
		for (int i = 0; i < savedColors; i++)
		{
			//int col = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + 3 + i, 0);
			//clrs[i] = GetColor(col, CRGB::Red);
			clrs[i] = ReadColorFromEEPROM(pointer);
		}

		ChangeState(new State_BurningDot(leds, NUM_LEDS, clrs, clrsSize, speed, dimming));
		break;
	}
	case Breathing:
	{
		/*CRGB clrs[10] = { CRGB::Black };
		int clrsSize = 1;

		float speed = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1, 1) / 100.0f;

		int savedColors = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + 1, 0);

		if (savedColors > State_Breathing::MAX_COLORS)
		{
			savedColors = State_Breathing::MAX_COLORS;
		}

		if (savedColors != 0)
		{
			clrsSize = savedColors;
		}

		int pointer = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + 2;
		for (int i = 0; i < savedColors; i++)
		{
			//int col = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + 3 + i, 0);
			//clrs[i] = GetColor(col, CRGB::Red);
			clrs[i] = ReadColorFromEEPROM(pointer);
		}

		ChangeState(new State_Breathing(leds, NUM_LEDS, clrs, clrsSize, speed));
		break;*/

		float speed = ReadEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION), 1) / 100.0f;

		ChangeState(new State_Breathing(leds, NUM_LEDS, speed));

		break;

	}
	default:
		ChangeState(new State_StaticColor(leds, NUM_LEDS, CRGB(255, 128, 0)));
		break;
	}
}

bool SaveConfig()
{
	Configurator::Write(EEPROM_SLOT_BRIGHTNESS + (currentPreset * PRESET_SEPARATION), brightness);
	Configurator::Write(EEPROM_SLOT_DEFAULT_MODE + (currentPreset * PRESET_SEPARATION), actualState->id);
	Configurator::Write(EEPROM_SLOT_PHYSICAL_BRIGHTNESS_ENABLED + (currentPreset * PRESET_SEPARATION), physicalBrightnessControlEnabled);

	States s = (States)actualState->id;
	switch (s)
	{
	case undefinied:
		Serial.println("#!CannotSaveConfigForUndefiniedState");
		return false;
	case StaticColor:
	{
		State_StaticColor* st = (State_StaticColor*)actualState;
		SaveColorToEEPROM(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION), st->color);
		return true;
	}
	case Rainbow:
	{
		State_Rainbow* st = (State_Rainbow*)actualState;
		int val = st->speed * 100.f;
		Configurator::Write(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION), val);
		Configurator::Write(EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION) + 1, st->width);
		return true;
	}
	case RisingAndFalling:
	{
		State_RisingAndFalling* st = (State_RisingAndFalling*)actualState;
		int ptr = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION);
		Configurator::Write(ptr++, st->delay / 10);
		Configurator::Write(ptr++, st->colorSize);
		for (int i = 0; i < st->colorSize; i++)
		{
			ptr = SaveColorToEEPROM(ptr, st->colors[i]);
		}
		return true;
	}
	case BurningDot:
	{
		State_BurningDot* st = (State_BurningDot*)actualState;
		int ptr = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION);
		Configurator::Write(ptr++, st->speed);
		Configurator::Write(ptr++, st->dimming * 100.f);
		Configurator::Write(ptr++, st->colors_size);

		for (int i = 0; i < st->colors_size; i++)
		{
			ptr = SaveColorToEEPROM(ptr, st->colors[i]);
		}
		return true;
	}
	case Breathing:
	{
		/*State_Breathing* st = (State_Breathing*)actualState;
		int ptr = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1;
		Configurator::Write(ptr++, st->speed * 100.0f);
		Configurator::Write(ptr++, st->colorsSize);
		for (int i = 0; i < st->colorsSize; i++)
		{
			ptr = SaveColorToEEPROM(ptr, st->colors[i]);
		}*/
		State_Breathing* st = (State_Breathing*)actualState;
		int ptr = EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 + (currentPreset * PRESET_SEPARATION);
		Configurator::Write(ptr++, st->speed * 100.0f);
		return true;
	}
	default:
		Serial.println("#!UnknownState");
		return false;
	}

	Serial.println("#!UnknownErrorSaving");
	return false;
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

// Saves color value to EEPROM at specified slot and returns new slot value after writing values
int SaveColorToEEPROM(int slot, CRGB color)
{
	int number = GetColorNumber(color, -1);
	if (number == -1)
	{
		Configurator::Write(slot++, 254);
		Configurator::Write(slot++, color.r);
		Configurator::Write(slot++, color.g);
		Configurator::Write(slot++, color.b);
	}
	else
	{
		Configurator::Write(slot++, number);
	}

	return slot;
}

CRGB ReadColorFromEEPROM(int& slot)
{
	int val = Configurator::Read(slot++);
	CRGB clr = CRGB::Black;
	if (val == 255)
	{
		return CRGB::Black;
	}
	if (val == 254)
	{
		clr.r = Configurator::Read(slot++);
		clr.g = Configurator::Read(slot++);
		clr.b = Configurator::Read(slot++);
	}
	else
	{
		clr = GetColor(val);
	}

	return clr;
}

const int colorsSize = 6;
Color colors[colorsSize] = { Color("red", CRGB(255,0,0)), Color("blue",CRGB(0,0,255)), Color("green", CRGB(0,255,0)), Color("yellow", CRGB::Yellow), Color("pink", CRGB::Pink), Color("white", CRGB(255,255,255)) };

CRGB GetColor(int number, CRGB def)
{
	if (number < 0)
	{
		Serial.println("I don't have any color with number under 0 :/");
		return def;
	}
	if (number >= colorsSize)
	{
		Serial.println("I don't have so much colors :/");
		Serial.print("Trying to load: ");
		Serial.println(number);
		return def;
	}

	return colors[number].color;
}

int GetColorNumber(CRGB color, int errorValue)
{
	for (int i = 0; i < colorsSize; i++)
	{
		if (colors[i].color.r == color.r && colors[i].color.g == color.g && colors[i].color.b == color.b)
		{
			return i;
		}
	}
	return errorValue;
}

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

CRGB ParseStringToColor(Argument arg)
{
	return FindColor(arg.name, arg.nameSize);
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

// You HAVE TO create an arg parameter as this: char arr[] = "const"; Argument arg(arr, size); It won't work and CRASH the other way
void ByteToString(unsigned char number, Argument& arg)
{
	if (number == 0) 
	{
		arg.name[0] = '0';
		arg.nameSize = 1;
		return;
	}

	

	//char a[] = "   ";
	int pos = 0;
	for (int i = 100; i > 0; i /= 10)
	{
		if (number >= i)
		{
			if (arg.nameSize == pos)
			{
				Serial.println("#![ByteToString]TooSmallArgGiven");
				return;
			}

			char nn = ((number / i) % 10) + '0';
			//a[pos] = nn;
			arg.name[pos] = nn;
			pos++;
		}
	}

	arg.nameSize = pos;


	return;
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
