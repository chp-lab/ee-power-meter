/*
 * Cr. Chatpeth Kenanan
 * INET Giant team
 * ch.ee.psu@outlook.com
 */


#ifndef IOTDAVINCI_H
#define IOTDAVINCI_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define SW1 16
#define SW2 14
#define SENSOR_VP 36
#define LDR SENSOR_VP
#define SDA0 21
#define SCL0 22
#define SDA1 4
#define SCL1 5
#define LM73_ADDR 0x4D
#define IOT_LED 12
#define NTP_LED 15
#define WIFI_LED 2
#define BT_LED 23
#define USB_SW 25
#define IN1 32
#define IN2 33
#define IN3 34
#define IN4 35
#define OUT1 26
#define OUT2 27

class IoTDavinci {
    private:
        Adafruit_8x16minimatrix matrix;
    public:
    	IoTDavinci();
    	void init();
		void scroll_text(String scroll_msg, int scroll_speed);
		void show_sticker(const uint8_t sticker[]);
		void test_matrix();
		void draw_pixel(int row, int col);
		void clear_matrix();
		void scroll_large_text(String scroll_msg, int scroll_speed);
		float readTemperature();
};
#endif



