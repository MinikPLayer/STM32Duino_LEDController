#include "PrezentAneta.h"
#include "Commands.h"
#include "Util.h"

#include "States.h"




void ChangeState(State* newState)
{
	if(actualState == NULL)
	{
		Serial.println("I won't delete old object, because why bother??");

		actualState = newState;
	}
	else
	{
		Serial.println("Deleting old state");

		delete actualState;
		actualState = newState;
	}

	newState->Start();

	fill_solid(leds, NUM_LEDS, CRGB::Black);
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


void _setup() 
{

	Serial.begin(115200);

	//while (!Serial);
	//display_allocInfo();

	// put your setup code here, to run once:
	pinMode(13, OUTPUT);

	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);

	FastLED.clear();
	FastLED.show();

	//FastLED.clear();

	//actualState = new StaticColor((CRGB**)&leds, NUM_LEDS);
	//ChangeState(new State_StaticColor(leds, NUM_LEDS, CRGB(0,0,255)));
	//ChangeState(new State_RisingAndFalling(leds, NUM_LEDS, Array<CRGB>{ CRGB(255,0,0), CRGB(0, 255, 0), CRGB(0,0,254) }, 200));
	ChangeState(new State_Rainbow(leds, NUM_LEDS, 0.1));
	//CRGB colors[] = { CRGB::Red, CRGB::Blue, CRGB::Yellow };
	//ChangeState(new State_BurningDot(leds, NUM_LEDS, colors, 3, 50, 0.35));
  

	//digitalWrite(13, HIGH);
	//delay(1000);
	//digitalWrite(13, LOW);


	Serial.println("READY");

	actualState->Start();
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
			Serial.println("!Overflow");
			serialPos = 0;
			return;
		}



		if(command[0] == '+')
		{
			int result = ReactToCommand(&command[1], serialPos - 1);
			if (result == false)
			{
				Serial.println("!BadSyntax");
			}
			else if (result == 2)
			{
				Serial.println("!NotFound");
			}
			else if (result == 3)
			{
				Serial.println("!BadArgs");
			}
			else if (result == true)
			{
				Serial.println("OK");
			}
			else
			{
				Serial.print("!OtherError:");
				Serial.println(result);
			}
		}
		else
		{
			Serial.println("!NoPlusSign");
		}
    

		serialPos = 0;								
	}


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

	CheckSerial();

	int res = micros() - lastMicros;
	lastMicros = micros();
	deltaTime = res/1000.0;

  



	if(actualState->updateLeds)
	{
		FastLED.show();
		actualState->updateLeds = false;
	}
}
