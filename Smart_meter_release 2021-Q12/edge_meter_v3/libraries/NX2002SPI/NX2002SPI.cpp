/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */

#include "NX2002SPI.h"

SPIClass spiSD(HSPI);

NX2002SPI::NX2002SPI() {
    this->sck = NX_SCK;
    this->miso = NX_MISO;
    this->mosi = NX_MOSI;
    this->cs = NX_CS;
}

void NX2002SPI::init() {
    spiSD.begin(sck, miso, mosi, cs);
}

void NX2002SPI::init(byte cs)
{
    this->cs = cs;
    init();
}

bool NX2002SPI::mount() {
    return SD.begin(cs, spiSD);
}

