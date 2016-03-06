/*
 * PKES_prac_1_atmel_1.cpp
 *
 * Created: 15-Oct-15 17:31:31
 * Author : Arne
 */

#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
//cant break
#include "arnes_library.h"

unsigned char data[3];

unsigned char hello[] = {7, 5, 9, 9, 11};
	
void saveWord(int wordData[], int offset);
void count();
void word();

//data2 is rightmost, data 1 is middle, data 0 is left
void saveWord(unsigned char wordData[], int offset){
	int wordLength = sizeof(wordData) / sizeof(wordData[0]);
	//begin at first 7SD position, user should start with offset 0
	offset -= 2;
	const int right = 2, middle = 1, left = 0;
	if(offset < (wordLength - 2))
		data[right] = SEVEN_SEGMENT_ALPHABET[wordData[offset + 2]];
	else
		data[right] = 0x00;
		
	if(offset >= -1 && offset < (wordLength - 1))
		data[middle] = SEVEN_SEGMENT_ALPHABET[wordData[offset + 1]];
	else
		data[middle] = 0x00;
	
	if(offset >= 0 && offset < wordLength)
		data[left] = SEVEN_SEGMENT_ALPHABET[wordData[offset]];
	else
		data[left] = 0x00;
}

void displayFixedPointNumber(fixed_point_number fixedPointNumber){
	data[0] = SEVEN_SEGMENT_NUMBERS[(fixedPointNumber.number / 100) % 10];
	data[1] = SEVEN_SEGMENT_NUMBERS[(fixedPointNumber.number / 10)  % 10];
	data[2] = SEVEN_SEGMENT_NUMBERS[(fixedPointNumber.number / 1)   % 10];
	//comma is last segment
	data[fixedPointNumber.commaPos] |= 1;
	send7SDTransmission(data);
}

void saveNumber(int counterPosition){
	if(counterPosition > 0){
		data[0] = SEVEN_SEGMENT_NUMBERS[(counterPosition / 100) % 10];
		data[1] = SEVEN_SEGMENT_NUMBERS[(counterPosition / 10)  % 10];
		data[2] = SEVEN_SEGMENT_NUMBERS[(counterPosition / 1)   % 10];
	}
	else if (counterPosition > -100) {
		data[0] = SEVEN_SEGMENT_NUMBERS[10];
		data[1] = SEVEN_SEGMENT_NUMBERS[(-counterPosition / 10)  % 10];
		data[2] = SEVEN_SEGMENT_NUMBERS[(-counterPosition / 1)   % 10];
	}
	else {
		data[0] = 0b01100010;
		data[1] = SEVEN_SEGMENT_NUMBERS[(-counterPosition / 10)  % 10];
		data[2] = SEVEN_SEGMENT_NUMBERS[(-counterPosition / 1)   % 10];
	}
	send7SDTransmission(data);
}

void count(){
	int counterPosition = 100, delta = -1;
	
	while (1)
	{
		saveNumber(counterPosition);
		if(counterPosition == 999)
			delta = -1;
		else if (counterPosition == -199)
			delta = 1;
		counterPosition += delta;
		_delay_ms(20);
	}
}

void countWithSerial(){
	int counterPosition = 100, delta = -1;
	unsigned char input;
	bool isFloat = false;
	
	while (1)
	{
		if(isSerialDataAvailable_0()){
			input = readSerial_0();
			switch(input){
				case 'f':
				case 'F':
					isFloat = true;
					writeSerialString_0("float\0");
					break;
				case 'c':
				case 'C':
					isFloat = false;
					writeSerialString_0("count\0");
					break;
				default:
					writeSerialString_0("default\0");
			}
		}
		if(isFloat == false)
			saveNumber(counterPosition);
		else{
			//make positive
			bool changedCounter = counterPosition < 0;
			counterPosition = counterPosition < 0 ? -counterPosition : counterPosition;
			uint8_t comPos = counterPosition / 100 > 2 ? 1 : counterPosition / 100;
			fixed_point_number r = { .number = counterPosition, .commaPos = comPos };
			if(changedCounter)
				counterPosition = -counterPosition;
			displayFixedPointNumber(r);
		}
		if(counterPosition == 999)
			delta = -1;
		else if (counterPosition == -199)
			delta = 1;
		counterPosition += delta;
		_delay_ms(20);
	}
}

void word(){
	while (1)
	{
		for(int i = 0; i < 8; i ++){
			saveWord(hello, i);
			send7SDTransmission(data);
			_delay_ms(400);
		}
	}
}

void testSerial(){
	char test = 'A';
	
	bool isAlive = false;
	
	//test serial communication
	while(1){
		if(isSerialDataSent_0()){
			writeSerial_0(test++);
			
			writeDigital(&PORTA, LED_2, isAlive);
			isAlive = !isAlive;
			//writeDigital(&PORTA, LED_3, true);
		}
		else{
			//writeDigital(&PORTA, LED_2, false);
			writeDigital(&PORTA, LED_3, true);
		}
		_delay_ms(50);
	}
}

void testSerialRead(){
	int counter = 0;
	bool enableDbgLed = false;
	
	while(1){
		if(isSerialDataAvailable_0()){
			unsigned char input = readSerial_0();
			switch (input)
			{
				case 'f':
				case 'F':{
					fixed_point_number r = { .number = 420, .commaPos = 1 };
					displayFixedPointNumber(r);
					writeSerialString_0("display fixed point number\n\0");
					break;
					}
				case 'c':
				case 'C':{
					saveNumber(counter++);
					writeSerialString_0("counter increased\n\0");
					}
					break;
				case '\0':
					//this basically just says "nothing in rx buffer"
					writeSerialString_0("empty rx buffer\n\0");
					break;
				default:
					writeSerialString_0("unrecognized command\n\0");
			}
		}
		else{
			writeDigital(&PORTA, LED_2, enableDbgLed);
			enableDbgLed = !enableDbgLed;
		}
		_delay_ms(50);
	}
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
	
	//initialize data array
	data[0] = SEVEN_SEGMENT_NUMBERS[0];
	data[1] = SEVEN_SEGMENT_NUMBERS[1];
	data[2] = SEVEN_SEGMENT_NUMBERS[2];
	
	//initialize serial, config is 57600:8N1
	initSerial_0();
	
	//why do i need this??
	hello[1] = 5;
	
	//so self-documenting code
	enableSerialInterrupts_0(true, true);
	
	//writeSerial_0("test", 4);
	//writeSerialDecimal_0(1234);
	
	//testSerial();
	
	//word();
	
	countWithSerial();
	
	count();
	
	testSerialRead();
		
	//do not return from main
	while(1){}
}