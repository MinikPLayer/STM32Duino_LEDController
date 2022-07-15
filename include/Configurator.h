#ifndef _CONFIGURATOR_H
#define _CONFIGURATOR_H

#define EEPROM_SLOT_BRIGHTNESS 1
#define EEPROM_SLOT_DEFAULT_MODE 2
#define EEPROM_SLOT_PHYSICAL_BRIGHTNESS_ENABLED 3
#define EEPROM_DEFAULT_PRESET 4

// WiFi Settings
#define EEPROM_SLOT_WIFI_SSID_LENGTH 5 // Max 32B
#define EEPROM_SLOT_WIFI_SSID 6 // SSID - Max 32B
#define EEPROM_SLOT_WIFI_PASS_LENGTH 38 // Max 32B (6 + 32)
#define EEPROM_SLOT_WIFI_PASS 39 // (38 + 1)

#define EEPROM_SLOT_DEFAULT_MODE_PARAMETER_1 EEPROM_SLOT_WIFI_PASS + 32


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
	static void Initialize();
	static void Erase();

	static void Write(int slot, int value);
	static void Write(Slot slot, int value);
	static bool Write(char* slotname, int slotNameLength, int value);

	static unsigned char Read(int slot);
	static unsigned char Read(Slot slot);
	static unsigned char Read(char* slotname, int slotNameLength);
};




#endif // !_CONFIGURATOR_H
