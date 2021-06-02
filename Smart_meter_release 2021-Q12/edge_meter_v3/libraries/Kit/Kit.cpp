//
// Created by Teerachot on 28/10/2563.
//

#include "Kit.h"
#include "Arduino.h"
#include <time.h>
#include <EEPROM.h>

#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer Server;
#elif defined(ARDUINO_ARCH_ESP32)
WebServer Server;
#endif

char val_read[35] ;
AutoConnect       Portal(Server);
AutoConnectConfig Config;       // Enable autoReconnect supported on v0.9.4
AutoConnectAux    Timezone;

static const char AUX_TIMEZONE[] PROGMEM = R"(
{
  "title": "TimeZone",
  "uri": "/timezone",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "Sets the time zone to get the current local time.",
      "style": "font-family:Arial;font-weight:bold;text-align:center;margin-bottom:10px;color:DarkSlateBlue"
    },
    {
      "name": "timezone",
      "type": "ACSelect",
      "label": "Select TZ name",
      "option": [],
      "selected": 10
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<br>"
    },
    {
      "name": "start",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/start"
    }
  ]
}
)";

typedef struct {
    const char* zone;
    const char* ntpServer;
    int8_t      tzoff;
} Timezone_t;

static const Timezone_t TZ[] = {
        { "Europe/London", "europe.pool.ntp.org", 0 },
        { "Europe/Berlin", "europe.pool.ntp.org", 1 },
        { "Europe/Helsinki", "europe.pool.ntp.org", 2 },
        { "Europe/Moscow", "europe.pool.ntp.org", 3 },
        { "Asia/Dubai", "asia.pool.ntp.org", 4 },
        { "Asia/Karachi", "asia.pool.ntp.org", 5 },
        { "Asia/Dhaka", "asia.pool.ntp.org", 6 },
        { "Asia/Jakarta", "asia.pool.ntp.org", 7 },
        { "Asia/Manila", "asia.pool.ntp.org", 8 },
        { "Asia/Tokyo", "asia.pool.ntp.org", 9 },
        { "Australia/Brisbane", "oceania.pool.ntp.org", 10 },
        { "Pacific/Noumea", "oceania.pool.ntp.org", 11 },
        { "Pacific/Auckland", "oceania.pool.ntp.org", 12 },
        { "Atlantic/Azores", "europe.pool.ntp.org", -1 },
        { "America/Noronha", "south-america.pool.ntp.org", -2 },
        { "America/Araguaina", "south-america.pool.ntp.org", -3 },
        { "America/Blanc-Sablon", "north-america.pool.ntp.org", -4},
        { "America/New_York", "north-america.pool.ntp.org", -5 },
        { "America/Chicago", "north-america.pool.ntp.org", -6 },
        { "America/Denver", "north-america.pool.ntp.org", -7 },
        { "America/Los_Angeles", "north-america.pool.ntp.org", -8 },
        { "America/Anchorage", "north-america.pool.ntp.org", -9 },
        { "Pacific/Honolulu", "north-america.pool.ntp.org", -10 },
        { "Pacific/Samoa", "oceania.pool.ntp.org", -11 }
};

