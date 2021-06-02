#include "IoTDavinci.h"

// Constructure
IoTDavinci::IoTDavinci() {
    this->matrix = Adafruit_8x16minimatrix();
}

void IoTDavinci::init()
{
	this->matrix.begin(0x70);
}

void IoTDavinci::scroll_text(String scroll_msg, int scroll_speed)
{
	int msg_len = scroll_msg.length();
  	int gap = 6;
	
	this->matrix.setTextSize(1);
	this->matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
	this->matrix.setTextColor(LED_ON);
	this->matrix.setRotation(1);
  
	for (int8_t x=16; x>=-gap*msg_len; x--) {
		this->matrix.clear();
		this->matrix.setCursor(x,0);
		this->matrix.print(scroll_msg);
		this->matrix.writeDisplay();
		delay(scroll_speed);
	}
  	this->matrix.setRotation(0);
}

void IoTDavinci::show_sticker(const uint8_t sticker[])
{
	this->matrix.setRotation(1);
	this->matrix.clear();
	this->matrix.drawBitmap(0, 0, sticker, 8, 8, LED_ON);
	this->matrix.writeDisplay();
	delay(100);
	this->matrix.setRotation(0);
}

void IoTDavinci::test_matrix()
{
	int i, j;
	int row = 8;
	int col = 16;
	for(i = 0; i < row; i++)
	{
		for(j = 0; j < col; j++)
		{
			this->matrix.clear();
			this->matrix.drawPixel(i, j, LED_ON);  
			this->matrix.writeDisplay();  // write the changes we just made to the display
			delay(50);
		}
	}
}

void IoTDavinci::draw_pixel(int row, int col)
{
	this->matrix.drawPixel(row, col, LED_ON);  
	this->matrix.writeDisplay();  // write the changes we just made to the display
	delay(50);
}

void IoTDavinci::clear_matrix()
{
	this->matrix.clear();
}

void IoTDavinci::scroll_large_text(String scroll_msg, int scroll_speed)
{
	int msg_len = scroll_msg.length();
	int gap = 12;
	
	this->matrix.setTextSize(2);
	this->matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
	this->matrix.setTextColor(LED_ON);
	
	for (int x=7; x>=-gap*msg_len; x--) 
	{
		this->matrix.clear();
		this->matrix.setCursor(x,0);
		this->matrix.print(scroll_msg);
		this->matrix.writeDisplay();
		delay(scroll_speed);
	}
}

float IoTDavinci::readTemperature()
{
	int analog_value = 0;
	Wire1.begin(SDA1, SCL1);
	Wire1.beginTransmission(LM73_ADDR);
	Wire1.write(0x00); // Temperature Data Register
	Wire1.endTransmission();
	
	uint8_t count = Wire1.requestFrom(LM73_ADDR, 2);
	float temp = 0.0;
	if (count == 2) {
		byte buff[2];
		buff[0] = Wire1.read();
		buff[1] = Wire1.read();
		temp += (int)(buff[0]<<1);
		if (buff[1]&0b10000000) temp += 1.0;
		if (buff[1]&0b01000000) temp += 0.5;
		if (buff[1]&0b00100000) temp += 0.25;
		if (buff[0]&0b10000000) temp *= -1.0;
	}
	
	return temp;
}
