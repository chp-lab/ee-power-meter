
#ifndef NX2003_H
#define NX2003_H

#include "NX2003Thermistor.h"
#include "NX2003RGB.h"
#include "NX2003SPI.h"
#include "NX2003I2C.h"
#include "NX2003BTN.h"

#if defined(NX2003_H)
	#define NX_R_PIN 21
	#define NX_G_PIN 19
	#define NX_B_PIN 18
	#define NX_THERMISTOR_PIN 39
	#define NX_BUTTON_PIN 35
	#define NX_SDA_PIN_1 4
	#define NX_SDA_PIN_2 5
	#define NX_SCL_PIN 22
	#define NX_MISO_PIN 23
	#define NX_MOSI_PIN 13
	#define NX_SCK_PIN 14
	#define NX_CS_PIN 27
	#define NX_TX_PIN 1
	#define NX_RX_PIN 3
	#define G1_MAJOR 33
	#define G1_MINOR 32
	#define G2_MAJOR 25
	#define G2_MINOR 33
	#define G3_MAJOR 26
	#define	G3_MINOR 25
#endif

#endif
