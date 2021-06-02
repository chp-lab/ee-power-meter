/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */


#include "NX2002BTN.h"

// for use by ISR glue routines
NX2002BTN * NX2002BTN::instance0_;

// Constructure
NX2002BTN::NX2002BTN() {
    this->btn_pin = NXDI1;
    this->btn_press = false;
}

void NX2002BTN::init() {
    pinMode(btn_pin, INPUT);
    attachInterrupt(btn_pin, isr0, RISING);
    instance0_ = this;
}

void NX2002BTN::init(byte new_btn) {
    this->btn_pin = new_btn;
    init();
}

int NX2002BTN::read() {
    return digitalRead(btn_pin);
}

// ISR glue routines
void IRAM_ATTR NX2002BTN::isr0() {
    instance0_->handleInterrupt();
}

// class instance to handle an interrupt
void NX2002BTN::handleInterrupt () {
    this->btn_press = true;
    destroy_di();
}

void NX2002BTN::set_di() {
    clear_di();
    delay(255);
    attachInterrupt(btn_pin, isr0, RISING);
}

void NX2002BTN::clear_di() {
    this->btn_press = false;
}

void NX2002BTN::destroy_di() {
    detachInterrupt(btn_pin);
}

bool NX2002BTN::pressed() {
    if(btn_press)
    {
        set_di();
        return true;
    }
    else
    {
        return false;
    }
}

