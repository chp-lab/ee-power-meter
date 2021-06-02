#ifndef NX2003RGB_H
#define NX2003RGB_H
#include "Arduino.h"

#if defined(NX2003_H) || defined(NX2003RGB_H)
	#define DEFAULT_R_PIN 21
	#define DEFAULT_G_PIN 19
	#define DEFAULT_B_PIN 18
	#define TYPE_COMMON_CATHODE true
	#define MAX_BRIGHTNESS 255
	#define MIN_BRIGHTNESS 0
#endif

class NX2003RGB {
	private:
		byte _REDPIN;
		byte _GREENPIN;
		byte _BLUEPIN;
		bool _common_cathode = TYPE_COMMON_CATHODE;
		const int _redChanel = 0;
		const int _greenChanel = 1;
		const int _blueChanel = 2;
		const int _freq = 5000;
		const int _resolution = 8; // 0 - 255
		byte _red = 255;
		byte _green = 255;
		byte _blue = 255;
	public:
		NX2003RGB();
		NX2003RGB(byte r_pin, byte g_pin, byte b_pin, bool common_cathode=true);
		void begin();
		void setPin(byte r_pin, byte g_pin, byte b_pin);
		void setCommonCathode(bool common_cathode);
		void setColor(String color);
		void setColor(byte rgb[3]);
		void setColor(byte r, byte g, byte b);
		void on();
		void on(byte r, byte g, byte b);
		void off();
		void red();
		void green();
		void blue();
		void yellow();
		void pink();
		void lightblue();
		void white();
		void orange();
		void lightgreen();
		void clear(); // clear r,g,b value to default (255)
};
#endif