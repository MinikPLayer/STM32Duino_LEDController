#ifdef WIFI

#pragma once

#include "CustomSerial.hpp"
#include "../Process.hpp"
#include "../Objects.h"

#ifdef DEV_ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
#endif

#define SSID DEVICE_NAME

class WifiSerial : public CustomSerial, public Process
{
    IPAddress local_ip = IPAddress(192, 168, 1, 1);
    IPAddress gateway = IPAddress(192, 168, 1, 1);
    IPAddress subnet = IPAddress(255, 255, 255, 0);

    ESP8266WebServer server = ESP8266WebServer(80);

    void handle_OnSerial() 
    {
        for(int i = 0;i<server.args();i++) {
            Serial.print("Arg [");
            Serial.print(i);
            Serial.print("] ");
            Serial.print(server.argName(i));
            Serial.print(" = ");
            Serial.println(server.arg(i));

            if(server.argName(i) == "data") {
                Serial.println("Data :)");
            }
        }
        server.send(200, "text/html", "OK");
    }

public:
    void begin(int baudrate) override
    {
        // Ignore baudrate in this example
        WiFi.softAP(SSID);
        WiFi.softAPConfig(local_ip, gateway, subnet);
        delay(100);

        server.on("/config", [this]() { handle_OnSerial(); });

        server.begin();
    }

    bool available() override
    {
        return false;
    }

    void Tick() override 
    {
        server.handleClient();
    }
};

#endif