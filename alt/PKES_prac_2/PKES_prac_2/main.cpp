/*
 * PKES_prac_2.cpp
 *
 * Created: 15-Oct-15 17:31:31
 * Author : Arne
 */

#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
//cant break
#include "arnes_library.h"
#include "arnes_7SD.h"
#include "arnes_serial.h"
#include "arnes_twi.h"
#include "ADConverter.h"

void assignYBalken(int y, unsigned char data[]){
	uint8_t yBalkenOben = 0b10000000;
	uint8_t yBalkenMitte = 0b00000010;
	uint8_t yBalkenUnten = 0b00010000;

	uint8_t balken;
	
	if(y < -500)
		balken =  yBalkenOben;
	else if(y > 500)
		balken =  yBalkenMitte;
	else
		balken = yBalkenUnten;

	
	for(int i = 0; i < 3; i++)
		data[i] |= balken; 
}

void assignXBalken(int x, unsigned char data[]){
	uint8_t xBalkenLinks = 0b01100000;
	uint8_t xBalkenRechts = 0b00001100;
	
	int xRange[6][2] = {
		{-10000, -400},
		{-400, -200},
		{-200, 0},
		{0, 200},
		{200, 400},
		{400, 10000},
	};
		
	int xResult = 0;
	for(int i = 0; i < 6; i++){
		if(xRange[i][0] < x && x < xRange[i][1])
			xResult = i;
	}
		
	switch(xResult){
		case 0:
			data[0] |= xBalkenLinks;
		break;
		case 1:
			data[0] |= xBalkenRechts;
		break;
		case 2:
			data[1] |= xBalkenLinks;
		break;
		case 3:
			data[1] |= xBalkenRechts;
		break;
		case 4:
			data[2] |= xBalkenLinks;
		break;
		default:
			data[2] |= xBalkenRechts;
	}
}

void assignImuData(imu_data val){
	unsigned char data[3];
	
	assignXBalken(val.x, data);
	assignYBalken(val.y, data);
	
	SevenSegmentDisplay::send7SDTransmission(data);
}

int main(void)
{
	//make 7SD pins output
	DDRC = (1 << SEVEN_SEGMENT_CLOCK) | (1 << SEVEN_SEGMENT_DATA) | (1 << SEVEN_SEGMENT_DATA_ENABLE);
	
	//set all to off state
	PORTC = 0x00;

	//initialize serial, config is 57600:8N1
	Serial::serialInit();
	//enable serial interrupts for buffered processing
	Serial::serialEnableInterrupts(true, true);
	
	/*//testcode for serialDecimal write
	imu_data testData = {};//init all content as zero
	while(1){
		testData.x+=3;
		testData.y+=2;
		testData.z+=3;
		Serial::serialWriteDecimal(testData.x);
		Serial::serialWrite(' ');
		Serial::serialWriteDecimal(testData.y);
		Serial::serialWrite(' ');
		Serial::serialWriteDecimal(testData.z);
		Serial::serialWrite('\n');
		_delay_ms(50);
	}*/
	
	//enable twi
	//Twi::twiInitialize();
	
	//disable sleep bit
	//imuEnable();
	int16_t buffer = 0;
	uint8_t err = 0;
	while(1){
		//err = imuReadByte(MPU9150_WHO_AM_I, &buffer);
		/*err = imuReadInt(&buffer, MPU9150_GYRO_ZOUT_L, MPU9150_GYRO_ZOUT_H);
		Serial::serialWriteDecimal(buffer);
		Serial::serialWrite('s');
		Serial::serialWriteDecimal(err);
		Serial::serialWrite('\n');
		_delay_ms(150);	*/
		
		uint16_t test = ADConverter::readVoltageBlocking(0);
		Serial::serialWriteDecimal(test);
		Serial::serialWrite('\n');
	}
	
	//imu_data c_val = {};
	
	/*while(1){
		Serial::serialWrite('s');
		imuRead(&c_val);
		Serial::serialWriteDecimal(c_val.x);
		//Serial::serialWrite('\t');
		Serial::serialWriteDecimal(c_val.y);
		//Serial::serialWrite('\t');
		Serial::serialWriteDecimal(c_val.z);
		Serial::serialWrite('\n');
		_delay_ms(50);
	}*/
	
	//do not return from main
	while(1){}
}