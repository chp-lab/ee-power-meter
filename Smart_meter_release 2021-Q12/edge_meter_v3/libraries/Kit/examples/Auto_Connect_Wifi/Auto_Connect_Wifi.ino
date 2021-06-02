#include "Kit.h"
//#include <BlynkSimpleEsp8266.h>

//#define BLYNK_PRINT Serial
#define led_pin 2
Kit k;
//char auth[] = "adpk2qFcWVQSR_cJ18Jenx3FegGUmGla";
void setup() {
  Serial.begin(115200);
  k.v();
  k.ConnectWiFi();
    k.UploadBin(); //For esp32
//  k.UploadOTA("12345"); //For k.UploadOTA("password")
  //  Blynk.config(auth);
  pinMode(led_pin, OUTPUT);

}
void loop() {
  k.HandleWiFi();
//  k.HandleOTA();
  digitalWrite(led_pin, !digitalRead(led_pin));
  delay(100);
  //  Blynk.run();
}
