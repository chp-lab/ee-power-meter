//
// Created by Teerachot on 28/10/2563.
//

#ifndef Kit_h
#define Kit_h
#include "Arduino.h"

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266mDNS.h>
#elif defined(ARDUINO_ARCH_ESP32)
    #include <WiFi.h>
    #include <WebServer.h>
    #include <WiFiClient.h>
    #include <ESPmDNS.h>
    #include <Update.h>
#endif
    #include <AutoConnect.h>
    #include <ArduinoOTA.h>
    #include <WiFiUdp.h>

class Kit{
    private:
        int _pin;
        int _timer;
    public:
        void v();
        void ConnectWiFi();
        void HandleWiFi();
        void UploadBin();
        void UploadOTA(const char *passwordOta);
        void HandleOTA();

};

#endif
