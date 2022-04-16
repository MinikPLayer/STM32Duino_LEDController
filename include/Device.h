#pragma once

#define STM32_BluePill
//#define ESP32
//#define ESP8266

#ifdef STM32_BluePill
    #define STM32
#endif

#if defined(ESP32) || defined(ESP8266)
    #define ESP
#endif

#ifdef ESP32
    #define WIFI
    #define BT
    #define DAC
#endif

#ifdef ESP8266
    #define WIFI
#endif