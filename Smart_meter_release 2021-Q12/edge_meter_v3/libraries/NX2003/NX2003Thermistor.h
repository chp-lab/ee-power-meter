#ifndef NX2003THERMISTOR_H
#define NX2003THERMISTOR_H
#include <Arduino.h>

#if defined(NX2003_H) || defined(NX2003THERMISTOR_H)
	#define NX2003_THERMISTOR_PIN 39
	#define NX2003_ADC_RESOLUTION 4095
	#define NX2003_VOLTAGE_INPUT 3.30
	#define NX2003_REFERENCE_RESISTANCE 10*1000
	#define NX2003_NOMINAL_THERMISTOR_RESISTANCE 10*1000
	#define NX2003_NOMINAL_TEMP 25
	#define NX2003_B_VALUE 3950
#endif

class NX2003Thermistor {
	private:
		// static const int DEFAULT_ADC_RESOLUTION = 1023;		//default analog resolution for arduino board
		const int NUM_SAMPLING = 10;			// number of sampling
		int _thermistorPin;
		int _adcResolution;
		double _voltageInput;
		double _referenceResistance;
		double _nominalThermistorResistance;
		double _nominalTemperature;
		double _bValue;
		bool _adding_calibrate = false;
		double _TEMPERATURE_DIFFERENT_VALUE = 4.75;
	public:
		NX2003Thermistor();
		NX2003Thermistor(
			int pin,
			double voltageInput,
			double referenceResistance,
			double nominalThermistorResistance,
			double nominalTemperature,
			double bValue,
			int resolution = NX2003_ADC_RESOLUTION
		);
		void setPin(int pin);
		void setVoltageInput(double voltageInput);
		void setReferenceResistance(double resistance);
		void setNorminalThermistorResistance(double resistance);
		void setNorminalTemperature(double temperature);
		void setBValue(double bValue);
		void setADCResolution(int adcResolution);
		void setCalibrateValue(double differentTemp, bool adding=false);
		double readVoltage();
		double readThermistorResistance();
		double readTemperatureCelsius();
		double readTemperatureFahrenheit();
};
#endif