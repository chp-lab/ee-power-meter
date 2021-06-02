/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */


#ifndef NX2002BTN_H
#define NX2002BTN_H
#define NXDI1 35

#include <Arduino.h>

class NX2002BTN {
    private:
        static NX2002BTN * instance0_;
        byte btn_pin;
        void handleInterrupt ();
        static void isr0 ();
        bool btn_press;
    public:
        NX2002BTN();
        void init();
        void init(byte new_btn);
        int read();
        bool pressed();
        void set_di();
        void clear_di();
        void destroy_di();
};
#endif

