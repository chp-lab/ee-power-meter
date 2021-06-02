#include "NX2003Thermistor.h"

// constructor
NX2003Thermistor::NX2003Thermistor(){
	setPin(NX2003_THERMISTOR_PIN);
	setVoltageInput(NX2003_VOLTAGE_INPUT);
	setReferenceResistance(NX2003_REFERENCE_RESISTANCE);
	setNorminalThermistorResistance(NX2003_NOMINAL_THERMISTOR_RESISTANCE);
	setNorminalTemperature(NX2003_NOMINAL_TEMP);
	setBValue(NX2003_B_VALUE);
	setADCResolution(NX2003_ADC_RESOLUTION);
}

// constructor with parameter
NX2003Thermistor::NX2003Thermistor(
	int pin,
	double voltageInput,
	double referenceResistance,
	double nominalThermistorResistance,
	double nominalTemperature,
	double bValue,
	int resolution
) {
	setPin(pin);
	setVoltageInput(voltageInput);
	setReferenceResistance(referenceResistance);
	setNorminalThermistorResistance(nominalThermistorResistance);
	setNorminalTemperature(nominalTemperature);
	setBValue(bValue);
	setADCResolution(resolution);
}

void NX2003Thermistor::setPin(int pin){
	this->_thermistorPin = pin;
	pinMode(_thermistorPin, INPUT);
}

void NX2003Thermistor::setVoltageInput(double voltageInput){
	this->_voltageInput = voltageInput;
}

void NX2003Thermistor::setReferenceResistance(double resistance){
	this->_referenceResistance = resistance;
}

void NX2003Thermistor::setNorminalThermistorResistance(double resistance){
	this->_nominalThermistorResistance = resistance;
}

void NX2003Thermistor::setNorminalTemperature(double temperature) {
	this->_nominalTemperature = temperature + 273.15;
}

void NX2003Thermistor::setADCResolution(int resolution){
	this->_adcResolution = resolution;
}

void NX2003Thermistor::setBValue(double bValue){
	this->_bValue = bValue;
}

void NX2003Thermistor::setCalibrateValue(double differentTemp, bool adding){
	this->_adding_calibrate = adding;
	this->_TEMPERATURE_DIFFERENT_VALUE = differentTemp;
}

double NX2003Thermistor::readVoltage(){
	double voltageOutput = 0;
	double thermistorVoltage = 0;
	for(int i = 0; i<NUM_SAMPLING; i++){
		thermistorVoltage = thermistorVoltage + analogRead(this->_thermistorPin);
	}
	thermistorVoltage = thermistorVoltage/NUM_SAMPLING;
	voltageOutput = this->_voltageInput * thermistorVoltage / this->_adcResolution;
	return voltageOutput;
}

double NX2003Thermistor::readThermistorResistance(){
	return this->_referenceResistance/ ((this->_voltageInput / readVoltage()) - 1);
}

double NX2003Thermistor::readTemperatureCelsius(){
	double celsius_temp;
	if (this->_adding_calibrate){
		celsius_temp = (1/(log(readThermistorResistance()/this->_nominalThermistorResistance)/this->_bValue + (1/this->_nominalTemperature))) - 273.15;
		celsius_temp = celsius_temp + _TEMPERATURE_DIFFERENT_VALUE;
	}else{
		celsius_temp = (1/(log(readThermistorResistance()/this->_nominalThermistorResistance)/this->_bValue + (1/this->_nominalTemperature))) - 273.15;
		celsius_temp = celsius_temp - _TEMPERATURE_DIFFERENT_VALUE;
	}
	return celsius_temp;
}

double NX2003Thermistor::readTemperatureFahrenheit(){
	double fahrenheit_temp = (readTemperatureCelsius() * 1.8) + 32;
	return fahrenheit_temp;
}