/****************************************************************************************************************************
   BlynkSimpleEsp32_SSL_Async_WM.h
   For ESP32 boards

   Blynk_Async_WM is a library, using AsyncWebServer instead of (ESP8266)WebServer for the ESP8266/ESP32 to enable easy
   configuration/reconfiguration and autoconnect/autoreconnect of WiFi/Blynk.
   
   Based on and modified from Blynk library v0.6.1 (https://github.com/blynkkk/blynk-library/releases)
   Built by Khoi Hoang (https://github.com/khoih-prog/Blynk_Async_WM)
   Licensed under MIT license
   Version: 1.0.16

   Original Blynk Library author:
   @file       BlynkSimpleEsp8266.h
   @author     Volodymyr Shymanskyy
   @license    This project is released under the MIT License (MIT)
   @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
   @date       Jan 2015
   @brief

   Version    Modified By   Date      Comments
   -------    -----------  ---------- -----------
    1.0.16    K Hoang      25/08/2020 Initial coding to use (ESP)AsyncWebServer instead of (ESP8266)WebServer. 
                                      Bump up to v1.0.16 to sync with Blynk_WM v1.0.16
 *****************************************************************************************************************************/

#ifndef BlynkSimpleEsp32_SSL_Async_WM_h
#define BlynkSimpleEsp32_SSL_Async_WM_h

#ifndef ESP32
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

#if defined(BLYNK_SSL_USE_LETSENCRYPT)
static const char BLYNK_DEFAULT_ROOT_CA[] =
#include <certs/letsencrypt_pem.h>
#else
static const char BLYNK_DEFAULT_ROOT_CA[] =
#include <certs/blynkcloud_pem.h>
#endif

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <ESPAsyncWebServer.h>

#define HTTP_PORT     80

  //default to use EEPROM, otherwise, use SPIFFS
#if USE_SPIFFS
#include <FS.h>
#include "SPIFFS.h"
#else
#include <EEPROM.h>
#endif

///////// NEW for DRD /////////////
// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS)
// Otherwise, library will use default EEPROM storage
#define ESP8266_DRD_USE_RTC     false   //true

#if USE_SPIFFS
#define ESP_DRD_USE_EEPROM      false
#define ESP_DRD_USE_SPIFFS      true
#else
#define ESP_DRD_USE_EEPROM      true
#define ESP_DRD_USE_SPIFFS      false
#endif

#ifndef DOUBLERESETDETECTOR_DEBUG
#define DOUBLERESETDETECTOR_DEBUG     false
#endif

#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

//DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
DoubleResetDetector* drd;

///////// NEW for DRD /////////////


#include <esp_wifi.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

  template <typename Client>
class BlynkArduinoClientSecure
  : public BlynkArduinoClientGen<Client>
{
  public:
    BlynkArduinoClientSecure(Client& client)
      : BlynkArduinoClientGen<Client>(client)
      , caCert(NULL)
    {}

    void setRootCA(const char* fp) {
      caCert = fp;
    }

    bool connect()
    {
      //KH
      if (this->connected())
        return true;

      // Synchronize time useing SNTP. This is necessary to verify that
      // the TLS certificates offered by the server are currently valid.
      configTime(0, 0, "pool.ntp.org", "time.nist.gov");
      time_t now = time(nullptr);

      int i = 0;
      while ( (i++ < 30) && (now < 100000) ) 
      {
        delay(1000);
        now = time(nullptr);
      }

      struct tm timeinfo;
      gmtime_r(&now, &timeinfo);
      String ntpTime = asctime(&timeinfo);
      ntpTime.trim();
      BLYNK_LOG2("NTP time: ", ntpTime);
      
      this->client->setCACert(caCert);

      if (BlynkArduinoClientGen<Client>::connect())
      {
        BLYNK_LOG1(BLYNK_F("Certificate OK"));
        return true;
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("Secure connection failed"));
      }

      return false;
    }

  private:
    const char* caCert;
};

#define MAX_ID_LEN                5
#define MAX_DISPLAY_NAME_LEN      16

typedef struct
{
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
} MenuItem;

extern uint16_t NUM_MENU_ITEMS;
extern MenuItem myMenuItems [];

#define SSID_MAX_LEN      32
// WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN      64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

#define BLYNK_SERVER_MAX_LEN      32
#define BLYNK_TOKEN_MAX_LEN       36

typedef struct
{
  char blynk_server[BLYNK_SERVER_MAX_LEN];
  char blynk_token [BLYNK_TOKEN_MAX_LEN];
}  Blynk_Credentials;

#define NUM_WIFI_CREDENTIALS      2
#define NUM_BLYNK_CREDENTIALS     2

// Configurable items besides fixed Header
#define NUM_CONFIGURABLE_ITEMS    ( 2 + (2 * NUM_WIFI_CREDENTIALS) + (2 * NUM_BLYNK_CREDENTIALS) )
typedef struct Configuration
{
  char header         [16];
  WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
  Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
  int  blynk_port;
  char board_name     [24];
  int  checkSum;
} Blynk_WM_Configuration;
// Currently CONFIG_DATA_SIZE  =  ( 48 + (96 * NUM_WIFI_CREDENTIALS) + (68 * NUM_BLYNK_CREDENTIALS) ) = 376

uint16_t CONFIG_DATA_SIZE = sizeof(Blynk_WM_Configuration);

extern bool LOAD_DEFAULT_CONFIG_DATA;
extern Blynk_WM_Configuration defaultConfig;

// Permit special chars such as # and %

