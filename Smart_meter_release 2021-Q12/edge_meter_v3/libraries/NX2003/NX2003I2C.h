#ifndef NX2003I2C_H
#define NX2003I2C_H

#include "Arduino.h"
#include "Wire.h"

#if defined(NX2003_H) || defined(NX2003I2C_H)
	#define DEFAULT_SDA_PIN 4
	#define DEFAULT_SCL_PIN 22
#endif

class NX2003I2C {
	private:
		int _SDAPIN;
		int _SCLPIN;
		
	public:
		NX2003I2C(void);
		NX2003I2C(int sda, int scl);
		void begin(void);
		int scanI2CDevice(void);
		bool hasDeviceAddress(int address);
};

#endif