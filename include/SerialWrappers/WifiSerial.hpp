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

#define AP_SSID DEVICE_NAME

const int RESPONSE_BUFFER_LENGTH = 100;

class WifiSerial : public Process
{
    IPAddress local_ip = IPAddress(192, 168, 1, 1);
    IPAddress gateway = IPAddress(192, 168, 1, 1);
    IPAddress subnet = IPAddress(255, 255, 255, 0);

    ESP8266WebServer server = ESP8266WebServer(80);

    String net_ssid;
    String net_pass;

    const long long connectTryDelay = 1000 * 300; // Retry to connect every 5min
    long long lastConnectTry = 0;

    char command[200];
    int commandPos = 0;

    bool shouldConnect = false;

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

    void handle_OnSerial() 
    {
        for(int i = 0;i<server.args();i++) {
            if(server.argName(i) == "data") {
                //Serial.println("Data :)");
                if(!write_to_buf(server.arg(i)))
                    server.send(507, "text/html", "Buffer is full, no space left to fulfil the request");

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

                return;
            }
        }
        server.send(400, "text/html", "No data found, use ?data={data}");
    }

    bool IsConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    bool Connect(String& ssid, String& pass, int timeoutCount = 100)
    {
        Serial.println("[WiFi] Connecting...\n\n");
        Serial.print("[WiFi] SSID: \"");
        Serial.print(ssid);
        Serial.print("\"\n\n[WiFi] Password: \"");
        Serial.print(pass);
        Serial.println("\"");

        WiFi.disconnect();
        WiFi.begin(ssid, pass);
        WiFi.setAutoReconnect(true);
        lastConnectTry = millis();
        for(int i = 0;i<timeoutCount;i++)
        {
            if(IsConnected()) {
                Serial.println("[WiFi] Connected!");
                return true;
            }

            delay(100);
        }

        Serial.println("[WiFi] Can't connect to network");
        SetupAsSoftAP();

        return false;
    }

    void SetupAsSoftAP()
    {
        WiFi.disconnect();
        Serial.println("[WiFi] Setting up as Soft AP mode");
        Serial.print("[WiFi] Using SSID \"");
        Serial.print(AP_SSID);
        Serial.println("\"");
        WiFi.softAP(AP_SSID);
        WiFi.softAPConfig(local_ip, gateway, subnet);
        delay(100);
    }

public:
    void Init()
    {

        int wifiSSIDLength = Configurator::Read(EEPROM_SLOT_WIFI_SSID_LENGTH);
        int wifiPassLength = Configurator::Read(EEPROM_SLOT_WIFI_PASS_LENGTH);
        if(wifiSSIDLength == 0) // Wifi not set-up
        {
            shouldConnect = false;

            SetupAsSoftAP();
        }
        else {
            shouldConnect = true;

            for(int i = 0;i<wifiSSIDLength;i++)
            {
                char c = Configurator::Read(EEPROM_SLOT_WIFI_SSID + i);
                net_ssid.concat(c);
            }

            for(int i = 0;i<wifiPassLength;i++)
            {
                char c = Configurator::Read(EEPROM_SLOT_WIFI_PASS + i);
                net_pass.concat(c);
            }

            Connect(net_ssid, net_pass);
        }

        server.on("/serial", [this]() { handle_OnSerial(); });
        server.on("/reconfigure", [this]() { Init(); });
        server.begin();
    }

    void Tick() override 
    {
        if(shouldConnect && (millis() - lastConnectTry > connectTryDelay) && !IsConnected()) 
            Connect(net_ssid, net_pass);

        server.handleClient();
    }
};

#endif