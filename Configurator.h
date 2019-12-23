#ifndef _CONFIGURATOR_H
#define _CONFIGURATOR_H

#define EEPROM_SLOT_BRIGHTNESS 1
#define EEPROM_SLOT_DEFAULT_MODE 2
#define EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 100

#include "Commands.h"
#include "Objects.h"

struct Slot
{
	Argument name;
	int slot;

	Slot(const char* _name, int _slot)
	{
		name = Argument(_name);
		slot = _slot;
	}

	bool Equals(char* str, int length)
	{
		return name.Equals(str, length);
	}
};

class Configurator
{
public:
	static void Write(int slot, unsigned char value);
	static void Write(Slot slot, unsigned char value);
	static bool Write(char* slotname, int slotNameLength, unsigned char value);

	static unsigned char Read(int slot);
	static unsigned char Read(Slot slot);
	static unsigned char Read(char* slotname, int slotNameLength);
};




#endif // !_CONFIGURATOR_H
