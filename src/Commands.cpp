#include "Commands.h"
#include "Objects.h"            // Serial_Print("Arg [");
            // Serial_Print(i);
            // Serial_Print("] ");
            // Serial_Print(server.argName(i));
            // Serial_Print(" = ");
            // Serial_Println(server.arg(i));
#include "LEDController.h"
#include "States.h"

#include "Util.h"

#include "Configurator.h"

#include <EEPROM.h>



const int commandsSize = 10;
Command commands[commandsSize] = {"test", "info", "set", "get", "setparam", "eeprom", "save", "getparam", "load", "easter"};

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
				Serial_Println("#Too much arguments");
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

	/*Serial_Println("Arguments: ");
	for (int i = 0; i < argSize; i++)
	{
		Serial_Print(i);
		Serial_Print(") \"");
		for (int j = 0; j < args[i].nameSize; j++)
		{
			Serial_Print(args[i].name[j]);
		}
		Serial_Println("\"");
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
				Serial_Println("#Bad function parameter");
				return false;
			}

			switch (argState)
			{
			default:
				Serial_Println("#Bad function parameter");
				return false;
			}

		}
	}
	*/
	if (IsCommand("rainbow", 7, name, nameSize))
	{

		float speed = 0.1;
		int width = NUM_LEDS;

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
				else if (IsCommand("-width", 6, args[i].name, args[i].nameSize))
				{
					argState = 2;
				}
			}
			else
			{
				if (argState == -1)
				{
					Serial_Println("#Bad function parameter");
					return false;
				}

				switch (argState)
				{
				case 1: // Speed
					speed = StringToInt(args[i].name, args[i].nameSize) / 100.0;
					if (speed == -1)
					{
						Serial_Println("#Cannot convert speed parameter to integer");
						return false;
					}
					break;

				case 2: // width
				{
					width = StringToInt(args[i].name, args[i].nameSize);
					if (width == -1)
					{
						Serial_Println("#Cannot convert speed parameter to integer");
						return false;
					}
					break;
				}

				default:
					Serial_Println("#Bad function parameter");
					return false;
				}

			}
		}

		ChangeState(new State_Rainbow(leds, NUM_LEDS, speed, width));
		return true;
	}
	else if(IsCommand("burning", 7, name, nameSize))
	{
		CRGB colors[State_BurningDot::MAX_COLORS] = { CRGB::Red, CRGB::Green, CRGB::Blue };
		int colorsSize = 0;

    float speed = 50;
    float dimming = 0.35;


		int argState = -1;
		for (int i = 0; i < argSize; i++)
		{
			if (args[i].nameSize == 0) continue;

			if (args[i].name[0] == '-')
			{
				if (IsCommand("-colors", 7, args[i].name, args[i].nameSize))
					argState = 1;
        else if(IsCommand("-speed", 6, args[i].name, args[i].nameSize))
          argState = 2;
        else if(IsCommand("-dimming", 8, args[i].name, args[i].nameSize))
          argState = 3;
			}
			else
			{
				if (argState == -1)
				{
					Serial_Println("#!Bad function parameter");
					return false;
				}

				switch (argState)
				{
				case 1:
				{

					if (colorsSize >= State_BurningDot::MAX_COLORS)
					{
						Serial_Println("#!Too much colors specified");
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
        case 2: // speed
        {
          int val = StringToInt(args[i].name, args[i].nameSize);

          speed = val / 100.f;
          break;
        }
        case 3: // dimming
        {
          int val = StringToInt(args[i].name, args[i].nameSize);

          dimming = val / 100.f;
          break;
        }

				default:
					Serial_Println("#Bad function parameter");
					return false;
				}

			}
		}

		if (colorsSize == 0)
		{
			colorsSize = 3;
		}
		ChangeState(new State_BurningDot(leds, NUM_LEDS, colors, colorsSize, speed, dimming));
		return true;
	}
	else if (IsCommand("static", 6, name, nameSize) || IsCommand("solid", 5, name, nameSize))
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
					Serial_Println("Unknown argument");
					return false;
				}
			}


		}

		ChangeState(new State_StaticColor(leds, NUM_LEDS, color));
		return true;
	}
	else if (IsCommand("risingandfalling", 16, name, nameSize) || IsCommand("raf", 3, name, nameSize))
	{
		CRGB colors[State_RisingAndFalling::MAX_COLORS] = { CRGB::Red, CRGB::Green, CRGB::Blue };
		int colorsSize = 0;

		int delay = 50;


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
				else if (IsCommand("-delay", 6, args[i].name, args[i].nameSize))
				{
					argState = 2;
				}
			}
			else
			{
				if (argState == -1)
				{
					Serial_Println("#!Bad function parameter");
					return false;
				}

				switch (argState)
				{
				case 1:
				{

					if (colorsSize >= State_RisingAndFalling::MAX_COLORS)
					{
						Serial_Println("#!Too much colors specified");
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

				case 2:
				{
					int val = StringToInt(args[i].name, args[i].nameSize);
					if (val == -1)
					{
						return false;
					}
					delay = val;
					
					break;
				}

				default:
					Serial_Println("#Bad function parameter");
					return false;
				}

			}
		}

		if (colorsSize == 0)
		{
			colorsSize = 3;
		}
		//ChangeState(new State_BurningDot(leds, NUM_LEDS, colors, colorsSize, 50, 0.35));
		ChangeState(new State_RisingAndFalling(leds, NUM_LEDS, colors, colorsSize, delay));
		return true;
	}
	else if (IsCommand("breathing", 9, name, nameSize))
	{
		float speed = 0.03;


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
					Serial_Println("#!Bad function parameter");
					return false;
				}

				switch (argState)
				{
				case 1:
				{

					speed = StringToInt(args[i].name, args[i].nameSize) / 100.f;
					if (speed == -1)
					{
						Serial_Println("#!CannotConvertStringToInt");
						return false;
					}


					break;
				}


				default:
					Serial_Println("#Bad function parameter");
					return false;
				}

			}
		}


		ChangeState(new State_Breathing(leds, NUM_LEDS, speed));
		return true;
	}
  else if(IsCommand("gradient", 8, name, nameSize))
  {
     int lastArg = -1;
     CRGB colors[2];
     colors[0] = CRGB(255,0,0);
     colors[1] = CRGB(0,0,255);
     bool smooth = false;
     for (int i = 0; i < argSize; i++)
     {
        if (args[i].nameSize == 0) continue;
  
        if (args[i].name[0] == '-')
        {
          if (IsCommand("-color1", 7, args[i].name, args[i].nameSize))
          {
            lastArg = 1;
          }
          else if (IsCommand("-color2", 7, args[i].name, args[i].nameSize))
          {
            lastArg = 2;
          }
          else if(IsCommand("-smooth", 7, args[i].name, args[i].nameSize))
          {
            smooth = true; 
          }
          else if(IsCommand("-rough", 6, args[i].name, args[i].nameSize))
          {
            smooth = false;
          }
        }
        else
        {
          switch (lastArg)
          {
          case 1: // color 1
          {
            colors[0] = FindColor(args[i].name, args[i].nameSize);
  
  
            lastArg = -1;
            break;
          }
          case 2: // color 2
          {
            colors[1] = FindColor(args[i].name, args[i].nameSize);

            lastArg = -1;
            break;
          }
            
  
          default:
            Serial_Println("#!Unknown argument");
            return false;
          }
        }

    }

    ChangeState(new State_Gradient(leds, NUM_LEDS, colors, smooth));
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

		Serial_Print("Command: \"");
		for(int i = 0;i<size;i++)
		{
			Serial_Print(cmnd[i]);
		}
		Serial_Println("\"");

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

			//Serial_Print("Arg size: ");
			//Serial_Println(argSize);

			Arguments<20> a = GetArguments<20>(args, argSize);
			//Serial_Print("a.size: ");
			//Serial_Println(a.size);

			if (debugCmnd)
			{
				for (int i = 0; i < a.size; i++)
				{
					Serial_Print(i);
					Serial_Print(") \"");
					for (int k = 0; k < a[i].nameSize; k++)
					{
						Serial_Print(a[i].name[k]);
					}
					Serial_Println("\"");
				}
			}


			switch(i)
			{
				case 0: // Test
					/*Serial_Println("Test");
					for (int i = 0; i < argSize; i++)
					{
						Serial_Print(args[i]);
					}
					Serial_Println("\"");*/
					return true;

				case 1: // Info
				{
					if (a.size > 0)
					{
						if (a[0] == "commands")
						{
							for (int i = 0; i < commandsSize; i++)
							{
								Serial_Print("| ");
								for (int j = 0; j < commands[i].nameSize; j++)
								{
									Serial_Print(commands[i].name[j]);
								}
								Serial_Println("");
							}
							return true;
						}
						else if (a[0] == "params")
						{
							for (int i = 0; i < actualState->params.size; i++)
							{
								Serial_Print("| ");
								actualState->params[i].Println();
							}
							return true;
						}
						return false;
					}

					Serial_Println("|===================");
					Serial_Println("| M-LED's controller");
					Serial_Print("| Version: ");
					Serial_Println(VERSION);
					Serial_Println("| Author: Minik");
					Serial_Println("| -------------------");
					Serial_Print("| Actual state: ");
					actualState->name.Print();
					Serial_Print("| Device name: ");
					Serial_Println(DEVICE_NAME);
					Serial_Println("|===================");
					return true;
				}

				case 2: // set global param
				{
					if (a.size < 2) return false; // param name + value

					if (a[0] == "bright" || a[0] == "brightness")
					{
						if (a[1] == "-physical")
						{
							if (a.size < 3)
							{
								return false;
							}
							if (a[2] == "1" || a[2] == "on")
							{
								physicalBrightnessControlEnabled = true;
							}
							else if (a[2] == "0" || a[2] == "off")
							{
								physicalBrightnessControlEnabled = false;
							}
							else
							{
								physicalBrightnessControlEnabled = !physicalBrightnessControlEnabled;
							}

							return true;
						}

						int value = StringToInt(a[1].name, a[1].nameSize);

						if (value < 0 || value > 255)
						{
							Serial_Println("#!BadBrightnessValue");
							return false;
						}

						brightness = value;

						FastLED.setBrightness(brightness);
						FastLED.show();

						return true;
					}
					else if (a[0] == "state")
					{
						int size = 0;
						for (int i = 1; i < a.size; i++)
						{
							size += a[i].nameSize + 1; // Size + space
						}
						size--; // Because there's no space at the end
						bool res = ChangeMode(a[1].name, size);//a[1].nameSize);
						if (!res)
						{
							return 3;
						}

						return true;
					}
					else if (a[0] == "LED" || a[0] == "led")
					{
						if (a.size < 3)
						{
							return false;
						}

						int pin = StringToInt(a[1].name, a[1].nameSize);
						if (pin < 0 || pin >= NUM_LEDS)
						{
							Serial_Println("#!Bad pin number");
							return false;
						}

						CRGB color = ParseStringToColor(a[2]);

						leds[pin] = color;

						FastLED.show();

						return true;
					}
					else if (a[0] == "net_ssid")
					{
						if(a.size < 2)
							return false;

						String arg;
						for(int i = 1; i < a.size; i++) {
							for(int j = 0; j < a[i].nameSize; j++) {
								arg.concat(a[i].name[j]);
							}
							if(i != a.size - 1)
								arg.concat(' ');
						}
						
						if(arg.length() >= 32)
						{
							Serial_Println("#!This device only supports up to 32 characters long SSIDs");
							return false;
						}
						Configurator::Write(EEPROM_SLOT_WIFI_SSID_LENGTH, arg.length());
						for(int i = 0;i<arg.length();i++)
							Configurator::Write(EEPROM_SLOT_WIFI_SSID + i, arg[i]);

						return true;
						
					}
					else if (a[0] == "net_pass")
					{
						if(a.size < 2)
							return false;

						String arg;
						for(int i = 1; i < a.size; i++) {
							for(int j = 0; j < a[i].nameSize; j++) {
								arg.concat(a[i].name[j]);
							}
							if(i != a.size - 1)
								arg.concat(' ');
						}
						
						if(arg.length() >= 32)
						{
							Serial_Println("#!This device only supports up to 32 characters long password");
							return false;
						}
						Configurator::Write(EEPROM_SLOT_WIFI_PASS_LENGTH, arg.length());
						for(int i = 0;i<arg.length();i++)
							Configurator::Write(EEPROM_SLOT_WIFI_PASS + i, arg[i]);

						return true;
						
					}
					else if (a[0] == "preset")
					{
						Serial_Println("#!Presets are disabled");
						return false;
						// if (a.size < 2)
						// {
						// 	return false;
						// }

						// if (a[1] == "-default")
						// {
						// 	Configurator::Write(EEPROM_DEFAULT_PRESET, currentPreset);
						// 	return true;
						// }


						// int val = StringToInt(a[1].name, a[1].nameSize);

						// if (val < 0)
						// {
						// 	Serial_Println("Bad preset value");
						// 	return false;
						// }
						// if (val >= MAX_PRESETS)
						// {
						// 	Serial_Println("Too big preset, not enough memory");
						// 	return false;
						// }

						// currentPreset = val;

						// LoadConfig();
						// return true;
					}

					return false;
				}

				case 3: // get global param
				{

					if (a.size == 0)
					{
						Serial_Println("#!No arguments passed");
						return false;
					}

					if (a[0] == "bright" || a[0] == "brightness")
					{
						if (a.size > 1)
						{
							if (a[1] == "-physical")
							{
								PrintParam(physicalBrightnessControlEnabled);
								return true;
							}
						}

						PrintParam(brightness);
						return true;
					}
					else if (a[0] == "state")
					{
						Serial_Print("| ");
						actualState->name.Println();
						return true;
					}
					else if (a[0] == "LED" || a[0] == "led")
					{
						char arr[] = "             ";
						Argument arg(arr, 13);

						int number = StringToInt(a[1].name, a[1].nameSize);
						if (number < 0 || number >= NUM_LEDS)
						{
							Serial_Println("#!Bad pin number");
							return false;
						}



						ParseColorToString(leds[number], arg);
						Serial_Print("| ");
						arg.Println();

						return true;
					}
					else if (a[0] == "preset")
					{

						Serial_Print("| ");
						Serial_Println(currentPreset);

						return true;
					}
					else if(a[0] == "ledC")
					{
						Serial_Print("| ");
						Serial_Println(NUM_LEDS);

						return true;
					}
					else if(a[0] == "potAvailable")
					{
						Serial_Print("| ");
						Serial_Println(POTENTIOMETER_ENABLED);

						return true;
					}
					else if(a[0] == "deviceName")
					{
						Serial_Print("| ");
						Serial_Println(DEVICE_NAME);

						return true;
					}

					return false;
				}

				case 4: // set parameter 
				{

					if (a.size < 2)
					{
						Serial_Println("#!TooLittleArguments");
						return false;
					}

					a[0].nameSize++; // space, it's deleted later
					int paramPos = 0;
					for (int i = 1; i < a.size; i++)
					{
						if (a[i].name[0] != '-')
						{
							if (paramPos > 0)
							{
								break;
							}

							a[paramPos].nameSize--;
							paramPos++;
							a[paramPos].name = a[i].name;
							a[paramPos].nameSize = a[i].nameSize + 1;
							break;
						}
						a[paramPos].nameSize += a[i].nameSize + 1; // Size + space
					}
					if (paramPos == 0)
					{
						Serial_Println("#!NotEnoughArguments");
						return false;
					}
					a[1].nameSize--; // Because there's no space at the end

					bool r = actualState->SetParameter(a[0], a[1]);
					if (r == false)
					{
						Serial_Println("#!Cannot send parameter, it may be caused by an error or wrong command / parameter");
						return false;
					}

					return true;
				}

				case 5: // EEPROM
				{
					if (a.size == 0)
					{
						Serial_Println("#!NoArgumentsSpecified");
						return false;
					}

					if (IsCommand("write", 5, a[0].name, a[0].nameSize))
					{
						if (a.size < 3)
						{
							Serial_Println("#!TooLittleArgumentsSpecified");
							return false;
						}

						int addr = StringToInt(a[1].name, a[1].nameSize);
						if (addr < 0)
						{
							Serial_Println("#!BadAddress");
							return false;
						}

						int val = StringToInt(a[2].name, a[2].nameSize);
						if (val < 0)
						{
							Serial_Println("#!BadValue");
							return false;
						}
						if (val > 255)
						{
							Serial_Println("#!TooBigValue");
							return false;
						}

						Configurator::Write(addr, (unsigned char)val);

						Serial_Println("Written data");
					}
					else if (IsCommand("read", 4, a[0].name, a[0].nameSize))
					{
						if (a.size < 2)
						{
							Serial_Println("#!TooLittleArgumentsSpecified");
							return false;
						}

						int addr = StringToInt(a[1].name, a[1].nameSize);
						if (addr < 0)
						{
							Serial_Println("#!BadAddress");
							return false;
						}

						int val = (int)(Configurator::Read(addr));

						Serial_Print("EEPROM [");
						Serial_Print(addr);
						Serial_Print("] = ");
						Serial_Println(val);
					}
					else if(IsCommand("erase", 5, a[0].name, a[0].nameSize))
					{
						Configurator::Erase();
					}
					else
					{
						return false;
					}

					return true;
				}
				case 6: // save
				{
					SaveConfig();
					return true;
				}
				case 7: // getparam
				{
					if (a.size == 0) return false;

					bool state = actualState->GetParameter(a[0]);
					if (!state)
					{
						Serial_Println("#!ParameterNotFoundOrOtherError");
						return false;
					}
					return true;

				}
				case 8: // Load
				{
					// Presets are disabled
					/*if (a.size > 0)
					{
						int val = StringToInt(a[0].name, a[0].nameSize);

						if (val < 0 || val >= MAX_PRESETS)
						{
							Serial_Println("Wrong preset value");
							return false;
						}

						currentPreset = val;
					}*/

					LoadConfig();
					return true;
				}
				
			}
		}
	}

	return 2;
}