// -- HTML page fragments
const char BLYNK_WM_HTML_HEAD[]     /*PROGMEM*/ = "<!DOCTYPE html><html><head><title>BlynkSimpleEsp32_SSL_WM</title><style>div,input{padding:2px;font-size:1em;}input{width:95%;}\
body{text-align: center;}button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.5rem;margin:0px;}\
</style></head><div style=\"text-align:left;display:inline-block;min-width:260px;\">\
<fieldset><div><label>WiFi SSID</label><input value=\"[[id]]\"id=\"id\"><div></div></div>\
<div><label>PWD</label><input value=\"[[pw]]\"id=\"pw\"><div></div></div>\
<div><label>WiFi SSID1</label><input value=\"[[id1]]\"id=\"id1\"><div></div></div>\
<div><label>PWD1</label><input value=\"[[pw1]]\"id=\"pw1\"><div></div></div></fieldset>\
<fieldset><div><label>Blynk Server</label><input value=\"[[sv]]\"id=\"sv\"><div></div></div>\
<div><label>Token</label><input value=\"[[tk]]\"id=\"tk\"><div></div></div>\
<div><label>Blynk Server1</label><input value=\"[[sv1]]\"id=\"sv1\"><div></div></div>\
<div><label>Token1</label><input value=\"[[tk1]]\"id=\"tk1\"><div></div></div>\
<div><label>Port</label><input value=\"[[pt]]\"id=\"pt\"><div></div></div></fieldset>\
<fieldset><div><label>Board Name</label><input value=\"[[nm]]\"id=\"nm\"><div></div></div></fieldset>";
const char BLYNK_WM_FLDSET_START[]  /*PROGMEM*/ = "<fieldset>";
const char BLYNK_WM_FLDSET_END[]    /*PROGMEM*/ = "</fieldset>";
const char BLYNK_WM_HTML_PARAM[]    /*PROGMEM*/ = "<div><label>{b}</label><input value='[[{v}]]'id='{i}'><div></div></div>";
const char BLYNK_WM_HTML_BUTTON[]   /*PROGMEM*/ = "<button onclick=\"sv()\">Save</button></div>";
const char BLYNK_WM_HTML_SCRIPT[]   /*PROGMEM*/ = "<script id=\"jsbin-javascript\">\
function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+encodeURIComponent(val);request.open('GET',url,false);request.send(null);}\
function sv(){udVal('id',document.getElementById('id').value);udVal('pw',document.getElementById('pw').value);\
udVal('id1',document.getElementById('id1').value);udVal('pw1',document.getElementById('pw1').value);\
udVal('sv',document.getElementById('sv').value);udVal('tk',document.getElementById('tk').value);\
udVal('sv1',document.getElementById('sv1').value);udVal('tk1',document.getElementById('tk1').value);\
udVal('pt',document.getElementById('pt').value);udVal('nm',document.getElementById('nm').value);";

const char BLYNK_WM_HTML_SCRIPT_ITEM[]  /*PROGMEM*/ = "udVal('{d}',document.getElementById('{d}').value);";
const char BLYNK_WM_HTML_SCRIPT_END[]   /*PROGMEM*/ = "alert('Updated');}</script>";
const char BLYNK_WM_HTML_END[]          /*PROGMEM*/ = "</html>";
///


#define BLYNK_SERVER_HARDWARE_PORT    9443

#define BLYNK_BOARD_TYPE      "SSL_ESP32"
#define NO_CONFIG             "blank"

template <typename Transport>
class BlynkWifi
  : public BlynkProtocol<Transport>
{
    typedef BlynkProtocol<Transport> Base;
  public:
    BlynkWifi(Transport& transp)
      : Base(transp)
    {}

    void connectWiFi(const char* ssid, const char* pass)
    {
      BLYNK_LOG2(BLYNK_F("Con2:"), ssid);
      WiFi.mode(WIFI_STA);

      if (static_IP != IPAddress(0, 0, 0, 0))
      {
        BLYNK_LOG1(BLYNK_F("UseStatIP"));
        WiFi.config(static_IP, static_GW, static_SN, static_DNS1, static_DNS2);
      }

      setHostname();

      if (WiFi.status() != WL_CONNECTED)
      {
        if (pass && strlen(pass))
        {
          WiFi.begin(ssid, pass);
        } else
        {
          WiFi.begin(ssid);
        }
      }
      while (WiFi.status() != WL_CONNECTED)
      {
        BlynkDelay(500);
      }

      BLYNK_LOG1(BLYNK_F("Conn2WiFi"));
      displayWiFiData();
    }

    void config(const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_DEFAULT_PORT_SSL,
                const char* root_ca = BLYNK_DEFAULT_ROOT_CA)
    {
      Base::begin(auth);
      this->conn.begin(domain, port);
      this->conn.setRootCA(root_ca);
    }

    void config(const char* auth,
                IPAddress   ip,
                uint16_t    port = BLYNK_DEFAULT_PORT_SSL,
                const char* root_ca = BLYNK_DEFAULT_ROOT_CA)
    {
      Base::begin(auth);
      this->conn.begin(ip, port);
      this->conn.setRootCA(root_ca);
    }

    void begin(const char* auth,
               const char* ssid,
               const char* pass,
               const char* domain = BLYNK_DEFAULT_DOMAIN,
               uint16_t    port   = BLYNK_DEFAULT_PORT_SSL,
               const char* root_ca = BLYNK_DEFAULT_ROOT_CA)
    {
      connectWiFi(ssid, pass);
      config(auth, domain, port, root_ca);
      while (this->connect() != true) {}
    }

    void begin(const char* auth,
               const char* ssid,
               const char* pass,
               IPAddress   ip,
               uint16_t    port   = BLYNK_DEFAULT_PORT_SSL,
               const char* root_ca = BLYNK_DEFAULT_ROOT_CA)
    {
      connectWiFi(ssid, pass);
      config(auth, ip, port, root_ca);
      while (this->connect() != true) {}
    }

#ifndef LED_BUILTIN
#define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
#endif

    // For ESP32
#define LED_OFF     LOW
#define LED_ON      HIGH

    void begin(const char *iHostname = "")
    {
#define TIMEOUT_CONNECT_WIFI			30000

      //Turn OFF
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LED_OFF);
      
      //// New DRD ////
      drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);  
      bool noConfigPortal = true;
   
      if (drd->detectDoubleReset())
      {
#if ( BLYNK_WM_DEBUG > 1)        
        BLYNK_LOG1(BLYNK_F("Double Reset Detected"));
#endif
        
        noConfigPortal = false;
      }
      //// New DRD ////

#if ( BLYNK_WM_DEBUG > 2)    
      if (LOAD_DEFAULT_CONFIG_DATA) 
      {   
        BLYNK_LOG1(BLYNK_F("======= Start Default Config Data ======="));
        displayConfigData(defaultConfig);
      }