void rootPage(){
    String  content =
            "<html>"
            "<head>"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
            "<script type=\"text/javascript\">"
            "setTimeout(\"location.reload()\", 1000);"
            "</script>"
            "</head>"
            "<body>"
            "<h1 align=\"center\" style=\"color:#6633FF;margin:20px;\">Welcome To Setting</h1>"
            "<h3 align=\"center\" style=\"color:gray;margin:10px;\">{{DateTime}}</h3>"
            "<p style=\"text-align:center;\">Reload the page to update the time.</p>"
    "<p></p><p style=\"padding-top:15px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
           "</body>"
           "</html>";
    static const char *wd[7] = { "Sun","Mon","Tue","Wed","Thr","Fri","Sat" };
    struct tm *tm;
    time_t  t;
    char    dateTime[26];

    t = time(NULL);
    tm = localtime(&t);
    sprintf(dateTime, "%04d/%02d/%02d(%s) %02d:%02d:%02d.",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            wd[tm->tm_wday],
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    content.replace("{{DateTime}}", String(dateTime));
    Server.send(200, "text/html", content);
}

void startPage() {
    String tz = Server.arg("timezone");
    for (uint8_t n = 0; n < sizeof(TZ) / sizeof(Timezone_t); n++) {
        String tzName = String(TZ[n].zone);
        if (tz.equalsIgnoreCase(tzName)) {
            configTime(TZ[n].tzoff * 3600, 0, TZ[n].ntpServer);
            Serial.println("Time zone: " + tz);
            Serial.println("ntp server: " + String(TZ[n].ntpServer));
            break;
        }
    }
}

/* Style */
String style =
        "<style>"
//            "#file-input,input{"
//                "width:100%;"
//                "height:44px;"
//                "border-radius:4px;"
//                "margin:10px auto;"
//                "font-size:15px"
//            "}"
//            "input{"
//                "background:#f1f1f1;"
//                "border:0;"
//                "padding:0 15px"
//            "}"
//            "body{"
//                "background:#3498db;"
//                "font-family:sans-serif;"
//                "font-size:14px;"
//                "color:#777"
//            "}"
//            "#file-input{"
//                "padding:0;"
//                "border:1px solid #ddd;"
//                "line-height:44px;"
//                "text-align:left;"
//                "display:block;"
//                "cursor:pointer"
//            "}"
            "#bar,#prgbar{"
                "background-color:#f1f1f1;"
                "border-radius:10px"
            "}"
            "#bar{"
//                "background-color:#3498db;"
                "width:0%;"
                "height:10px"
            "}"
//        "form{"
//            "background:#fff;"
//            "max-width:258px;"
//            "margin:75px auto;"
//            "padding:30px;"
//            "border-radius:5px;"
//            "text-align:center"
//        "}"
//        ".btn{"
//            "background:#3498db;"
//            "color:#fff;cursor:pointer"
//        "}"
    "</style>";

void login_uploadbin(){
    /* Login page */
    String loginIndex =
            "<form name=loginForm>"
                "<h1>ESP32 Login</h1>"
                    "<input name=userid placeholder='User ID'> "
                    "<input name=pwd placeholder=Password type=Password> "
                    "<input type=submit onclick=check(this.form) class=btn value=Login>"
            "</form>"
            "<script>"
                "function check(form) {"
                    "if(form.userid.value=='admin' && form.pwd.value=='admin'){"
                            "window.open('/serverIndex')"
                    "}else{"
                        "alert('Error Password or Username')"
                    "}"
                "}"
            "</script>" + style;
    Server.sendHeader("Connection", "close");
    Server.send(200, "text/html", loginIndex);
}

void uploadbin(){
    /* Server Index Page */
    String serverIndex =
            "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"

            "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
                "<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
                    "<label id='file-input' for='file'>   "
                        "Choose file..."
                    "</label>"
                    "<input type='submit' class=btn value='Update'> <br><br> "
                    "<div id='prg'></div><br>"
                    "<div id='prgbar'>"
                        "<div id='bar'></div>"
                    "</div><br>"
            "</form>"

            "<script>"
                "function sub(obj){"
                    "var fileName = obj.value.split('\\\\');"
                        "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
                "};"

                "$('form').submit(function(e){"
                    "e.preventDefault();"
                    "var form = $('#upload_form')[0];"
                    "var data = new FormData(form);"
                    "$.ajax({"
                        "url: '/update',"
                        "type: 'POST',"
                        "data: data,"
                        "contentType: false,"
                        "processData:false,"
                        "xhr: function() {"
                            "var xhr = new window.XMLHttpRequest();"
                            "xhr.upload.addEventListener('progress', function(evt) {"
                                "if (evt.lengthComputable) {"
                                    "var per = evt.loaded / evt.total;"
                                    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
                                        "var value = Math.round(per*100);"
                                            "$('#bar').css('width',value + '%');"

                                        "if (value <= 15) {"
                                            "$('#bar').css({'background-color':'#FF0000'});"
                                        "} else if(value <= 30) {"
                                            "$('#bar').css({'background-color':'#FF0033'});"
                                        "} else if(value <= 45) {"
                                            "$('#bar').css({'background-color':'#FF0066'});"
                                        "} else if(value <= 60) {"
                                            "$('#bar').css({'background-color':'#FF0099'});"
                                        "} else if(value <= 75) {"
                                            "$('#bar').css({'background-color':'#FF00CC'});"
                                        "} else if(value <= 100) {"
                                            "$('#bar').css({'background-color':'#FF00FF'});"
                                        "}"
                                "}"
                            "}, false);"
                            "return xhr;"
                        "},"
                        "success:function(d, s) {"
                            "console.log('success!') "
                        "},"
                        "error: function (a, b, c) {}"
                    "});"
                "});"
            "</script>" + style;
    Server.sendHeader("Connection", "close");
    Server.send(200, "text/html", serverIndex);
}
void Kit::ConnectWiFi() {
    Config.autoReconnect = true;
    Portal.config(Config);
    Timezone.load(AUX_TIMEZONE);
    AutoConnectSelect&  tz = Timezone["timezone"].as<AutoConnectSelect>();
    for (uint8_t n = 0; n < sizeof(TZ) / sizeof(Timezone_t); n++) {
        tz.add(String(TZ[n].zone));
    }
    Portal.join({ Timezone });
    Server.on("/", rootPage);
    Server.on("/start", startPage);

    if (Portal.begin()) {
        Serial.println("WiFi connected: " + WiFi.localIP().toString());
    }
}

void Kit::UploadBin() {
    Server.on("/uploadbin", login_uploadbin);
    Server.on("/serverIndex", uploadbin);
    Server.on("/update", HTTP_POST, []() {
        Server.sendHeader("Connection", "close");
        Server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        HTTPUpload& upload = Server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            /* flashing firmware to ESP*/
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) { //true to set the size to the current progress
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        }
    });
}

void Kit::UploadOTA(const char *passwordOta) {
    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
     ArduinoOTA.setHostname("MyOTA_Upload");

    // No authentication by default
     ArduinoOTA.setPassword(passwordOta);

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
            .onStart([]() {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH)
                    type = "sketch";
                else // U_SPIFFS
                    type = "filesystem";

                // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                Serial.println("Start updating " + type);
            })
            .onEnd([]() {
                Serial.println("\nEnd");
            })
            .onProgress([](unsigned int progress, unsigned int total) {
                Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
            })
            .onError([](ota_error_t error) {
                Serial.printf("Error[%u]: ", error);
                if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                else if (error == OTA_END_ERROR) Serial.println("End Failed");
            });
    ArduinoOTA.begin();
}
void Kit::HandleOTA() {
    ArduinoOTA.handle();
}

void Kit::HandleWiFi(){
    Portal.handleClient();
}

void Kit::v() {
    delay(500);
    Serial.println("\nLib Version 1.0");
}