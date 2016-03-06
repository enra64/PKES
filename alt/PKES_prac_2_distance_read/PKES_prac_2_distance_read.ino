#include "arnes_7SD.h"
#include "ADConverter.h"
#include "arnes_library.h"
#include <Wire.h>
#include "imu.h"

//last analog value
uint16_t lav = 0;

/*
const uint16_t CONVERSION_TABLE[] = {
  2600,  8,
  2500,  9,
  2400,  10,
  2300,  11,
  2200,  12,
  
  2100,  13,
  2000,  13,
  1900,  14,
  1800,  14,
  1700,  15,
  
  1600,  16,
  1550,  17,
  1500,  18,
  1450,  19,
  1400,  20,
  
  1350,  21,
  1300,  22,
  1250,  23,
  1200,  24,
  1100,  26,
  
  1050,  28,
  1000,  30,
  950,   32,
  900,   34,
  800,   38,
  
  700,   44,
  600,   55,
  500,   65};*/

const uint16_t CONVERSION_TABLE[]{
  2500,10,
  2270,11,
  2070,12,
  1990,13,
  1870,14,
  1720,15,
  1590,16,
  1520,17,
  1460,18,
  1320,19,
  1230,20,
  1180,21,
  1130,22,
  1060,23,
  1020,24,
  960,25
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("test"); 
  //make 7SD pins output
  DDRC = (1 << SEVEN_SEGMENT_CLOCK) | (1 << SEVEN_SEGMENT_DATA) | (1 << SEVEN_SEGMENT_DATA_ENABLE);
  //set all 7SD pins to off state
  PORTC = 0x00;
  initAdc();

  Serial.println("init ok");
}

int convertMvToCm(uint16_t mv){
  for(int i = 0; i < 56; i+=2){
    if(mv > CONVERSION_TABLE[i])
      return CONVERSION_TABLE[i+1];
  }
  return 999;
}

void loop() {
  lav = readVoltageBlocking(0);

  int cm = convertMvToCm(lav * 5);

  SevenSegmentDisplay::displayNumber(cm);
  
  //Serial.print("test");
  Serial.println(lav);
}
