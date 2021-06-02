/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */

#include "NX2003SPI.h"

SPIClass spiSD(HSPI);

NX2003SPI::NX2003SPI() {
    this->sck = NX_SCK;
    this->miso = NX_MISO;
    this->mosi = NX_MOSI;
    this->cs = NX_CS;
}

void NX2003SPI::init() {
    spiSD.begin(sck, miso, mosi, cs);
}

void NX2003SPI::init(byte cs)
{
    this->cs = cs;
    init();
}

bool NX2003SPI::mount() {
    return SD.begin(cs, spiSD);
}
