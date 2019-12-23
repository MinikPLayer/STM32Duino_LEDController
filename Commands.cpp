#include "Commands.h"
#include "Objects.h"
#include "PrezentAneta.h"
#include "States.h"

#include "Util.h"

#include <EEPROM.h>

int brightness = 255;

const int commandsSize = 6;
Command commands[commandsSize] = {"test", "info", "chmode", "brightness", "sendparam", "eeprom"};

bool IsCommand(char* ogCmd, int ogSize, char* checkCmd, int checkSize)
{
	if(ogSize > checkSize) return false;
  
	for(int i = 0;i<ogSize;i++)
	{
		if(checkCmd[i] == ' ')
		{
			break;
		}
    
		if(ogCmd[i] != checkCmd[i])
		{
			return false;
		}
	}

	if (checkSize > ogSize)
	{
		if (checkCmd[ogSize] != ' ')
		{
			return false;
		}
	}

	return true;
}

bool IsCommand(Command cmd, char* checkCmd, int checkSize)
{
	return IsCommand(cmd.name, cmd.nameSize, checkCmd, checkSize);
}

bool ChangeMode(char* name, int nameSize)
{
	const int maxArgs = 50;

	Argument args[maxArgs];
	int argSize = 0;
	
	for (int i = 0; i < nameSize; i++)
	{
		if (name[i] == ' ')
		{
			argSize++;
			if (argSize >= maxArgs + 1)
			{
				Serial.println("#Too much arguments");
				break;
			}

			args[argSize - 1].name = name + i + 1;
		}
		else
		{
			if (argSize != 0)
			{
				args[argSize - 1].nameSize++;
			}
			
		}
	}

	/*Serial.println("Arguments: ");
	for (int i = 0; i < argSize; i++)
	{
		Serial.print(i);
		Serial.print(") \"");
		for (int j = 0; j < args[i].nameSize; j++)
		{
			Serial.print(args[i].name[j]);
		}
		Serial.println("\"");
	}*/


	// TEMPLATE
	/*
	int argState = -1;
	for (int i = 0; i < argSize; i++)
	{
		if (args[i].nameSize == 0) continue;

		if (args[i].name[0] == '-')
		{

		}
		else
		{
			if (argState == -1)
			{
				Serial.println("#Bad function parameter");
				return false;
			}

			switch (argState)
			{
			default:
				Serial.println("#Bad function parameter");
				return false;
			}

		}
	}
	*/
	if (IsCommand("rainbow", 7, name, nameSize))
	{

		float speed = 0.1;

		int argState = -1;
		for (int i = 0; i < argSize; i++)
		{
			if (args[i].nameSize == 0) continue;

			if (args[i].name[0] == '-')
			{
				if (IsCommand("-speed", 6, args[i].name, args[i].nameSize))
				{
					argState = 1;
				}
			}
			else
			{
				if (argState == -1)
				{
					Serial.println("#Bad function parameter");
					return false;
				}

				switch (argState)
				{
				case 1: // Speed
					speed = StringToInt(args[i].name, args[i].nameSize) / 100.0;
					if (speed == -1)
					{
						Serial.println("#Cannot convert speed parameter to integer");
						return false;
					}
					break;

				default:
					Serial.println("#Bad function parameter");
					return false;
				}

			}
		}

		ChangeState(new State_Rainbow(leds, NUM_LEDS, speed));
		return true;
	}
	else if(IsCommand("burning", 7, name, nameSize))
	{
		CRGB colors[10] = { CRGB::Red, CRGB::Green, CRGB::Blue };
		int colorsSize = 0;


		int argState = -1;
		for (int i = 0; i < argSize; i++)
		{
			if (args[i].nameSize == 0) continue;

			if (args[i].name[0] == '-')
			{
				if (IsCommand("-colors", 7, args[i].name, args[i].nameSize))
				{
					argState = 1;
				}
			}
			else
			{
				if (argState == -1)
				{
					Serial.println("#!Bad function parameter");
					return false;
				}

				switch (argState)
				{
				case 1:
				{

					if (colorsSize >= 10)
					{
						Serial.println("#!Too much colors specified");
						break;
					}

					CRGB c = FindColor(args[i].name, args[i].nameSize);
					if (c.r == 0 && c.g == 0 && c.b == 0)
					{
						return false;
					}

					colors[colorsSize] = c;
					colorsSize++;


					break;
				}

				default:
					Serial.println("#Bad function parameter");
					return false;
				}

			}
		}

		if (colorsSize == 0)
		{
			colorsSize = 3;
		}
		ChangeState(new State_BurningDot(leds, NUM_LEDS, colors, colorsSize, 50, 0.35));
		return true;
	}
	else if (IsCommand("solid", 5, name, nameSize))
	{
		int lastArg = -1;
		CRGB color = CRGB::Green;

		for (int i = 0; i < argSize; i++)
		{
			if (args[i].nameSize == 0) continue;

			if (args[i].name[0] == '-')
			{
				if (IsCommand("-color", 6, args[i].name, args[i].nameSize))
				{
					lastArg = 1;
				}
			}
			else
			{
				switch (lastArg)
				{
				case 1: // color
				{
					color = FindColor(args[i].name, args[i].nameSize);


					lastArg = -1;
					break;
				}
					

				default:
					Serial.println("Unknown argument");
					return false;
				}
			}


		}

		ChangeState(new State_StaticColor(leds, NUM_LEDS, color));
		return true;
	}

	return false;
}

