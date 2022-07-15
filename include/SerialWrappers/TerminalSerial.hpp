#pragma once

#include "CustomSerial.hpp"
#include "Arduino.h"
#include "Objects.h"

class TerminalSerial : public CustomSerial
{
public:
    void begin(int baudrate) override { Serial.begin(baudrate); }
    char read() override { return Serial.read(); }
    bool available() override { return Serial.available(); }
    void print(char data[]) override { Serial.print(data); }
    void println(char data[]) override { Serial.println(data); }
    void print(const char* data) override { Serial.print(data); }
    void println(const char* data) override { Serial.println(data); }
    void print(int data) override { Serial.print(data); }
    void println(int data) override { Serial.println(data); }
};