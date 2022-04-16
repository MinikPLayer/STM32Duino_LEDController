#pragma once

class CustomSerial
{
public:
    virtual void begin(int baudrate) {}
    virtual char read() {return 0;}
    virtual bool available() {return false;}
    virtual void print(char data[]) {}
    virtual void println(char data[]) {}
    virtual void print(int data) {}
    virtual void println(int data) {}
};