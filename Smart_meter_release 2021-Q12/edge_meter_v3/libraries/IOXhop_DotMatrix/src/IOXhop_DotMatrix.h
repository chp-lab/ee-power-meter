#ifndef __IOXHOP_DOTMATRIX_H__
#define __IOXHOP_DOTMATRIX_H__

#include "Arduino.h"
#include "SPI.h"

class IOXhop_DotMatrix {
    private:
        int _cs_pin, _dio_pin, _clk_pin;
        int display_number;

        uint8_t *bufferDisplay = NULL;

        void _write(uint8_t cmd, uint8_t data) ;
        int calcTextWidth(String text) ;

    public:
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
        IOXhop_DotMatrix(int max_device=2, int cs=23, int dio=21, int clk=22) ;
//        IOXhop_DotMatrix(int max_device=8, int cs=5, int dio=23, int clk=18) ;
#else
        IOXhop_DotMatrix(int max_device=8, int cs=13) ;
#endif
        void begin() ;

        void shutdown(bool value = false) ;
        void intensity(int level = 15) ;

        void invertColor(bool showOnDisplay = true) ;
        void show() ;
        void clear() ;
        void showText(String text, int top = 0, int left = 0, bool invert = false) ;
        void showTextCenter(String text) ;

        void scrollInLeft(String text, int speed = 50) ;
        void scrollOutLeft(String text, int speed = 50) ;
        void scrollInRight(String text, int speed = 50) ;
        void scrollOutRight(String text, int speed = 50) ;
        void scrollInTop(String text, int speed = 50) ;
        void scrollOutTop(String text, int speed = 50) ;
        void scrollInBottom(String text, int speed = 50) ;
        void scrollOutBottom(String text, int speed = 50) ;
        void scrollLeftToRight(String text, int speed = 50, int pause = 0) ;
        void scrollRightToLeft(String text, int speed = 50, int pause = 0) ;

};

#endif
