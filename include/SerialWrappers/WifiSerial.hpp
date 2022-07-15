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

    char responseBuffer[MAX_CMND_SIZE];
    int responseBufferPos = 0;

    char command[MAX_CMND_SIZE];
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

    char sendBuffer[MAX_CMND_SIZE+11];
    int sendBufferLen = 0;
    void PrepareResponseBuffer()
    {
        if(responseBufferPos == MAX_CMND_SIZE)
            responseBufferPos--;

        sendBufferLen = 0;
        sendBuffer[sendBufferLen++] = '<';
        sendBuffer[sendBufferLen++] = 'p';
        sendBuffer[sendBufferLen++] = 'r';
        sendBuffer[sendBufferLen++] = 'e';
        sendBuffer[sendBufferLen++] = '>';

        for(int i = 0;i<responseBufferPos;i++) {
            sendBuffer[sendBufferLen++] = responseBuffer[i];
        }

        sendBuffer[sendBufferLen++] = '<';
        sendBuffer[sendBufferLen++] = '/';
        sendBuffer[sendBufferLen++] = 'p';
        sendBuffer[sendBufferLen++] = 'r';
        sendBuffer[sendBufferLen++] = 'e';
        sendBuffer[sendBufferLen++] = '>';

        sendBuffer[sendBufferLen] = '\0';
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
                        println("!BadCommandSyntax");
                        PrepareResponseBuffer();
                        server.send(406, "text/html", sendBuffer);
                    }
                    else if (result == 2)
                    {
                        Serial.println("[WiFi] !CommandNotFound");
                        println("!CommandNotFound");
                        PrepareResponseBuffer();
                        server.send(406, "text/html", sendBuffer);
                    }
                    else if (result == 3)
                    {
                        Serial.println("[WiFi] !CommandBadArgs");
                        println("!CommandBadArgs");
                        PrepareResponseBuffer();
                        server.send(406, "text/html", sendBuffer);
                    }
                    else if (result == true)
                    {
                        Serial.println("[WiFi] =OK");
                        println("=OK");
                        PrepareResponseBuffer();
                        server.send(406, "text/html", sendBuffer);
                    }
                    else
                    {
                        Serial.print("[WiFi] !CommandOtherError");
                        Serial.println(result);

                        print("!CommandOtherError");
                        println(result);
                        PrepareResponseBuffer();
                        server.send(406, "text/html", sendBuffer);
                    }
                }
                else
                {
                    Serial.println("[WiFi] !CommandNoPlusSign");
                    server.send(405, "text/html", "!CommandNoPlusSign");
                }

                responseBufferPos = 0;
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
        Serial.println("\n[WiFi] Connecting...\n");
        Serial.print("[WiFi] SSID: \"");
        Serial.print(ssid);
        Serial.print("\"\n[WiFi] Password: \"");
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

    bool reconfigure = false;
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
            net_ssid.clear();
            net_pass.clear();

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
        server.on("/reconfigure", [this]() { 
            server.send(200, "text/html", "OK!");
            reconfigure = true;
        });
        server.begin();
    }

    void Tick() override 
    {
        if(reconfigure) {
            Init();
            reconfigure = false;
            return;
        }

        if(shouldConnect && (millis() - lastConnectTry > connectTryDelay) && !IsConnected()) 
            Connect(net_ssid, net_pass);

        server.handleClient();
    }

    void println(const char* data) {
        while(*data != '\0' && responseBufferPos < MAX_CMND_SIZE) 
            responseBuffer[responseBufferPos++] = *data++;
        
        if(responseBufferPos < MAX_CMND_SIZE - 1)
            responseBuffer[responseBufferPos++] = '\n';
    }

    void println(int data) {
        auto str = String(data);
        for(int i = 0;i<str.length() && responseBufferPos < MAX_CMND_SIZE;i++) {
            responseBuffer[responseBufferPos++] = str[i];
        }

        if(responseBufferPos < MAX_CMND_SIZE - 1)
            responseBuffer[responseBufferPos++] = '\n';
    }

    void print(const char* data) {
        while(*data != '\0' && responseBufferPos < MAX_CMND_SIZE) 
            responseBuffer[responseBufferPos++] = *data++;
    }

    void print(int data) {
        auto str = String(data);
        for(int i = 0;i<str.length() && responseBufferPos < MAX_CMND_SIZE;i++) {
            responseBuffer[responseBufferPos++] = str[i];
        }
    }
};

#endif