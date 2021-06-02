#include "NX2003I2C.h"

NX2003I2C::NX2003I2C(void){
	_SDAPIN = DEFAULT_SDA_PIN;
	_SCLPIN = DEFAULT_SCL_PIN;
}

NX2003I2C::NX2003I2C(int sda, int scl){
	_SDAPIN = sda;
	_SCLPIN = scl;
}

void NX2003I2C::begin(void){
	Wire.begin(_SDAPIN, _SCLPIN);
}

int NX2003I2C::scanI2CDevice(void){
	byte address;
  int nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    if (hasDeviceAddress(address)) {
      nDevices++;
    }
  }
	return nDevices;
}

bool NX2003I2C::hasDeviceAddress(int address){
	byte error;
	Wire.beginTransmission(address);
	error = Wire.endTransmission();
	if (error == 0) {
		return true;
	} else {
		return false;
	}
}