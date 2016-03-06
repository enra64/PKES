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
	
	send7SDTransmission(data);
}

int main(void)
{
    //make led pins output
	writeDigital(&DDRA, LED_2, true);
	writeDigital(&DDRA, LED_3, true);
	
	//make 7SD pins output
	DDRC = (1 << SEVEN_SEGMENT_CLOCK) | (1 << SEVEN_SEGMENT_DATA) | (1 << SEVEN_SEGMENT_DATA_ENABLE);
	
	//set all to off state
	PORTC = 0x00;
	PORTA = 0x00;

	//initialize serial, config is 57600:8N1
	serialInit_0();
	
	//enable serial interrupts for buffered processing
	serialEnableInterrupts_0(true, true);
	//testcode for serialDecimal write
	imu_data testData;
	testData.x = 0;
	testData.y = 0;
	testData.z = 0;
	while(1){
		testData.x++;
		testData.y+=2;
		testData.z+=3;
		serialWriteUint16_0(testData.x);
		//writeSerial_0('\t');
		//writeSerialDecimal_0(testData.y);
		//writeSerial_0('\t');
		//writeSerialDecimal_0(testData.z);
		serialWrite_0('\n');
		_delay_ms(200);
	}
	
	//enable twi
	twiInitialize();
	
	//disable sleep bit
	imuEnable();
	
	while(1){
		serialWrite_0('s');
		imu_data c_val;
		imuRead(&c_val);
		serialWriteUint16_0(c_val.x);
		serialWrite_0('\t');
		serialWriteUint16_0(c_val.y);
		serialWrite_0('\t');
		serialWriteUint16_0(c_val.z);
		serialWrite_0('\n');
		_delay_ms(400);
	}
	
	//do not return from main
	while(1){}
}