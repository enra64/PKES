#include "arnes_7SD.h"
#include "ADConverter.h"
#include "arnes_library.h"
#include <Wire.h>
#include "imu.h"

//last analog value
uint16_t lav = 0;

//level stuff
uint8_t xBalkenOben = 0b10000000;
uint8_t xBalkenMitte = 0b00000010;
uint8_t xBalkenUnten = 0b00010000;

uint8_t yBalkenRechts = 0b01100000;
uint8_t yBalkenLinks = 0b00001100;

//buffer for imu result set
imu_data res;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("test"); 
  //make 7SD pins output
  DDRC = (1 << SEVEN_SEGMENT_CLOCK) | (1 << SEVEN_SEGMENT_DATA) | (1 << SEVEN_SEGMENT_DATA_ENABLE);
  //set all 7SD pins to off state
  PORTC = 0x00;
  initAdc();

  Wire.begin();
  
  // Clear the 'sleep' bit to start the sensor.
  MPU9150_writeSensor(MPU9150_PWR_MGMT_1, 0);
  MPU9150_setupCompass();

  Serial.println("init ok");
}

void readAccel(imu_data* result){
  int x = MPU9150_readSensor(MPU9150_ACCEL_XOUT_L,MPU9150_ACCEL_XOUT_H);
  int y = MPU9150_readSensor(MPU9150_ACCEL_YOUT_L,MPU9150_ACCEL_YOUT_H);
  int z = MPU9150_readSensor(MPU9150_ACCEL_ZOUT_L,MPU9150_ACCEL_ZOUT_H);
  result->x = x;
  result->y = y;
  result->z = z;
}

void loop() {
  readAccel(&res);
  /*Serial.print(res.x);
  Serial.print("\t");
    Serial.print(res.y);
  Serial.print("\t");
    Serial.print(res.z);
  Serial.print("\n");*/
  displayLevel(&res);
  //Serial.println(res.y);
  //_delay_ms(200);
}

void displayLevel(imu_data* val){
  unsigned char data[3] = {0,0,0};
  
  assignXBalken(val->x, data);
  assignYBalken(val->y, data);
  
  SevenSegmentDisplay::send7SDTransmission(data);
}

void assignXBalken(int x, unsigned char data[]){
  uint8_t balken;
  
  if(-7500 < x && x < 7500)
    balken = xBalkenMitte;
  else if(x < -7500)
    balken =  xBalkenUnten;
  else
    balken =  xBalkenOben;
  
  for(int i = 0; i < 3; i++)
    data[i] |= balken;
}

void assignYBalken(int y, unsigned char data[]){
  int yRange[6][2] = {
    {-32000, -10000},
    {-9999, -5000},
    {-4999, 0},
    {0, 5000},
    {5001, 10000},
    {10001, 32000}};
    
  int yResult = 0;
  for(int i = 0; i < 6; i++){
    if(yRange[i][0] < y && y < yRange[i][1])
      yResult = i;
  }

  //- ist nach rechts, + nach links
  switch(yResult){
    case 0:
      data[2] |= yBalkenRechts;
    break;
    case 1:
      data[2] |= yBalkenLinks;
    break;
    case 2:
      data[1] |= yBalkenRechts;
    break;
    case 3:
      data[1] |= yBalkenLinks;
    break;
    case 4:
      data[0] |= yBalkenRechts;
    break;
    default:
      data[0] |= yBalkenLinks;
  }
}
