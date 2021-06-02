/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */


#include "NX2003BTN.h"

// for use by ISR glue routines
NX2003BTN * NX2003BTN::instance0_;

// Constructure
NX2003BTN::NX2003BTN() {
    this->btn_pin = NXDI1;
    this->btn_press = false;
}

void NX2003BTN::init() {
    pinMode(btn_pin, INPUT);
    attachInterrupt(btn_pin, isr0, RISING);
    instance0_ = this;
}

void NX2003BTN::init(byte new_btn) {
    this->btn_pin = new_btn;
    init();
}

int NX2003BTN::read() {
    return digitalRead(btn_pin);
}

// ISR glue routines
void IRAM_ATTR NX2003BTN::isr0() {
    instance0_->handleInterrupt();
}

// class instance to handle an interrupt
void NX2003BTN::handleInterrupt () {
    this->btn_press = true;
    destroy_di();
}

void NX2003BTN::set_di() {
    clear_di();
    delay(255);
    attachInterrupt(btn_pin, isr0, RISING);
}

void NX2003BTN::clear_di() {
    this->btn_press = false;
}

void NX2003BTN::destroy_di() {
    detachInterrupt(btn_pin);
}

bool NX2003BTN::pressed() {
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

