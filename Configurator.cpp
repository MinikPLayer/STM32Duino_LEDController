#include "Configurator.h"

#include <EEPROM.h>

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

void Configurator::Write(int slot, int value)
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
	EEPROM.update(slot, value);
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