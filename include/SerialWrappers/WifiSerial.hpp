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
            if(server.argName(i) == "data") {
                //Serial.println("Data :)");
                if(!write_to_buf(server.arg(i)))
                    server.send(507, "text/html", "Buffer is full, no space left to fulfil the request");

                reactToCommand = true;

                if(command[0] == '+')
                {
                    int result = ReactToCommand(&command[1], commandPos - 1);
                    commandPos = 0;
                    FastLED.show();
                    if (result == false)
                    {
                        Serial.println("[WiFi] !BadCommandSyntax");
                        server.send(406, "text/html", "!BadCommandSyntax");
                    }
                    else if (result == 2)
                    {
                        Serial.println("[WiFi] !CommandNotFound");
                        server.send(404, "text/html", "!CommandNotFound");
                    }
                    else if (result == 3)
                    {
                        Serial.println("[WiFi] !CommandBadArgs");
                        server.send(400, "text/html", "!CommandBadArgs");
                    }
                    else if (result == true)
                    {
                        Serial.println("[WiFi] !OK");
                        server.send(200, "text/html", "=OK");
                    }
                    else
                    {
                        Serial.print("[WiFi] !CommandOtherError");
                        Serial.println(result);
                        server.send(500, "text/html", "Other command error");
                    }
                }
                else
                {
                    Serial.println("[WiFi] !CommandNoPlusSign");
                    server.send(405, "text/html", "!CommandNoPlusSign");
                }

                commandPos = 0;
                reactToCommand = false;

                return;
            }
        }
        server.send(400, "text/html", "No data found, use ?data={data}");
    }

public:
    
    void Init()
    {
        WiFi.softAP(SSID);
        WiFi.softAPConfig(local_ip, gateway, subnet);
        delay(100);

        server.on("/serial", [this]() { handle_OnSerial(); });

        server.begin();
    }

    void Tick() override 
    {
        server.handleClient();
    }
};

#endif