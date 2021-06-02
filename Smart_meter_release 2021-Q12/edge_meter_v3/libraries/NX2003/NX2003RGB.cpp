#include "NX2003RGB.h"

// Constructor
NX2003RGB::NX2003RGB(){
	setPin(DEFAULT_R_PIN, DEFAULT_G_PIN, DEFAULT_B_PIN);			// default RGB pin on NX2003Shield
	setCommonCathode(TYPE_COMMON_CATHODE);
}

NX2003RGB::NX2003RGB(byte r_pin, byte g_pin, byte b_pin, bool common_cathode){
	setPin(r_pin, g_pin, b_pin);
	setCommonCathode(common_cathode);
}

// Init method
void NX2003RGB::begin(){
	// setup timer chanel
	ledcSetup(_redChanel, _freq, _resolution);
	ledcSetup(_greenChanel, _freq, _resolution);
	ledcSetup(_blueChanel, _freq, _resolution);

	// attach the channel to the GPIO to be controlled
	ledcAttachPin(_REDPIN, _redChanel);
  ledcAttachPin(_GREENPIN, _greenChanel);
  ledcAttachPin(_BLUEPIN, _blueChanel);
}

void NX2003RGB::setPin(byte r_pin, byte g_pin, byte b_pin){
	_REDPIN = r_pin;
	_GREENPIN = g_pin;
	_BLUEPIN = b_pin;
}

void NX2003RGB::setCommonCathode(bool common_cathode){
	_common_cathode = common_cathode;
}

void NX2003RGB::setColor(String color) {
	if (color.equals("red")) {
		red();
	}else if (color.equals("green")) {
		green();
	}else if (color.equals("blue")) {
		blue();
	}else if (color.equals("pink")) {
		pink();
	}else if (color.equals("yellow")) {
		yellow();
	}else if (color.equals("orange")) {
		orange();
	}else if (color.equals("lightblue")) {
		lightblue();
	}else if (color.equals("lightgreen")) {
		lightgreen();
	}else if (color.equals("white")) {
		white();
	}
}

void NX2003RGB::setColor(byte rgb[3]){
	setColor(rgb[0], rgb[1], rgb[2]);
}

void NX2003RGB::setColor(byte r, byte g, byte b){
	_red = r;
	_green = g;
	_blue = b;
	on();
}

void NX2003RGB::on(){
	if(_common_cathode){
		ledcWrite(_redChanel, _red);
		ledcWrite(_greenChanel, _green);
		ledcWrite(_blueChanel, _blue);
	}else{
		ledcWrite(_redChanel, MAX_BRIGHTNESS - _red);
		ledcWrite(_greenChanel, MAX_BRIGHTNESS - _green);
		ledcWrite(_blueChanel, MAX_BRIGHTNESS - _blue);
	}
}

void NX2003RGB::on(byte r, byte g, byte b){
	if(_common_cathode){
		ledcWrite(_redChanel, r);
		ledcWrite(_greenChanel, g);
		ledcWrite(_blueChanel, b);
	}else{
		ledcWrite(_redChanel, MAX_BRIGHTNESS - r);
		ledcWrite(_greenChanel, MAX_BRIGHTNESS - g);
		ledcWrite(_blueChanel, MAX_BRIGHTNESS - b);
	}
}

void NX2003RGB::off(){
	if(_common_cathode){
		ledcWrite(_redChanel, MIN_BRIGHTNESS);
		ledcWrite(_greenChanel, MIN_BRIGHTNESS);
		ledcWrite(_blueChanel, MIN_BRIGHTNESS);
	}else{
		ledcWrite(_redChanel, MAX_BRIGHTNESS);
		ledcWrite(_greenChanel, MAX_BRIGHTNESS);
		ledcWrite(_blueChanel, MAX_BRIGHTNESS);
	}
}

void NX2003RGB::clear(){
	if (_common_cathode){
		_red = MAX_BRIGHTNESS;
		_green = MAX_BRIGHTNESS;
		_blue = MAX_BRIGHTNESS;
	}else{
		_red  = MIN_BRIGHTNESS;
		_green = MIN_BRIGHTNESS;
		_blue = MIN_BRIGHTNESS;
	}
	on();
}

void NX2003RGB::red(){
	setColor(MAX_BRIGHTNESS, MIN_BRIGHTNESS, MIN_BRIGHTNESS);
}

void NX2003RGB::green(){
	setColor(MIN_BRIGHTNESS, MAX_BRIGHTNESS, MIN_BRIGHTNESS);
}

void NX2003RGB::blue(){
	setColor(MIN_BRIGHTNESS, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
}

void NX2003RGB::yellow(){
	setColor(MAX_BRIGHTNESS, MAX_BRIGHTNESS, MIN_BRIGHTNESS);
}

void NX2003RGB::pink(){
	setColor(MAX_BRIGHTNESS, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
}

void NX2003RGB::lightblue(){
	setColor(MIN_BRIGHTNESS, MAX_BRIGHTNESS/3, MAX_BRIGHTNESS);
}

void NX2003RGB::orange(){
	setColor(MAX_BRIGHTNESS, MAX_BRIGHTNESS/10, MIN_BRIGHTNESS);
}

void NX2003RGB::lightgreen(){
	setColor(MIN_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS/5);
}

void NX2003RGB::white(){
	setColor(MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
}

