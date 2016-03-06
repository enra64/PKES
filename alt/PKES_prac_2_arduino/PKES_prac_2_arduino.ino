#include "arnes_7SD.h"
#include "ADConverter.h"
#include "arnes_library.h"
#include <Wire.h>
#include "imu.h"

//last analog value
uint16_t lav = 0;

const int yRange[6][2] = {
    {-14999, -10000},
    {-9999, -5000},
    {-4999, 0},
    {1, 5000},
    {5001, 10000},
    {10001, 15000}};

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

int convertMvToCm(uint16_t mv){
  for(int i = 0; i < 56; i+=2){
    if(mv > CONVERSION_TABLE[i])
      return CONVERSION_TABLE[i+1];
  }
  return 999;
}

void displayDistance(){
  lav = readVoltageBlocking(0);
  int cm = convertMvToCm(lav * 5);
  SevenSegmentDisplay::displayNumber(cm);
}

void readAccel(imu_data* result){
  result->x = MPU9150_readSensor(MPU9150_ACCEL_XOUT_L,MPU9150_ACCEL_XOUT_H);
  result->y = MPU9150_readSensor(MPU9150_ACCEL_YOUT_L,MPU9150_ACCEL_YOUT_H);
  result->z = MPU9150_readSensor(MPU9150_ACCEL_ZOUT_L,MPU9150_ACCEL_ZOUT_H);
}

void loop() {
  readAccel(&res);
  displayLevel(&res);
  //Serial.println(res.y);
  _delay_ms(200);
}

void displayLevel(imu_data* val){
  unsigned char data[3] = {0,0,0};
  
  assignXBalken(val->x, data);
  assignYBalken(val->y, data);
  
  SevenSegmentDisplay::send7SDTransmission(data);
}

void assignXBalken(int x, unsigned char data[]){
  uint8_t verticalLine = 0;

  x/=7500;

  if(x == 0)
    verticalLine = LINE_MIDDLE;
  else if (x > 0)
    verticalLine = LINE_TOP;
  else
    verticalLine = LINE_BOTTOM;
  
  for(int i = 0; i < 3; i++)
    data[i] |= verticalLine;
}

void assignYBalken(int y, unsigned char data[]){
  int yResult = 0;
  for(int i = 0; i < 6; i++){
    if(yRange[i][0] < y && y < yRange[i][1]){
      yResult = i;
      break;
    }
  }

  //- ist nach rechts, + nach links
  switch(yResult){
    case 0:
      data[2] |= LINE_RIGHT;
      break;
    case 1:
      data[2] |= LINE_LEFT;
      break;
    case 2:
      data[1] |= LINE_RIGHT;
      break;
    case 3:
      data[1] |= LINE_LEFT;
      break;
    case 4:
      data[0] |= LINE_RIGHT;
      break;
    default:
      data[0] |= LINE_LEFT;
  }
}
