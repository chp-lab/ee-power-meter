#include <IOXhop_DotMatrix.h>

#define MAX_DEVICES 8  // number of MAX7219 module

#if defined(ARDUINO_ARCH_ESP32) // if use ESP32 base
#define DIN_PIN     23 // DIN pin
#define CLK_PIN     18 // CLK pin
#define CS_PIN      5  // CS pin

IOXhop_DotMatrix dot(MAX_DEVICES, CS_PIN, DIN_PIN, CLK_PIN);
#else // Other (eg. Arduino Uno, Arduino Nano)
#define CS_PIN      13  // CS pin

IOXhop_DotMatrix dot(MAX_DEVICES, CS_PIN);
#endif

void setup() {
  dot.begin();
}

void loop() {
  dot.scrollInBottom("IOXhop.com", 50);
  delay(300);
  dot.invertColor();
  delay(300);
  dot.invertColor();
  delay(300);
  dot.invertColor();
  delay(300);
  dot.scrollOutLeft("IOXhop.com", 50);
  
  dot.scrollInTop("Electronic", 50);
  delay(1000);

  dot.showTextCenter("Component");
  delay(1000);

  dot.showTextCenter("ESP32");
  delay(1000);

  dot.showTextCenter("Arduino");
  delay(1000);

  dot.scrollInRight("K210 STM32", 50);
  delay(2000);
  dot.scrollOutBottom("K210 STM32", 50);

  dot.scrollLeftToRight("ATSAMD21", 50, 1000);
  dot.scrollRightToLeft("AIoT , IoT, MCU", 50);
}
