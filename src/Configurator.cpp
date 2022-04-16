#include "Configurator.h"

#include <EEPROM.h>

#ifdef ESP
	#define ESP_EEPROM_SIZE 1024
#endif

const static int slotsSize = 3;
Slot slots[slotsSize] = { Slot("brightness", EEPROM_SLOT_BRIGHTNESS), Slot("defaultmode", EEPROM_SLOT_DEFAULT_MODE), Slot("defaultmodeparam1", EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1) };

int FindSlot(char* name, int length)
{
	for (int i = 0; i < slotsSize; i++)
	{
		if (slots[i].Equals(name, length))
		{
			return i;
		}
	}

	return -1;
}



void Configurator::Initialize()
{
	#ifdef ESP
		EEPROM.begin(ESP_EEPROM_SIZE);
	#endif
}

void Configurator::Erase()
{
	#if defined(STM32)
		uint16_t length = 0;
		EEPROM.count(&length);
	#elif defined(ESP)
		length = ESP_EEPROM_SIZE;
	#else
		auto length = EEPROM.length();
	#endif
	
	for(int i = 0;i<length;i++)
		EEPROM.write(i, 255);

	#ifdef ESP32
		EEPROM.commit();
	#endif
}

void Configurator::Write(int address, int value)
{
	if (value > 255)
	{
		Serial.println("#@[EEPROM]TooBigValue");
		value = 254;
	}
	if (value < 0)
	{
		Serial.println("#@[EEPROM]TooSmallValue");
		value = 0;
	}

	#if defined ESP32 || defined ESP8266
		auto val = EEPROM.read(address);
		if(val == value)
			return;
		EEPROM.write(address, value);
		EEPROM.commit();
	#else
		EEPROM.update(address, value);
	#endif
}

void Configurator::Write(Slot slot, int value)
{
	Write(slot.slot, value);
}

bool Configurator::Write(char* slotname, int slotNameLength, int value)
{
	int slot = FindSlot(slotname, slotNameLength);
	if (slot == -1)
	{
		Serial.println("#!SlotNotFound");
		return false;
	}

	Write(slot, value);

	return true;
}

unsigned char Configurator::Read(int slot)
{
	return EEPROM.read(slot);
}

unsigned char Configurator::Read(Slot slot)
{
	return Read(slot.slot);
}

unsigned char Configurator::Read(char* slotname, int slotNameLength)
{
	int slot = FindSlot(slotname, slotNameLength);
	if (slot == -1)
	{
		Serial.println("#!SlotNotFound");
		return 255;
	}

	return Read(slot);
}