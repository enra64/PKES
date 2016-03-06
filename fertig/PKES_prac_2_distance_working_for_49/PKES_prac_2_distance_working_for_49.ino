#include "arnes_7SD.h"
#include "ADConverter.h"
#include "arnes_library.h"
#include <Wire.h>
#include "imu.h"

//last analog value
uint16_t lav = 0;

const uint16_t CONV_TBL_47[]{
  587,3,
  550,4,
  451,5,
  388,6,
  338,7,
  305,8,
  273,9,
  244,10,
  225,11,
  209,12,
  195,13,
  184,14,
  172,15,
  160,16,
  152,17,
  144,18,
  140,19,
  137,20,
  129,21,
  124,22,
  120,23,
  116,24,
  110,25};

const uint16_t CONV_TBL_39[] = {
  572,3,
  536,4,
  455,5,
  394,6,
  340,7,
  302,8,
  270,9,
  248,10,
  227,11,
  209,12,
  194,13,
  180,14,
  175,15,
  165,16,
  156,17,
  148,18,
  143,19,
  137,20,
  125,21,
  117,22,
  108,23,
  100,24,
  95,25
};

const uint16_t CONVERSION_TABLE[] = {//nelson ...49
  600,3,
  500,4,
  444,5,
  380,6,
  335,7,
  300,8,
  275,9,
  250,10,
  240,11,
  218,12,
  205,13,
  190,14,
  180,15,
  172,16,
  163,17,
  156,18,
  150,19,
  143,20,
  142,21,
  141,22,
  140,23,
  135,24,
  130,25};

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
  for(int i = 0; i < 46; i+=2){
    if(mv > CONVERSION_TABLE[i])
      return CONVERSION_TABLE[i+1];
  }
  return 999;
}

void loop() {
  lav = readVoltageBlocking(0);

  int cm = convertMvToCm(lav);

  SevenSegmentDisplay::displayInt(cm);

  _delay_ms(200);
  acos(1);
  Serial.println(lav);
}
