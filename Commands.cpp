#include "Commands.h"
#include "Objects.h"
#include "PrezentAneta.h"
#include "States.h"

#include "Util.h"

const int commandsSize = 5;
Command commands[commandsSize] = {"ping", "test", "chmode", "brightness", "sendparam"};

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
		ChangeState(new State_StaticColor(leds, NUM_LEDS, CRGB::Green));
		return true;
	}

	return false;
}

int ReactToCommand(char* cmnd, int size)
{
	Serial.print("Command: \"");
	for(int i = 0;i<size;i++)
	{
		Serial.print(cmnd[i]);
	}
	Serial.println("\"");


	for(int i = 0;i<commandsSize;i++)
	{
		if(IsCommand(commands[i], cmnd, size))
		{
			char* args = cmnd + commands[i].nameSize + 1; // space
			int argSize = size - commands[i].nameSize - 1; // space

			switch(i)
			{
				case 0: // Ping
					Serial.print("Pong! \"");
					for (int i = 0; i < argSize; i++)
					{
						Serial.print(args[i]);
					}
					Serial.println("\"");
					return true;

				case 1: // Test
					Serial.println("Test");
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
					Argument arg(args, argSize);

					break;
				}
			}
		}
	}

	return 2;
}
