/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */

#ifndef NX2002SPI_H
#define NX2002SPI_H

#include <SPI.h>
#include <SD.h>
#include <string.h>

#define NX_CS 15
#define NX_SCK 14
#define NX_MISO 26
#define NX_MOSI 13

class NX2002SPI
{
private:
    byte cs, sck, miso, mosi;
public:
    NX2002SPI();
    void init();
    void init(byte cs);
    bool mount();
};
#endif