#endif

      WiFi.mode(WIFI_STA);

      if (iHostname[0] == 0)
      {
        String _hostname = "ESP32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
        _hostname.toUpperCase();

        getRFC952_hostname(_hostname.c_str());

      }
      else
      {
        // Prepare and store the hostname only not NULL
        getRFC952_hostname(iHostname);
      }

      BLYNK_LOG2(BLYNK_F("Hostname="), RFC952_hostname);

      //// New DRD ////
      //  noConfigPortal when getConfigData() OK and no DRD'ed
      if (getConfigData() && noConfigPortal)
      //// New DRD ////
      {
        hadConfigData = true;
        
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG1(noConfigPortal? BLYNK_F("bg: noConfigPortal = true") : BLYNK_F("bg: noConfigPortal = false"));
#endif          

        for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++)
        {
          wifiMulti.addAP(BlynkESP32_WM_config.WiFi_Creds[i].wifi_ssid, BlynkESP32_WM_config.WiFi_Creds[i].wifi_pw);
        }

        if (connectMultiWiFi() == WL_CONNECTED)
        {
          BLYNK_LOG1(BLYNK_F("bg: WiFi OK. Try Blynk"));

          int i = 0;
          while ( (i++ < 10) && !connectMultiBlynk() )
          {
          }

          if  (this->connected())
          {
            BLYNK_LOG1(BLYNK_F("bg: WiFi+Blynk OK"));
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("bg: WiFi OK, Blynk not"));
            // failed to connect to Blynk server, will start configuration mode
            startConfigurationMode();
          }
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("bg: Fail2connect WiFi+Blynk"));
          // failed to connect to Blynk server, will start configuration mode
          startConfigurationMode();
        }
      }
      else
      {       
        BLYNK_LOG2(BLYNK_F("bg: Stay forever in config portal."), 
                   noConfigPortal ? BLYNK_F("No configDat") : BLYNK_F("DRD detected"));

        // failed to connect to Blynk server, will start configuration mode
        hadConfigData = false;
        startConfigurationMode();
      }
    }

#ifndef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   10000L
#else
    // Force range of user-defined TIMEOUT_RECONNECT_WIFI between 10-60s
#if (TIMEOUT_RECONNECT_WIFI < 10000L)
#warning TIMEOUT_RECONNECT_WIFI too low. Reseting to 10000
#undef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   10000L
#elif (TIMEOUT_RECONNECT_WIFI > 60000L)
#warning TIMEOUT_RECONNECT_WIFI too high. Reseting to 60000
#undef TIMEOUT_RECONNECT_WIFI
#define TIMEOUT_RECONNECT_WIFI   60000L
#endif
#endif

#ifndef RESET_IF_CONFIG_TIMEOUT
#define RESET_IF_CONFIG_TIMEOUT   true
#endif

#ifndef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET          10
#else
    // Force range of user-defined TIMES_BEFORE_RESET between 2-100
#if (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET < 2)
#warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too low. Reseting to 2
#undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   2
#elif (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET > 100)
#warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too high. Reseting to 100
#undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   100
#endif
#endif

    void run()
    {
      static int retryTimes = 0;
      
      //// New DRD ////
      // Call the double reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call drd.stop() when you wish to no longer
      // consider the next reset as a double reset.
      drd->loop();
      //// New DRD ////

      // Lost connection in running. Give chance to reconfig.
      if ( WiFi.status() != WL_CONNECTED || !this->connected() )
      {
        // If configTimeout but user hasn't connected to configWeb => try to reconnect WiFi / Blynk.
        // But if user has connected to configWeb, stay there until done, then reset hardware
        if ( configuration_mode && ( configTimeout == 0 ||  millis() < configTimeout ) )
        {
          retryTimes = 0;

          return;
        }
        else
        {
#if RESET_IF_CONFIG_TIMEOUT
          // If we're here but still in configuration_mode, permit running TIMES_BEFORE_RESET times before reset hardware
          // to permit user another chance to config.
          if ( configuration_mode && (configTimeout != 0) )
          {
            if (++retryTimes <= CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET)
            {
              BLYNK_LOG2(BLYNK_F("run: WiFi lost, configTimeout. Connect WiFi+Blynk. Retry#:"), retryTimes);
            }
            else
            {
              ESP.restart();
            }
          }
#endif

          // Not in config mode, try reconnecting before force to config mode
          if ( WiFi.status() != WL_CONNECTED )
          {
            BLYNK_LOG1(BLYNK_F("run: WiFi lost. Reconnect WiFi+Blynk"));
            if (connectMultiWiFi() == WL_CONNECTED)
            {
              // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
              digitalWrite(LED_BUILTIN, LED_OFF);

              BLYNK_LOG1(BLYNK_F("run: WiFi reconnected. Connect to Blynk"));

              if (this->connectMultiBlynk())
              {
                BLYNK_LOG1(BLYNK_F("run: WiFi+Blynk reconnected"));
              }
            }
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("run: Blynk lost. Connect Blynk"));

            if (this->connectMultiBlynk())
            {
              // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
              digitalWrite(LED_BUILTIN, LED_OFF);
              BLYNK_LOG1(BLYNK_F("run: Blynk reconnected"));
            }
          }

          //BLYNK_LOG1(BLYNK_F("run: Lost connection => configMode"));
          //startConfigurationMode();
        }
      }
      else if (configuration_mode)
      {
        configuration_mode = false;
        BLYNK_LOG1(BLYNK_F("run: got WiFi+Blynk back"));
        // Turn the LED_BUILTIN OFF when out of configuration mode. ESP32 LED_BUILDIN is correct polarity, LOW to turn OFF
        digitalWrite(LED_BUILTIN, LED_OFF);
      }

      //if (this->connected())
      {
        Base::run();
      }
    }

    void setHostname(void)
    {
      if (RFC952_hostname[0] != 0)
      {
        // See https://github.com/espressif/arduino-esp32/issues/2537
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(RFC952_hostname);
      }
    }

    void setConfigPortalIP(IPAddress portalIP = IPAddress(192, 168, 4, 1))
    {
      portal_apIP = portalIP;
    }

    void setConfigPortal(String ssid = "", String pass = "")
    {
      portal_ssid = ssid;
      portal_pass = pass;
    }

