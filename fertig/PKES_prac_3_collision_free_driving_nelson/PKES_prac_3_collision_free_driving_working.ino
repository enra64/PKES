#include "GlidingAverage.h"
#include "Motor.h"
#include "ADC.h"
#include "General.h"
#include "SSD.h"
#include <avr/io.h>
#include <avr/interrupt.h>

GlidingAverage _avg0, _avg1;
uint8_t _readNext = 0;

#define SPEED 300
#define MAGIC_DISTANCE 6

void setup() {
  Serial.begin(57600);
  initAdc(true);
  initMotors();
  ssdInit();
  while (!Serial);
  transition(SPEED, 10);
  Serial.println("ok");  
}

void loop() {
  int dstR = interpretDistanceSensor(_avg0.readAvg(), 49);
  int dstL = interpretDistanceSensor(_avg1.readAvg(), 49);
  ssdDisplayInt(dstR);
  if(dstR < MAGIC_DISTANCE || dstL < MAGIC_DISTANCE){
    brakeReward();
    if(dstR > dstL)
      turnRight();
    else
      turnLeft();
    brake();
    drive(SPEED);
  }
}

void turnLeft(){
  transition(0, MOTOR_RIGHT, 3);//turn left
  _delay_ms(600);//wait for turn
  Serial.println("left");
}

void turnRight(){
  transition(0, MOTOR_LEFT, 3);//turn left
  _delay_ms(600);//wait for turn
  Serial.println("right");
}

void brakeReward(){
    brake();
    transition(-SPEED, 5);//reward
    _delay_ms(300);//wait  
}

ISR(ADC_vect){
  if(_readNext == 0){
    _avg0.write(ADC);
    _readNext = 1;
    setAdmux(_readNext);
  }
  else{
    _avg1.write(ADC);
    _readNext = 0;
    setAdmux(_readNext);
  }
}
