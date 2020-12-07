#include "Objects.h"
#include "LEDController.h"
#include "Commands.h"
#include "Util.h"

#include "States.h"

#include "Configurator.h"

#include <Arduino.h>


void ChangeState(State* newState)
{
	if(actualState == NULL)
	{
		//Serial.println("I won't delete old object, because why bother??");

		actualState = newState;
	}
	else
	{
		//Serial.println("Deleting old state");

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


States defaultState = States::undefinied;



void _setup() 
{

	Serial.begin(BAUDRATE);
	//while (!Serial);
	//while (!Serial);
	//display_allocInfo();

	// put your setup code here, to run once:
	pinMode(13, OUTPUT);
	pinMode(BRIGHTNESS_PIN, INPUT_PULLUP);
	pinMode(BRIGHTNESS_POTENTIOMETER_INPUT_PIN, OUTPUT); digitalWrite(BRIGHTNESS_POTENTIOMETER_INPUT_PIN, HIGH);

	//while (!Serial);

	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);//.setCorrection(TypicalLEDStrip);
	//FastLED.setBrightness(brightness);
	FastLED.clear();


	if (digitalRead(PB3) != LOW)
	{ 
		currentPreset = Configurator::Read(EEPROM_DEFAULT_PRESET);
		if (currentPreset >= MAX_PRESETS)
		{
			currentPreset = 0;
			Configurator::Write(EEPROM_DEFAULT_PRESET, 0);
		}

		LoadConfig();
	}
	else
	{
		brightness = 255;
		//defaultState = States::Rainbow;
		ChangeState(new State_Rainbow(leds, NUM_LEDS));
	}
	


	FastLED.show();

	//FastLED.clear();

	//actualState = new StaticColor((CRGB**)&leds, NUM_LEDS);
	//ChangeState(new State_StaticColor(leds, NUM_LEDS, CRGB(0,0,255)));
	//ChangeState(new State_RisingAndFalling(leds, NUM_LEDS, Array<CRGB>{ CRGB(255,0,0), CRGB(0, 255, 0), CRGB(0,0,254) }, 200));
	//ChangeState(new State_Rainbow(leds, NUM_LEDS, 0.1));
	//CRGB colors[] = { CRGB::Red, CRGB::Blue, CRGB::Yellow };
	//ChangeState(new State_BurningDot(leds, NUM_LEDS, colors, 3, 50, 0.35));
  

	//digitalWrite(13, HIGH);
	//delay(1000);
	//digitalWrite(13, LOW);


	Serial.println("READY");

	//actualState->Start();
}

long long int lastMicros = 0;
double deltaTime = 0;




char command[MAX_CMND_SIZE];
int serialPos = 0;
void CheckSerial()
{


  
	while(Serial.available())
	{
		while(serialPos != MAX_CMND_SIZE)
		{
			if (!Serial.available()) return;
			char c = Serial.read();

			if(c == '\n')
			{
				if(serialPos == 0) // Empty command
				{
					return;
				}
				break;
			}

			command[serialPos] = c;
			serialPos++;
		}

		if(serialPos == MAX_CMND_SIZE)
		{
			Serial.println("!CommandOverflow");
			serialPos = 0;
			return;
		}



		if(command[0] == '+')
		{
			int result = ReactToCommand(&command[1], serialPos - 1);
			if (result == false)
			{
				Serial.println("!BadCommandSyntax");
			}
			else if (result == 2)
			{
				Serial.println("!CommandNotFound");
			}
			else if (result == 3)
			{
				Serial.println("!CommandBadArgs");
			}
			else if (result == true)
			{
				Serial.println("=OK");
			}
			else
			{
				Serial.print("!CommandOtherError:");
				Serial.println(result);
			}
		}
		else
		{
			Serial.println("!CommandNoPlusSign");
		}
    

		serialPos = 0;								
	}


}

int lastState = 255;
uint32 lastMax = 0;
uint32 lastMin = 0;

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

			if (abs(lastMax - lastMin) < 4000)
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

			if (abs(lastMin - lastMax) < 4000 && !wasMin)
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
	// put your main code here, to run repeatedly:
	
	//ChangeState(new State_BurningDot(leds, NUM_LEDS, Array<CRGB>{CRGB::Red, CRGB::Blue, CRGB::Green}, 50, 0.35));

	//ChangeState(new State_BurningDot(leds, NUM_LEDS, Array<CRGB>{CRGB::Red, CRGB::Blue, CRGB::Green}, 50, 0.35));
	//ChangeState(new State_BurningDot(leds, NUM_LEDS, Array<CRGB>{CRGB::Red, CRGB::Blue, CRGB::Green}, 50, 0.35));
	//ChangeState(new State_BurningDot(leds, NUM_LEDS, Array<CRGB>{CRGB::Red, CRGB::Blue, CRGB::Green}, 50, 0.35));


	if(actualState != NULL)
	{
		actualState->Update(deltaTime);
	}
	else
	{
		Serial.println("STATE IS NULL");
	}

	CheckBrightness();
	CheckSerial();

	int res = micros() - lastMicros;
	lastMicros = micros();
	deltaTime = res/1000.0;

  



	if(actualState->updateLeds)
	{
		FastLED.show();
		actualState->updateLeds = false;
	}

	delay(2);
}