template<int SIZE>
Arguments<SIZE> GetArguments(char* args, int size)
{

	Arguments<SIZE> ret;

	if (size == 0)
	{
		return ret;
	}

	int it = 1;
	ret[0].name = args;
	for (int i = 0; i < size; i++)
	{
		if (args[i] == ' ')
		{
			ret[it].name = args + i + 1;
			it++;
		}
		else
		{
			ret[it - 1].nameSize++;
		}
	}

	return ret;
}

int ReactToCommand(char* cmnd, int size)
{
	if (size == 0) return false;
	bool debugCmnd = false;
	if (cmnd[0] == '!')
	{
		debugCmnd = true;

		Serial.print("Command: \"");
		for(int i = 0;i<size;i++)
		{
			Serial.print(cmnd[i]);
		}
		Serial.println("\"");

		cmnd++;
		size--;
	}



	for(int i = 0;i<commandsSize;i++)
	{
		if(IsCommand(commands[i], cmnd, size))
		{
			char* args = cmnd + commands[i].nameSize; 
			int argSize = size - commands[i].nameSize; 

			if (argSize != 0) // space after
			{
				args++;
				argSize--;
			}

			//Serial.print("Arg size: ");
			//Serial.println(argSize);

			Arguments<20> a = GetArguments<20>(args, argSize);
			//Serial.print("a.size: ");
			//Serial.println(a.size);

			if (debugCmnd)
			{
				for (int i = 0; i < a.size; i++)
				{
					Serial.print(i);
					Serial.print(") \"");
					for (int k = 0; k < a[i].nameSize; k++)
					{
						Serial.print(a[i].name[k]);
					}
					Serial.println("\"");
				}
			}


			switch(i)
			{
				case 0: // Test
					Serial.println("Test");
					for (int i = 0; i < argSize; i++)
					{
						Serial.print(args[i]);
					}
					Serial.println("\"");
					return true;

				case 1: // Info
					Serial.println("|===================");
					Serial.println("| M-LED's controller");
					Serial.print("| Version: ");
					Serial.println(VERSION);
					Serial.println("| Author: Minik");
					Serial.println("| -------------------");
					Serial.print("| Actual state: ");
					actualState->name.Println();

					Serial.println("|===================");
					return true;

				case 2: // Change Mode
				{
					if (argSize <= 0)
					{
						return false;
					}

					//Serial.println(GetArguments(args, argSize)[0].name);

					bool res = ChangeMode(args, argSize);
					if (!res)
					{
						return 3;
					}

					return true;
				}

				case 3: // brightness
				{
					long long bright = StringToInt(args, argSize);
					//Serial.println(res);
					if (bright < 0 || bright > 255)
					{
						return false;
					}

					FastLED.setBrightness(bright);

					return true;
				}

				case 4: // send parameter 
				{

					if (a.size < 2)
					{
						Serial.println("#!TooLittleArguments");
						return false;
					}

					bool r = actualState->SendParameter(a[0], a[1]);
					if (r == false)
					{
						Serial.println("#!Cannot send parameter, it may be caused by an error or wrong command / parameter");
						return false;
					}

					return true;
				}

				case 5: // EEPROM
				{
					if (a.size == 0)
					{
						Serial.println("#!NoArgumentsSpecified");
						return false;
					}

					if (IsCommand("write", 5, a[0].name, a[0].nameSize))
					{
						if (a.size < 3)
						{
							Serial.println("#!TooLittleArgumentsSpecified");
							return false;
						}

						int addr = StringToInt(a[1].name, a[1].nameSize);
						if (addr < 0)
						{
							Serial.println("#!BadAddress");
							return false;
						}

						int val = StringToInt(a[2].name, a[2].nameSize);
						if (val < 0)
						{
							Serial.println("#!BadValue");
							return false;
						}
						if (val > 255)
						{
							Serial.println("#!TooBigValue");
							return false;
						}

						EEPROM.update(addr, (unsigned char)val);

						Serial.println("Written data");
					}
					else if (IsCommand("read", 4, a[0].name, a[0].nameSize))
					{
						if (a.size < 2)
						{
							Serial.println("#!TooLittleArgumentsSpecified");
							return false;
						}

						int addr = StringToInt(a[1].name, a[1].nameSize);
						if (addr < 0)
						{
							Serial.println("#!BadAddress");
							return false;
						}

						int val = (int)(EEPROM.read(addr));

						Serial.print("Readed data: ");
						Serial.println(val);
					}

					return true;
				}
			}
		}
	}

	return 2;
}