#define MIN_WIFI_CHANNEL      1
#define MAX_WIFI_CHANNEL      12

    int setConfigPortalChannel(int channel = 1)
    {
      // If channel < MIN_WIFI_CHANNEL - 1 or channel > MAX_WIFI_CHANNEL => channel = 1
      // If channel == 0 => will use random channel from MIN_WIFI_CHANNEL to MAX_WIFI_CHANNEL
      // If (MIN_WIFI_CHANNEL <= channel <= MAX_WIFI_CHANNEL) => use it
      if ( (channel < MIN_WIFI_CHANNEL - 1) || (channel > MAX_WIFI_CHANNEL) )
        WiFiAPChannel = 1;
      else if ( (channel >= MIN_WIFI_CHANNEL - 1) && (channel <= MAX_WIFI_CHANNEL) )
        WiFiAPChannel = channel;

      return WiFiAPChannel;
    }
    
    void setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn = IPAddress(255, 255, 255, 0),
                              IPAddress dns_address_1 = IPAddress(0, 0, 0, 0),
                              IPAddress dns_address_2 = IPAddress(0, 0, 0, 0))
    {
      static_IP     = ip;
      static_GW     = gw;
      static_SN     = sn;

      // Default to local GW
      if (dns_address_1 == IPAddress(0, 0, 0, 0))
        static_DNS1   = gw;
      else
        static_DNS1   = dns_address_1;

      // Default to Google DNS (8, 8, 8, 8)
      if (dns_address_2 == IPAddress(0, 0, 0, 0))
        static_DNS2   = IPAddress(8, 8, 8, 8);
      else
        static_DNS2   = dns_address_2;
    }

    String getWiFiSSID(uint8_t index)
    { 
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");
        
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkESP32_WM_config.WiFi_Creds[index].wifi_ssid));
    }

    String getWiFiPW(uint8_t index)
    {
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");
        
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkESP32_WM_config.WiFi_Creds[index].wifi_pw));
    }
    
    String getServerName(uint8_t index)
    {
      if (index >= NUM_BLYNK_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(BlynkESP32_WM_config.Blynk_Creds[index].blynk_server));
    }

    String getToken(uint8_t index)
    {
      if (index >= NUM_BLYNK_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(BlynkESP32_WM_config.Blynk_Creds[index].blynk_token));
    }

    String getBoardName()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(BlynkESP32_WM_config.board_name));
    }

    int getHWPort()
    {
      if (!hadConfigData)
        getConfigData();

      return (BlynkESP32_WM_config.blynk_port);
    }

    Blynk_WM_Configuration* getFullConfigData(Blynk_WM_Configuration *configData)
    {
      if (!hadConfigData)
        getConfigData();

      // Check if NULL pointer
      if (configData)
        memcpy(configData, &BlynkESP32_WM_config, sizeof(Blynk_WM_Configuration));

      return (configData);
    }

    void clearConfigData()
    {
      memset(&BlynkESP32_WM_config, 0, sizeof(BlynkESP32_WM_config));
      saveAllConfigData();
    }

  private:
    AsyncWebServer *server;
    bool configuration_mode = false;

    WiFiMulti wifiMulti;

    unsigned long configTimeout;
    bool hadConfigData = false;
    
    // default to channel 1
    int WiFiAPChannel = 1;

    Blynk_WM_Configuration BlynkESP32_WM_config;
    
    uint16_t totalDataSize = 0;

    // For Config Portal, from Blynk_WM v1.0.5
    IPAddress portal_apIP = IPAddress(192, 168, 4, 1);

    String portal_ssid = "";
    String portal_pass = "";

    // For static IP, from Blynk_WM v1.0.5
    IPAddress static_IP   = IPAddress(0, 0, 0, 0);
    IPAddress static_GW   = IPAddress(0, 0, 0, 0);
    IPAddress static_SN   = IPAddress(255, 255, 255, 0);
    IPAddress static_DNS1 = IPAddress(0, 0, 0, 0);
    IPAddress static_DNS2 = IPAddress(0, 0, 0, 0);

