#pragma once
#include "Objects.h"

class CustomSerial
{
public:
    bool tick = true;

    int serialPos = 0;
	char command[MAX_CMND_SIZE];

    virtual void begin(int baudrate) {}
    virtual char read() {return 0;}
    virtual bool available() {return false;}
    virtual void print(char data[]) {}
    virtual void println(char data[]) {}
    virtual void print(const char* data) {}
    virtual void println(const char* data) {}
    virtual void print(int data) {}
    virtual void println(int data) {}
    virtual void clear() {serialPos = 0;}
};