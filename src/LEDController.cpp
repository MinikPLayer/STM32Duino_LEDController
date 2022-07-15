#include "Objects.h"
#include "LEDController.h"
#include "Commands.h"
#include "Util.h"
#include "States.h"
#include "Configurator.h"
#include "SerialWrappers/TerminalSerial.hpp"
#include "SerialWrappers/WifiSerial.hpp"
#include "Process.hpp"

#include <Arduino.h>


void ChangeState(State* newState)
{
	if(actualState == NULL)
	{
		actualState = newState;
	}
	else
	{
		delete actualState;
		actualState = newState;
	}

	fill_solid(leds, NUM_LEDS, CRGB::Black);

	newState->Start();

	
}

void ChangeLED(int number, CRGB color, bool show)
{
	if(number >= NUM_LEDS)
	{
		return;
	}
	if(number < 0)
	{
		return;
	}

	leds[number] = color;

	if(show)
	{
		FastLED.show();
	}
}

WifiSerial* wSerial = nullptr;
States defaultState = States::undefinied;
const bool debugMode = false;

void Serial_Println(int data)
{
	Serial.println(data);
	if(wSerial != nullptr) {
		wSerial->println(data);
	}
}

void Serial_Println(const char* data)
{
	Serial.println(data);
	if(wSerial != nullptr) {
		wSerial->println(data);
	}
}

void Serial_Print(int data) 
{
	Serial.print(data);
	if(wSerial != nullptr) {
		wSerial->print(data);
	}
}

void Serial_Print(const char* data)
{
	Serial.print(data);
	if(wSerial != nullptr) {
		wSerial->print(data);
	}
}

void _setup() 
{
	Configurator::Initialize();

	Serial.begin(BAUDRATE);
	//while (!Serial);
	//while (!Serial);
	//display_allocInfo();

	// put your setup code here, to run once:
	pinMode(LED_BUILTIN, OUTPUT);

  #if POTENTIOMETER_ENABLED == true
  	pinMode(BRIGHTNESS_PIN, INPUT_PULLUP);
  	pinMode(BRIGHTNESS_POTENTIOMETER_INPUT_PIN, OUTPUT); 
  	digitalWrite(BRIGHTNESS_POTENTIOMETER_INPUT_PIN, HIGH);
  #endif

	//while (!Serial);

	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);//.setCorrection(TypicalLEDStrip);
	//FastLED.setBrightness(brightness);
	FastLED.clear();


	//if (digitalRead(PB3) != LOW)
	if(!debugMode)
	{ 
		currentPreset = Configurator::Read(EEPROM_DEFAULT_PRESET);
		// If preset is higher than max or == 255 (not set), reset to defaults
		if (currentPreset >= MAX_PRESETS || currentPreset == 255)
		{
			currentPreset = 1;
			Configurator::Write(EEPROM_DEFAULT_PRESET, currentPreset);

			brightness = 255;
			ChangeState(new State_Rainbow(leds, NUM_LEDS));

			SaveConfig();
		}

		Serial.print("Loading preset ");
		Serial.print(currentPreset);
		Serial.println("...");

		LoadConfig();
	}
	else
	{
		brightness = 255;
		//defaultState = States::Rainbow;
		ChangeState(new State_Rainbow(leds, NUM_LEDS));
	}

	#ifdef WIFI
		wSerial = new WifiSerial();
		wSerial->Init();
	#endif


	FastLED.show();
	Serial.println("READY");
}

long long int lastMicros = 0;
double deltaTime = 0;

class SerialInstance
{
	SerialInstance(CustomSerial* serialPtr)
	{
		serial = serialPtr;
		if(serial == nullptr)
			return;
		serial->serialPos = 0;
		for(int i = 0;i<MAX_CMND_SIZE;i++)
			serial->command[i] = 0;

		serial->begin(BAUDRATE);
	}

public:
	CustomSerial* serial = nullptr;

	template<class T>
	static SerialInstance Create(bool tick = true)
	{
		auto t = SerialInstance(new T());
		t.serial->tick = tick;
		return t;
	}

	void CheckSerial();

	~SerialInstance()
	{
		if(serial != nullptr)
		{
			delete serial;
			serial = nullptr;
		}
	}
};

SerialInstance serialInstances[] = 
{
	SerialInstance::Create<TerminalSerial>(),
};

