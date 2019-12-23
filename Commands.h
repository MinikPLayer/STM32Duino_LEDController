#ifndef _COMMANDS_H
#define _COMMANDS_H



#include "Arduino.h"

class Command
{
public:
	char* name;
	int nameSize;

	Command(const char* cmd)
	{
		nameSize = 0;

		name = (char*)cmd;
		//nameSize = sizeof(cmd);
		for (int i = 0; i < 100; i++)
		{
			if (cmd[i] == '\0')
			{
				nameSize = i;
				break;
			}
		}
	}
};

class Argument
{
public:
	char* name;
	int nameSize;

	Argument()
	{
		nameSize = 0;
		name = NULL;
	}

	Argument(const char* arg)
	{
		nameSize = 0;

		name = (char*)arg;
		//nameSize = sizeof(cmd);
		for (int i = 0; i < 100; i++)
		{
			if (arg[i] == '\0')
			{
				nameSize = i;
				break;
			}
		}
	}

	Argument(char* arg, int size)
	{
		nameSize = size;
		name = arg;
	}

	void Print()
	{
		for (int i = 0; i < nameSize; i++)
		{
			Serial.print(name[i]);
		}
	}

	void Println()
	{
		Print();
		Serial.println("");
	}

	bool Equals(char* str, int length)
	{
		if (length != nameSize)
		{
			return false;
		}

		for (int i = 0; i < length; i++)
		{
			if (str[i] != name[i])
			{
				return false;
			}
		}

		return true;
	}
};

template<int T>
class Arguments
{
	Argument args[T];


public:
	int size = 0;



	Arguments()
	{
		size = 0;
	}

	/*Arguments(Argument &_args, int argSize)
	{
		if (argSize > T)
		{
			argSize = T;
		}

		for (int i = 0; i < argSize; i++)
		{
			args[i] = _args[i];
		}

		size = argSize;
	}*/

	Argument &operator[](int index)
	{
		if (index < 0 || index >= T)
		{
			Serial.println("$ArgumentOutOfRangeException");
			//return Argument("", 0);
			Argument arg("");
			return arg;
		}

		if (index >= size)
		{
			size = index + 1;
		}

		return args[index];
	}

	void set(int index, Argument value)
	{

	}
};

extern const int commandsSize;
extern Command commands[];
//int tests[] = {1,2};

bool IsCommand(char* ogCmd, int ogSize, char* checkCmd, int checkSize);

int ReactToCommand(char* cmnd, int size);

#endif // !_COMMANDS_H
