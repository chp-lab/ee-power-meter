/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */


#ifndef NX2003BTN_H
#define NX2003BTN_H

#if defined(NX2003_H) || defined(NX2003BTN_H)
	#define NXDI1 35    //default DI in NX2003 board
#endif

#include <Arduino.h>

class NX2003BTN {
    private:
        static NX2003BTN * instance0_;
        byte btn_pin;
        void handleInterrupt ();
        static void isr0 ();
        bool btn_press;
    public:
        NX2003BTN();
        void init();
        void init(byte new_btn);
        int read();
        bool pressed();
        void set_di();
        void clear_di();
        void destroy_di();
};

#endif