void CheckSerialInterfaces()
{
	int count = sizeof(serialInstances) / sizeof(SerialInstance);
	
	for(int i = 0;i<count;i++)
		serialInstances[i].CheckSerial();
}

void SerialInstance::CheckSerial()
{ 
	if(serial == nullptr)
		return;

	while(serial->available())
	{
		while(serial->serialPos != MAX_CMND_SIZE)
		{
			if (!serial->available()) 
				return;
			char c = serial->read();
			// Serial.print("Command: \"");
			// Serial.print(serial->command);
			// Serial.println("\"");

			if(c == '\r')
				continue;

			if(c == '\n')
			{
				if(serial->serialPos == 0) // Empty command
				{
					return;
				}
				break;
			}

			serial->command[serial->serialPos] = c;
			serial->serialPos++;
		}

		if(serial->serialPos == MAX_CMND_SIZE)
		{
			serial->println("!CommandOverflow");
			serial->serialPos = 0;
			return;
		}

		if(serial->command[0] == '+')
		{
			int result = ReactToCommand(&serial->command[1], serial->serialPos - 1);
			if (result == false)
			{
				//Serial.println("!BadCommandSyntax");
				serial->println("!BadCommandSyntax");
			}
			else if (result == 2)
			{
				//Serial.println("!CommandNotFound");
				serial->println("!CommandNotFound");
			}
			else if (result == 3)
			{
				//Serial.println("!CommandBadArgs");
				serial->println("!CommandBadArgs");
			}
			else if (result == true)
			{
				//Serial.println("!OK");
				serial->println("=OK");
			}
			else
			{
				//Serial.print("!CommandOtherError");
				//Serial.println(result);
				serial->print("!CommandOtherError:");
				serial->println(result);
			}
		}
		else
		{
			serial->println("!CommandNoPlusSign");
		}

		serial->clear();						
	}


}

uint32_t abs_l(uint32_t v)
{
	return v < 0 ? -v : v;
}

int lastState = 255;
uint32_t lastMax = 0;
uint32_t lastMin = 0;

bool wasMax = false;
bool wasMin = false;
void CheckBrightness()
{
	if (!physicalBrightnessControlEnabled) return;

	int bright = analogRead(BRIGHTNESS_PIN) / 16;
	//Serial.print("Bright pin: ");
	//Serial.println(bright);


	//Serial.println(bright);
	//Serial.print(lastMin);
	//Serial.print(", ");
	//Serial.println(lastMax);
	if (bright > 250)
	{
		if (!wasMax)
		{
			lastMax = millis();

			if (abs_l(lastMax - lastMin) < 4000)
			{
				if (!(lastMax == 0 || lastMin == 0))
				{
					currentPreset++;
					if (currentPreset >= MAX_PRESETS)
					{
						currentPreset = 0;
					}

					LoadConfig();

					lastMin = 0;
					lastMax = 0;
				}
			}
			wasMax = true;
		}
	}
	else
	{
		wasMax = false;
	}
	if (bright < 5)
	{
		if (!wasMin)
		{
			lastMin = millis();

			if (abs_l(lastMin - lastMax) < 4000 && !wasMin)
			{
				if (!(lastMax == 0 || lastMin == 0))
				{
					currentPreset--;
					if (currentPreset < 0)
					{
						currentPreset = MAX_PRESETS - 1;
					}

					LoadConfig();

					lastMin = 0;
					lastMax = 0;
				}
			}
			wasMin = true;
		}
	}
	else
	{
		wasMin = false;
	}
	
	int diff = abs(lastState - bright);
	if (diff < 5 && !(bright == 0 && lastState != 0))
	{
		return;
	}

	lastState = bright;
	brightness = bright;


	FastLED.setBrightness(bright);
	FastLED.show();


}


void _loop() {
	if(actualState != NULL)
	{
		actualState->Update(deltaTime);
	}
	else
	{
		Serial.println("STATE IS NULL");
	}

  #if POTENTIOMETER_ENABLED == true
	  CheckBrightness();
  #endif
    
	CheckSerialInterfaces();

	int res = micros() - lastMicros;
	lastMicros = micros();
	deltaTime = res/1000.0;

  
	Process::_RunTicks();


	if(actualState->updateLeds)
	{
		FastLED.show();
		actualState->updateLeds = false;
	}
	

	delay(2);
}