#define RFC952_HOSTNAME_MAXLEN      24
    char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];

    char* getRFC952_hostname(const char* iHostname)
    {
      memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

      size_t len = ( RFC952_HOSTNAME_MAXLEN < strlen(iHostname) ) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

      size_t j = 0;

      for (size_t i = 0; i < len - 1; i++)
      {
        if ( isalnum(iHostname[i]) || iHostname[i] == '-' )
        {
          RFC952_hostname[j] = iHostname[i];
          j++;
        }
      }
      // no '-' as last char
      if ( isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-') )
        RFC952_hostname[j] = iHostname[len - 1];

      return RFC952_hostname;
    }

    void displayConfigData(Blynk_WM_Configuration configData)
    {
      BLYNK_LOG4(BLYNK_F("Hdr="),       configData.header,
                 BLYNK_F(",BrdName="),  configData.board_name);
      BLYNK_LOG4(BLYNK_F("SSID="),      configData.WiFi_Creds[0].wifi_ssid,
                 BLYNK_F(",PW="),       configData.WiFi_Creds[0].wifi_pw);
      BLYNK_LOG4(BLYNK_F("SSID1="),     configData.WiFi_Creds[1].wifi_ssid,
                 BLYNK_F(",PW1="),      configData.WiFi_Creds[1].wifi_pw);
      BLYNK_LOG4(BLYNK_F("Server="),    configData.Blynk_Creds[0].blynk_server,
                 BLYNK_F(",Token="),    configData.Blynk_Creds[0].blynk_token);
      BLYNK_LOG4(BLYNK_F("Server1="),   configData.Blynk_Creds[1].blynk_server,
                 BLYNK_F(",Token1="),   configData.Blynk_Creds[1].blynk_token);
      BLYNK_LOG2(BLYNK_F("Port="),      configData.blynk_port);
      BLYNK_LOG1(BLYNK_F("======= End Config Data ======="));
    }

    void displayWiFiData(void)
    {
      BLYNK_LOG6(BLYNK_F("IP="), WiFi.localIP().toString(), BLYNK_F(",GW="), WiFi.gatewayIP().toString(),
                 BLYNK_F(",SN="), WiFi.subnetMask().toString());
      BLYNK_LOG4(BLYNK_F("DNS1="), WiFi.dnsIP(0).toString(), BLYNK_F(",DNS2="), WiFi.dnsIP(1).toString());
    }

    int calcChecksum()
    {
      int checkSum = 0;
      for (uint16_t index = 0; index < (sizeof(BlynkESP32_WM_config) - sizeof(BlynkESP32_WM_config.checkSum)); index++)
      {
        checkSum += * ( ( (byte*) &BlynkESP32_WM_config ) + index);
      }

      return checkSum;
    }

#if USE_SPIFFS

#define  CONFIG_FILENAME              BLYNK_F("/wmssl_conf.dat")
#define  CONFIG_FILENAME_BACKUP       BLYNK_F("/wmssl_conf.bak")

#define  CREDENTIALS_FILENAME         BLYNK_F("/wmssl_cred.dat")
#define  CREDENTIALS_FILENAME_BACKUP  BLYNK_F("/wmssl_cred.bak")

    bool checkDynamicData(void)
    {
      int checkSum = 0;
      int readCheckSum;
      char* readBuffer;
           
      File file = SPIFFS.open(CREDENTIALS_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCredFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = SPIFFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCredFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
      }
      
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      uint16_t maxBufferLength = 0;
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > maxBufferLength)
          maxBufferLength = myMenuItems[i].maxlen;
      }
      
      if (maxBufferLength > 0)
      {
        readBuffer = new char[ maxBufferLength + 1 ];
        
        // check to see NULL => stop and return false
        if (readBuffer == NULL)
        {
          BLYNK_LOG1(BLYNK_F("ChkCrR: Error can't allocate buffer."));
          return false;
        }
#if ( BLYNK_WM_DEBUG > 2)          
        else
        {
          BLYNK_LOG2(BLYNK_F("ChkCrR: Buffer allocated, sz="), maxBufferLength + 1);
        }
#endif             
      }
     
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;

        // Actual size of pdata is [maxlen + 1]
        memset(readBuffer, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      // Free buffer
      if (readBuffer != NULL)
      {
        free(readBuffer);
        BLYNK_LOG1(BLYNK_F("Buffer freed"));
      }
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    bool loadDynamicData(void)
    {
      int checkSum = 0;
      int readCheckSum;
      totalDataSize = sizeof(BlynkESP32_WM_config) + sizeof(readCheckSum);
      
      File file = SPIFFS.open(CREDENTIALS_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCredFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = SPIFFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCredFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
      }
     
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;

        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CrR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    void saveDynamicData(void)
    {
      int checkSum = 0;
    
      File file = SPIFFS.open(CREDENTIALS_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("SaveCredFile "));

      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;

#if ( BLYNK_WM_DEBUG > 2)          
        BLYNK_LOG4(F("CW1:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));    
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }   
           
      BLYNK_LOG2(F("CrWCSum=0x"), String(checkSum, HEX));
      
      // Trying open redundant Auth file
      file = SPIFFS.open(CREDENTIALS_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("SaveBkUpCredFile "));

      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;

#if ( BLYNK_WM_DEBUG > 2)         
        BLYNK_LOG4(F("CW2:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));    
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }   
    }

    void loadConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCfgFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCfgFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return;
        }
      }

      file.readBytes((char *) &BlynkESP32_WM_config, sizeof(BlynkESP32_WM_config));

      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
    }

    void saveConfigData(void)
    {
      File file = SPIFFS.open(CONFIG_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("SaveCfgFile "));

      int calChecksum = calcChecksum();
      BlynkESP32_WM_config.checkSum = calChecksum;
      BLYNK_LOG2(BLYNK_F("WCSum=0x"), String(calChecksum, HEX));

      if (file)
      {
        file.write((uint8_t*) &BlynkESP32_WM_config, sizeof(BlynkESP32_WM_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }

      // Trying open redundant Auth file
      file = SPIFFS.open(CONFIG_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("SaveBkUpCfgFile "));

      if (file)
      {
        file.write((uint8_t *) &BlynkESP32_WM_config, sizeof(BlynkESP32_WM_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }
    }
    
    void saveAllConfigData(void)
    {
      saveConfigData();     
      saveDynamicData();
    }


    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      bool dynamicDataValid;
      int calChecksum;
      
      hadConfigData = false;
      
      // Format SPIFFS if not yet
      if (!SPIFFS.begin(true))
      {
        BLYNK_LOG1(BLYNK_F("SPIFFS failed! Formatting."));
        
        if (!SPIFFS.begin())
        {
          BLYNK_LOG1(BLYNK_F("SPIFFS failed! Pls use EEPROM."));
          return false;
        }
      }

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&BlynkESP32_WM_config, &defaultConfig, sizeof(BlynkESP32_WM_config));
        strcpy(BlynkESP32_WM_config.header, BLYNK_BOARD_TYPE);
        
        // Including config and dynamic data, and assume valid
        saveAllConfigData();
        
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Loaded Config Data ======="));
        displayConfigData(BlynkESP32_WM_config);
#endif

        // Don't need Config Portal anymore
        return true; 
      }
      else if ( ( SPIFFS.exists(CONFIG_FILENAME)      || SPIFFS.exists(CONFIG_FILENAME_BACKUP) ) &&
                ( SPIFFS.exists(CREDENTIALS_FILENAME) || SPIFFS.exists(CREDENTIALS_FILENAME_BACKUP) ) )
      {
        // if config file exists, load
        loadConfigData();
        
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Stored Config Data ======="));
        displayConfigData(BlynkESP32_WM_config);
#endif

        calChecksum = calcChecksum();

        BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                   BLYNK_F(",RCSum=0x"), String(BlynkESP32_WM_config.checkSum, HEX));
                 
        // Load dynamic data
        dynamicDataValid = loadDynamicData();
        
        if (dynamicDataValid)
        {
#if ( BLYNK_WM_DEBUG > 2)      
          BLYNK_LOG1(BLYNK_F("Valid Stored Dynamic Data"));
#endif          
        }
#if ( BLYNK_WM_DEBUG > 2)  
        else
        {
          BLYNK_LOG1(BLYNK_F("Invalid Stored Dynamic Data. Ignored"));
        }
#endif
      }
      else    
      {
        // Not loading Default config data, but having no config file => Config Portal
        return false;
      }    

      if ( (strncmp(BlynkESP32_WM_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkESP32_WM_config.checkSum) || !dynamicDataValid )
                      
      {         
        // Including Credentials CSum
        BLYNK_LOG2(BLYNK_F("InitCfgFile,sz="), sizeof(BlynkESP32_WM_config));

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&BlynkESP32_WM_config, &defaultConfig, sizeof(BlynkESP32_WM_config));
        }
        else
        {
          memset(&BlynkESP32_WM_config, 0, sizeof(BlynkESP32_WM_config));     
              
          strcpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid,       NO_CONFIG);
          strcpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw,         NO_CONFIG);
          strcpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid,       NO_CONFIG);
          strcpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw,         NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server,   NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token,    NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server,   NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token,    NO_CONFIG);
          BlynkESP32_WM_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;      
          strcpy(BlynkESP32_WM_config.board_name,       NO_CONFIG);
          
          for (int i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, NO_CONFIG, myMenuItems[i].maxlen);
          }
        }
    
        strcpy(BlynkESP32_WM_config.header, BLYNK_BOARD_TYPE);
        
        #if ( BLYNK_WM_DEBUG > 2)
        for (int i = 0; i < NUM_MENU_ITEMS; i++)
        {
          BLYNK_LOG4(BLYNK_F("g:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
        }
        #endif
        
        // Don't need
        BlynkESP32_WM_config.checkSum = 0;

        saveAllConfigData();

        return false;
      }
      else if ( !strncmp(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid,       NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid,       NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server,   NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token,    NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server,   NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token,    NO_CONFIG, strlen(NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="blank", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(BlynkESP32_WM_config);
      }

      return true;
    }

#else

#ifndef EEPROM_SIZE
#define EEPROM_SIZE     2048
#else
#if (EEPROM_SIZE > 2048)
#warning EEPROM_SIZE must be <= 2048. Reset to 2048
#undef EEPROM_SIZE
#define EEPROM_SIZE     2048
#endif
// FLAG_DATA_SIZE is 4, to store DRD flag
#if (EEPROM_SIZE < FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
#warning EEPROM_SIZE must be > CONFIG_DATA_SIZE. Reset to 512
#undef EEPROM_SIZE
#define EEPROM_SIZE     2048
#endif
#endif

#ifndef EEPROM_START
#define EEPROM_START     0      //define 256 in DRD
#else
#if (EEPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE > EEPROM_SIZE)
#error EPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE > EEPROM_SIZE. Please adjust.
#endif
#endif

// Stating positon to store BlynkESP32_WM_config
#define BLYNK_EEPROM_START    (EEPROM_START + FLAG_DATA_SIZE)

    bool checkDynamicData(void)
    {
      int checkSum = 0;
      int readCheckSum;
      
      #define BUFFER_LEN      128
      char readBuffer[BUFFER_LEN + 1];
      
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkESP32_WM_config);
                
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > BUFFER_LEN)
        {
          // Size too large, abort and flag false
          BLYNK_LOG1(BLYNK_F("ChkCrR: Error Small Buffer."));
          return false;
        }
      }
         
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;
        
        // Prepare buffer, more than enough
        memset(readBuffer, 0, sizeof(readBuffer));
        
        // Read more than necessary, but OK and easier to code
        EEPROM.get(offset, readBuffer);
        // NULL terminated
        readBuffer[myMenuItems[i].maxlen] = 0;

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }   
        
        offset += myMenuItems[i].maxlen;    
      }

      EEPROM.get(offset, readCheckSum);
           
      BLYNK_LOG4(F("ChkCrR:CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
           
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    bool EEPROM_getDynamicData(void)
    {
      int readCheckSum;
      int checkSum = 0;
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkESP32_WM_config);
           
      totalDataSize = sizeof(BlynkESP32_WM_config) + sizeof(readCheckSum);
      
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;
        
        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          *_pointer = EEPROM.read(offset);
          
          checkSum += *_pointer;  
         }    
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif             
      }
      
      EEPROM.get(offset, readCheckSum);
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;
    }

    void EEPROM_putDynamicData(void)
    {
      int checkSum = 0;
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkESP32_WM_config);
                
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CW:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
                            
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          EEPROM.write(offset, *_pointer);
          
          checkSum += *_pointer;     
         }
      }
      
      EEPROM.put(offset, checkSum);
      //EEPROM.commit();
      
      BLYNK_LOG2(F("CrWCSum=0x"), String(checkSum, HEX));
    }
    
    bool getConfigData()
    {
      bool dynamicDataValid;
      int calChecksum;
      
      hadConfigData = false; 
      
      EEPROM.begin(EEPROM_SIZE);
      BLYNK_LOG2(BLYNK_F("EEPROMsz:"), EEPROM_SIZE);
      
      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&BlynkESP32_WM_config, &defaultConfig, sizeof(BlynkESP32_WM_config));
        strcpy(BlynkESP32_WM_config.header, BLYNK_BOARD_TYPE);
        
        // Including config and dynamic data, and assume valid
        saveAllConfigData();
                 
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Loaded Config Data ======="));
        displayConfigData(BlynkESP32_WM_config);
