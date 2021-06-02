/*
 * Cr. Chatpeth Kenanan
 * Nexpie Giant team
 * ch.ee.psu@outlook.com
 */

#ifndef NX2003SPI_H
#define NX2003SPI_H

#include <SPI.h>
#include <SD.h>
#include <string.h>

#if defined(NX2003_H) || defined(NX2003SPI_H)
	#define NX_CS 27
	#define NX_SCK 14
	#define NX_MISO 23
	#define NX_MOSI 13
#endif

class NX2003SPI
{
private:
    byte cs, sck, miso, mosi;
public:
    NX2003SPI();
    void init();
    void init(byte cs);
    bool mount();
};
#endif
