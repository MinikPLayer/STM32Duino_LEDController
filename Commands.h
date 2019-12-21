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
};

extern const int commandsSize;
extern Command commands[];
//int tests[] = {1,2};

bool IsCommand(char* ogCmd, int ogSize, char* checkCmd, int checkSize);

int ReactToCommand(char* cmnd, int size);

#endif // !_COMMANDS_H
