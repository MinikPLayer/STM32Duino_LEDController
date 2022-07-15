#ifdef WIFI

#pragma once


#include "CustomSerial.hpp"
#include "Commands.h"
#include "../Process.hpp"
#include "../Objects.h"

#ifdef DEV_ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
#endif

#define SSID DEVICE_NAME

const int RESPONSE_BUFFER_LENGTH = 100;

class WifiSerial : public Process
{
    IPAddress local_ip = IPAddress(192, 168, 1, 1);
    IPAddress gateway = IPAddress(192, 168, 1, 1);
    IPAddress subnet = IPAddress(255, 255, 255, 0);

    ESP8266WebServer server = ESP8266WebServer(80);

    char command[200];
    int commandPos = 0;

    bool _lock = false;
    void lock()
    {
        while(_lock)
            delay(10);

        _lock = true;
    }

    void unlock()
    {
        _lock = false;
    }

    bool write_to_buf(const String& data)
    {
        lock();

        for(int i = 0;i<data.length();i++)
        {
            command[commandPos++] = data[i];

            if(commandPos >= MAX_CMND_SIZE - 1) // -1 because of \n
                return false;
        }

        unlock();
        return true;
    }

    bool reactToCommand = false;
    void handle_OnSerial() 
    {
        for(int i = 0;i<server.args();i++) {
            // Serial.print("Arg [");
            // Serial.print(i);
            // Serial.print("] ");
            // Serial.print(server.argName(i));
            // Serial.print(" = ");
            // Serial.println(server.arg(i));

            if(server.argName(i) == "data") {
                //Serial.println("Data :)");
                if(!write_to_buf(server.arg(i)))
                    server.send(507, "text/html", "Buffer is full, no space left to fulfil the request");

                //server.send(200, "text/html", "Acknowledged");
                reactToCommand = true;

                // // Do this later to have time to send acknowledgment message
                // command[writePos++] = '\n';
                // return;

                if(command[0] == '+')
                {
                    int result = ReactToCommand(&command[1], commandPos - 1);
                    commandPos = 0;
                    FastLED.show();
                    // delay(2000);
                    // FastLED.show();
                    if (result == false)
                    {
                        //Serial.println("!BadCommandSyntax");
                        server.send(406, "text/html", "!BadCommandSyntax");
                    }
                    else if (result == 2)
                    {
                        //Serial.println("!CommandNotFound");
                        server.send(404, "text/html", "!CommandNotFound");
                    }
                    else if (result == 3)
                    {
                        //Serial.println("!CommandBadArgs");
                        server.send(400, "text/html", "!CommandBadArgs");
                    }
                    else if (result == true)
                    {
                        //Serial.println("!OK");
                        server.send(200, "text/html", "=OK");
                    }
                    else
                    {
                        //Serial.print("!CommandOtherError");
                        //Serial.println(result);
                        // print("!CommandOtherError:");
                        // println(result);
                        Serial.print("!CommandOtherError");
                        Serial.println(result);
                        server.send(500, "text/html", "Other command error");
                    }
                }
                else
                {
                    server.send(405, "text/html", "!CommandNoPlusSign");
                }

                commandPos = 0;
                reactToCommand = false;

                //return true;

                return;
            }
        }
        server.send(400, "text/html", "No data found, use ?data={data}");
    }

public:
    
    void Init()
    {
        // Ignore baudrate in this example
        WiFi.softAP(SSID);
        WiFi.softAPConfig(local_ip, gateway, subnet);
        delay(100);

        server.on("/serial", [this]() { handle_OnSerial(); });

        server.begin();
    }

    // void clear() override 
    // {
    //     lock();
    //     for(int i = 0; i < MAX_CMND_SIZE - commandPos; i++) {
    //         command[i] = command[i + commandPos];
    //     }

    //     writePos -= commandPos + 1; // +1 because of \n
    //     commandPos = 0;
    //     unlock();
    // }

    // bool available() override
    // {
    //     lock();
    //     bool ret = writePos > commandPos;
    //     unlock();
    //     return ret;
    // }

    // char read() override {
    //     return command[commandPos];
    // }

    void Tick() override 
    {
        server.handleClient();
    }

    // void print(char data[]) override {
    //     int ptr = 0;
    //     while(data[ptr] != '\0' && responseBufferPos < RESPONSE_BUFFER_LENGTH)
    //         responseBuffer[responseBufferPos++] = data[ptr++];
        
    // }

    // void println(char data[]) override {
    //     print(data);
    //     if(responseBufferPos < RESPONSE_BUFFER_LENGTH - 1)
    //         responseBuffer[responseBufferPos++] = '\n';
    // }

    // void print(const char* data) override {
    //     int ptr = 0;
    //     while(data[ptr] != '\0' && responseBufferPos < RESPONSE_BUFFER_LENGTH)
    //         responseBuffer[responseBufferPos++] = data[ptr++];
        
    // }

    // void println(const char* data) override {
    //     print(data);
    //     if(responseBufferPos < RESPONSE_BUFFER_LENGTH - 1)
    //         responseBuffer[responseBufferPos++] = '\n';
    // }

    // void print(int data) override {
    //     if(responseBufferPos < RESPONSE_BUFFER_LENGTH - 1)
    //         responseBuffer[responseBufferPos++] = data;
    // }

    // void println(int data) override {
    //     if(responseBufferPos < RESPONSE_BUFFER_LENGTH - 1)
    //         responseBuffer[responseBufferPos++] = data;

    //     if(responseBufferPos < RESPONSE_BUFFER_LENGTH - 1)
    //         responseBuffer[responseBufferPos++] = '\n';
    // }
};

#endif