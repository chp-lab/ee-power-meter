# IOXhop_DotMatrix

Very easy use Dot Matrix MAX7219 show text with animation

# Wiring

This library develop for Arduino-ESP32 so this schematic i use for test

![](https://cz.lnwfile.com/_/cz/_raw/ub/o9/2w.png)

# Example

```c++
#include <IOXhop_DotMatrix.h>

#define MAX_DEVICES 8  // number of MAX7219 module

#define DIN_PIN     23 // DIN pin
#define CLK_PIN     18 // CLK pin
#define CS_PIN      5  // CS pin

IOXhop_DotMatrix dot(MAX_DEVICES, CS_PIN, DIN_PIN, CLK_PIN);

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
}
```