#endif

        // Don't need Config Portal anymore
        return true;             
      }
      else
      {
        // Load data from EEPROM
        EEPROM.get(BLYNK_EEPROM_START, BlynkESP32_WM_config);
        
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Stored Config Data ======="));
        displayConfigData(BlynkESP32_WM_config);
#endif

        calChecksum = calcChecksum();

        BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                   BLYNK_F(",RCSum=0x"), String(BlynkESP32_WM_config.checkSum, HEX));
                 
        // Load dynamic data from EEPROM
        dynamicDataValid = EEPROM_getDynamicData();
        
        if (dynamicDataValid)
        {
#if ( BLYNK_WM_DEBUG > 2)      
          BLYNK_LOG1(BLYNK_F("Valid Stored Dynamic Data"));
#endif          
        }
#if ( BLYNK_WM_DEBUG > 2)  
        else
        {
          BLYNK_LOG1(BLYNK_F("Invalid Stored Dynamic Data. Ignored"));
        }
#endif
      }
        
      if ( (strncmp(BlynkESP32_WM_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkESP32_WM_config.checkSum) || !dynamicDataValid )
      {       
        // Including Credentials CSum
        BLYNK_LOG4(F("InitEEPROM,sz="), EEPROM_SIZE, F(",DataSz="), totalDataSize);

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&BlynkESP32_WM_config, &defaultConfig, sizeof(BlynkESP32_WM_config));
        }
        else
        {
          memset(&BlynkESP32_WM_config, 0, sizeof(BlynkESP32_WM_config));
             
          strcpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid,       NO_CONFIG);
          strcpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw,         NO_CONFIG);
          strcpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid,       NO_CONFIG);
          strcpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw,         NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server,   NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token,    NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server,   NO_CONFIG);
          strcpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token,    NO_CONFIG);
          BlynkESP32_WM_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;      
          strcpy(BlynkESP32_WM_config.board_name,       NO_CONFIG);
          
          for (int i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, NO_CONFIG, myMenuItems[i].maxlen);
          }
        }
    
        strcpy(BlynkESP32_WM_config.header, BLYNK_BOARD_TYPE);
        
        #if ( BLYNK_WM_DEBUG > 2)    
        for (int i = 0; i < NUM_MENU_ITEMS; i++)
        {
          BLYNK_LOG4(BLYNK_F("g:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
        }
        #endif
        
        // Don't need
        BlynkESP32_WM_config.checkSum = 0;

        saveAllConfigData();

        return false;
      }
      else if ( !strncmp(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid,       NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid,       NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw,         NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server,   NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token,    NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server,   NO_CONFIG, strlen(NO_CONFIG) )  ||
                !strncmp(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token,    NO_CONFIG, strlen(NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(BlynkESP32_WM_config);
      }

      return true;
    }

    void saveConfigData()
    {
      int calChecksum = calcChecksum();
      BlynkESP32_WM_config.checkSum = calChecksum;
      BLYNK_LOG4(BLYNK_F("SaveEEPROM,sz="), EEPROM_SIZE, BLYNK_F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(BLYNK_EEPROM_START, BlynkESP32_WM_config);
      
      EEPROM.commit();
    }
    
    void saveAllConfigData(void)
    {
      int calChecksum = calcChecksum();
      BlynkESP32_WM_config.checkSum = calChecksum;
      BLYNK_LOG4(BLYNK_F("SaveEEPROM,sz="), EEPROM_SIZE, BLYNK_F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(BLYNK_EEPROM_START, BlynkESP32_WM_config);   
      EEPROM_putDynamicData();
      
      EEPROM.commit();
    }

#endif

    bool connectMultiBlynk(void)
    {
#define BLYNK_CONNECT_TIMEOUT_MS      10000L

      for (int i = 0; i < NUM_BLYNK_CREDENTIALS; i++)
      {
        config(BlynkESP32_WM_config.Blynk_Creds[i].blynk_token,
               BlynkESP32_WM_config.Blynk_Creds[i].blynk_server, BLYNK_SERVER_HARDWARE_PORT);

        if (this->connect(BLYNK_CONNECT_TIMEOUT_MS) )
        {
          BLYNK_LOG4(BLYNK_F("Connected to Blynk Server = "), BlynkESP32_WM_config.Blynk_Creds[i].blynk_server,
                     BLYNK_F(", Token = "), BlynkESP32_WM_config.Blynk_Creds[i].blynk_token);
          return true;
        }
      }

      BLYNK_LOG1(BLYNK_F("Blynk not connected"));

      return false;

    }

    uint8_t connectMultiWiFi(void)
    {
      // For ESP32, this better be 2000 to enable connect the 1st time
#define WIFI_MULTI_CONNECT_WAITING_MS      2000L

      uint8_t status;
      BLYNK_LOG1(BLYNK_F("Connecting MultiWifi..."));
      
      WiFi.mode(WIFI_STA);
      
      setHostname();
      
      int i = 0;
      status = wifiMulti.run();
      delay(WIFI_MULTI_CONNECT_WAITING_MS);

      while ( ( i++ < 10 ) && ( status != WL_CONNECTED ) )
      {
        status = wifiMulti.run();

        if ( status == WL_CONNECTED )
          break;
        else
          delay(WIFI_MULTI_CONNECT_WAITING_MS);
      }

      if ( status == WL_CONNECTED )
      {
        BLYNK_LOG2(BLYNK_F("WiFi connected after time: "), i);
        BLYNK_LOG4(BLYNK_F("SSID: "), WiFi.SSID(), BLYNK_F(", RSSI = "), WiFi.RSSI());
        BLYNK_LOG4(BLYNK_F("Channel: "), WiFi.channel(), BLYNK_F(", IP address: "), WiFi.localIP() );
      }
      else
        BLYNK_LOG1(BLYNK_F("WiFi not connected"));

      return status;
    }
    
    // NEW
    void createHTML(String &root_html_template)
    {
      String pitem;
      
      root_html_template = String(BLYNK_WM_HTML_HEAD)  + BLYNK_WM_FLDSET_START;
      
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = String(BLYNK_WM_HTML_PARAM);

        pitem.replace("{b}", myMenuItems[i].displayName);
        pitem.replace("{v}", myMenuItems[i].id);
        pitem.replace("{i}", myMenuItems[i].id);
        
        root_html_template += pitem;
      }
      
      root_html_template += String(BLYNK_WM_FLDSET_END) + BLYNK_WM_HTML_BUTTON + BLYNK_WM_HTML_SCRIPT;     
      
      for (int i = 0; i < NUM_MENU_ITEMS; i++)
      {
        pitem = String(BLYNK_WM_HTML_SCRIPT_ITEM);
        
        pitem.replace("{d}", myMenuItems[i].id);
        
        root_html_template += pitem;
      }
      
      root_html_template += String(BLYNK_WM_HTML_SCRIPT_END) + BLYNK_WM_HTML_END;
      
      return;     
    }
    ////

    void handleRequest(AsyncWebServerRequest *request)
    {
      if (request)
      {
        String key = request->arg("key");
        String value = request->arg("value");

        static int number_items_Updated = 0;

        if (key == "" && value == "")
        {
          String result;
          createHTML(result);

          //BLYNK_LOG1(BLYNK_F("hR: replace result"));

          // Reset configTimeout to stay here until finished.
          configTimeout = 0;
          
          if ( RFC952_hostname[0] != 0 )
          {
            // Replace only if Hostname is valid
            result.replace("BlynkSimpleEsp32_SSL_Async_WM", RFC952_hostname);
          }
          else if ( BlynkESP32_WM_config.board_name[0] != 0 )
          {
            // Or replace only if board_name is valid.  Otherwise, keep intact
            result.replace("BlynkSimpleEsp32_SSL_Async_WM", BlynkESP32_WM_config.board_name);
          }
          
          result.replace("[[id]]",     BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid);
          result.replace("[[pw]]",     BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw);
          result.replace("[[id1]]",    BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid);
          result.replace("[[pw1]]",    BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw);
          result.replace("[[sv]]",     BlynkESP32_WM_config.Blynk_Creds[0].blynk_server);
          result.replace("[[tk]]",     BlynkESP32_WM_config.Blynk_Creds[0].blynk_token);
          result.replace("[[sv1]]",    BlynkESP32_WM_config.Blynk_Creds[1].blynk_server);
          result.replace("[[tk1]]",    BlynkESP32_WM_config.Blynk_Creds[1].blynk_token);
          result.replace("[[pt]]",     String(BlynkESP32_WM_config.blynk_port));
          result.replace("[[nm]]",     BlynkESP32_WM_config.board_name);
          
          // Load default configuration        
          for (int i = 0; i < NUM_MENU_ITEMS; i++)
          {
            String toChange = String("[[") + myMenuItems[i].id + "]]";
            result.replace(toChange, myMenuItems[i].pdata);
#if ( BLYNK_WM_DEBUG > 2)                 
            BLYNK_LOG4(BLYNK_F("h1:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata )
#endif            
          }

          request->send(200, "text/html", result);

          return;
        }

        if (number_items_Updated == 0)
        {
          memset(&BlynkESP32_WM_config, 0, sizeof(BlynkESP32_WM_config));
          strcpy(BlynkESP32_WM_config.header, BLYNK_BOARD_TYPE);
        }

        if (key == "id")
        {
          BLYNK_LOG2(BLYNK_F("id: = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid) - 1)
            strcpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid, value.c_str(), sizeof(BlynkESP32_WM_config.WiFi_Creds[0].wifi_ssid) - 1);
        }
        else if (key == "pw")
        {
          BLYNK_LOG2(BLYNK_F("pw = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw) - 1)
            strcpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw, value.c_str(), sizeof(BlynkESP32_WM_config.WiFi_Creds[0].wifi_pw) - 1);
        }

        else if (key == "id1")
        {
          BLYNK_LOG2(BLYNK_F("id1 = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid) - 1)
            strcpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid, value.c_str(), sizeof(BlynkESP32_WM_config.WiFi_Creds[1].wifi_ssid) - 1);
        }
        else if (key == "pw1")
        {
          BLYNK_LOG2(BLYNK_F("pw1 = "), value.c_str());
        
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw) - 1)
            strcpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw, value.c_str(), sizeof(BlynkESP32_WM_config.WiFi_Creds[1].wifi_pw) - 1);
        }
        else if (key == "sv")
        {
          BLYNK_LOG2(BLYNK_F("sv = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server) - 1)
            strcpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server, value.c_str(), sizeof(BlynkESP32_WM_config.Blynk_Creds[0].blynk_server) - 1);
        }
        else if (key == "tk")
        {
          BLYNK_LOG2(BLYNK_F("tk = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token) - 1)
            strcpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token, value.c_str(), sizeof(BlynkESP32_WM_config.Blynk_Creds[0].blynk_token) - 1);
        }
        else if (key == "sv1")
        {
          BLYNK_LOG2(BLYNK_F("sv1 = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server) - 1)
            strcpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server, value.c_str(), sizeof(BlynkESP32_WM_config.Blynk_Creds[1].blynk_server) - 1);
        }
        else if (key == "tk1")
        {
          BLYNK_LOG2(BLYNK_F("tk1 = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token) - 1)
            strcpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token, value.c_str(), sizeof(BlynkESP32_WM_config.Blynk_Creds[1].blynk_token) - 1);
        }
        else if (key == "pt")
        {
          BLYNK_LOG2(BLYNK_F("pt = "), value.toInt());
          
          number_items_Updated++;
          BlynkESP32_WM_config.blynk_port = value.toInt();
        }
        else if (key == "nm")
        {
          BLYNK_LOG2(BLYNK_F("nm = "), value.c_str());
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(BlynkESP32_WM_config.board_name) - 1)
            strcpy(BlynkESP32_WM_config.board_name, value.c_str());
          else
            strncpy(BlynkESP32_WM_config.board_name, value.c_str(), sizeof(BlynkESP32_WM_config.board_name) - 1);
        }

        for (int i = 0; i < NUM_MENU_ITEMS; i++)
        {
          if (key == myMenuItems[i].id)
          {
            BLYNK_LOG4(F("h:"), myMenuItems[i].id, F("="), value.c_str() );
            number_items_Updated++;

            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);

            if ((int) strlen(value.c_str()) < myMenuItems[i].maxlen)
              strcpy(myMenuItems[i].pdata, value.c_str());
            else
              strncpy(myMenuItems[i].pdata, value.c_str(), myMenuItems[i].maxlen);
#if ( BLYNK_WM_DEBUG > 2)                   
            BLYNK_LOG4(BLYNK_F("h2:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
#endif            
          }
        }
        
        request->send(200, "text/html", "OK");

        // NEW
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS + NUM_MENU_ITEMS)
        {
#if USE_SPIFFS
          BLYNK_LOG2(BLYNK_F("h:UpdSPIFFS:"), CONFIG_FILENAME);
#else
          BLYNK_LOG1(BLYNK_F("h:UpdEEPROM"));
#endif

          saveAllConfigData();

          BLYNK_LOG1(BLYNK_F("h:Rst"));

          // Delay then reset the ESP8266 after save data
          delay(1000);
          ESP.restart();
        }
      }    // if (server)
    }

    void startConfigurationMode()
    {
#define CONFIG_TIMEOUT			60000L

      // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, LED_ON);

      if ( (portal_ssid == "") || portal_pass == "" )
      {
        String chipID = String(ESP_getChipId(), HEX);
        chipID.toUpperCase();

        portal_ssid = "ESP_" + chipID;

        portal_pass = "MyESP_" + chipID;
      }

      WiFi.mode(WIFI_AP);
      
      // New
      delay(100);

      static int channel;
      // Use random channel if  WiFiAPChannel == 0
      // Use random channel if  WiFiAPChannel == 0
      if (WiFiAPChannel == 0)
      {
        //channel = random(MAX_WIFI_CHANNEL) + 1;
        channel = (millis() % MAX_WIFI_CHANNEL) + 1;
      }
      else
        channel = WiFiAPChannel;

      WiFi.softAP(portal_ssid.c_str(), portal_pass.c_str(), channel);
      
      BLYNK_LOG4(BLYNK_F("\nstConf:SSID="), portal_ssid, BLYNK_F(",PW="), portal_pass);
      BLYNK_LOG4(BLYNK_F("IP="), portal_apIP.toString(), ",ch=", channel);
      
      delay(100); // ref: https://github.com/espressif/arduino-esp32/issues/985#issuecomment-359157428
      WiFi.softAPConfig(portal_apIP, portal_apIP, IPAddress(255, 255, 255, 0));

      if (!server)
      {
        server = new AsyncWebServer(HTTP_PORT);
      }

      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      if (server)
      {
        server->on("/", HTTP_GET, [this](AsyncWebServerRequest * request)  { handleRequest(request); });        
        server->begin();
      }

      // If there is no saved config Data, stay in config mode forever until having config Data.
      if (hadConfigData)
        configTimeout = millis() + CONFIG_TIMEOUT;
      else
        configTimeout = 0;

      configuration_mode = true;
    }
};

static WiFiClientSecure _blynkWifiClient;
static BlynkArduinoClientSecure<WiFiClientSecure> _blynkTransport(_blynkWifiClient);
BlynkWifi<BlynkArduinoClientSecure<WiFiClientSecure> > Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#endif    // BlynkSimpleEsp32_SSL_Async_WM_h
