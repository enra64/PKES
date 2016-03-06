#ifndef SSD
#define SSD

//generic includes
#include "General.h"
#include <stdbool.h>//bool
#include <avr/io.h>//uint8_t
#include <util/delay.h>

//seven segment display pins
#define SEVEN_SEGMENT_CLOCK PC2
#define SEVEN_SEGMENT_DATA PC3
#define SEVEN_SEGMENT_DATA_ENABLE PC4

const uint8_t NUMBERS_7SD[] = {
	0b11111100,
	0b01100000,
	0b11011010,
	0b11110010,
	0b01100110,
	0b10110110,
	0b10111110,
	0b11100000,
	0b11111110,
	0b11110110,
0b00000010};

/*
* Writes a single data bit to the seven segment display
*/
void ssdSendBit(bool val){
  //pull data bit
  writeDigital(&PORTC, SEVEN_SEGMENT_DATA, val);
  _delay_us(200);
  //pull clock high, low to read in data bit
  writeDigital(&PORTC, SEVEN_SEGMENT_CLOCK, 1);
  _delay_us(200);
  writeDigital(&PORTC, SEVEN_SEGMENT_CLOCK, 0);
}

/*
 * Send three data bytes to the display while                          
 * complying to the data sheet requirements of the display
 */
void ssdSendTransmission(unsigned char dataBytes[]){
  //enable write
  writeDigital(&PORTC, SEVEN_SEGMENT_DATA_ENABLE, 0);
  _delay_us(1);
  /*
  * BEGIN DATA ARRAY WRITE
  */
  //per data sheet, send a 1 first
  ssdSendBit(1);
  //send all three data bytes
  for(uint8_t digitPosition = 0; digitPosition < 3; digitPosition++){
    //sends a single digit
    for(int i = 7; i >= 0; i--){
      //high or low via bit shifting a mask
      bool val = (dataBytes[digitPosition] & (1 << i)) != 0;
      ssdSendBit(val);
    }
  }
  //send disregarded last 12 bits
  for(int i = 0; i < 12; i++)
    ssdSendBit(0);
  /*
  * END DATA ARRAY WRITE
  */
  //"write to display"
  writeDigital(&PORTC, SEVEN_SEGMENT_DATA_ENABLE, 1);
}
	
/*
 * Displays a number on the seven segment display
 */
void ssdDisplayInt(int val){
  unsigned char data[3];
  if(val > 0){
    data[0] = NUMBERS_7SD[(val / 100) % 10];
    data[1] = NUMBERS_7SD[(val / 10)  % 10];
    data[2] = NUMBERS_7SD[(val / 1)   % 10];
  }
  else if (val > -100) {
    data[0] = NUMBERS_7SD[10];
    data[1] = NUMBERS_7SD[(-val / 10)  % 10];
    data[2] = NUMBERS_7SD[(-val / 1)   % 10];
  }
  else {
    data[0] = 0b01100010;
    data[1] = NUMBERS_7SD[(-val / 10)  % 10];
    data[2] = NUMBERS_7SD[(-val / 1)   % 10];
  }
  ssdSendTransmission(data);
}
/*
 * Displays a number on the seven segment display
 */
void ssdDisplayNumber(double val){
  //get integer part
  uint8_t asInteger = (int) val;
  //where would the comma be
  uint8_t digitsBeforePoint = digitCount(asInteger);
  //fuck it, the comma would be useless anyway
  if(digitsBeforePoint >= 3)
    ssdDisplayInt(asInteger);
  else{
    //create all zero data array
    unsigned char data[3] = {0, 0, 0};
    //load digits
    if(digitsBeforePoint == 2){
      data[0] = NUMBERS_7SD[getDigit(val, 1)];
      data[1] = NUMBERS_7SD[getDigit(val, 0)] + 1;
      data[2] = NUMBERS_7SD[getDigit(val, -1)];
    }
    else if(digitsBeforePoint == 1){
      data[0] = NUMBERS_7SD[getDigit(val, 0)] + 1;
      data[1] = NUMBERS_7SD[getDigit(val, -1)];
      data[2] = NUMBERS_7SD[getDigit(val, -2)];
    }
    else {
      data[0] = NUMBERS_7SD[getDigit(val, -1)] + 1;
      data[1] = NUMBERS_7SD[getDigit(val, -2)] + 1;
      data[2] = NUMBERS_7SD[getDigit(val, -3)] + 1;
    }
    //display comma
    if(digitsBeforePoint > 0)
      data[digitsBeforePoint - 1] |= 1;
    else{
      //make all have commas to show that have 0.xyz
      for(int i = 0; i < 3; i++)
        data[i] |= 1;
    }
    //send
    ssdSendTransmission(data);
  }
}

void ssdInit(){
  //make 7SD pins output
  DDRC = (1 << SEVEN_SEGMENT_CLOCK) | (1 << SEVEN_SEGMENT_DATA) | (1 << SEVEN_SEGMENT_DATA_ENABLE);
  //set all 7SD pins to off state
  PORTC = 0x00;
}
	
#endif /* ARNES_7SD_H_ */
