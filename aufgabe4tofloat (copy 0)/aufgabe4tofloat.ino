#include <Arduino.h>
#include "motor.h"
#include "odometry.h"

Motor mMotorControl;
Odometry mOdometry;

void loop() {
  //Serial.print(mOdometry.getCurrentTicks(MOTOR_RIGHT));
  //Serial.print(":");
  //Serial.println(mOdometry.getCurrentTicks(MOTOR_LEFT));
  tabularOutput(true);
  //showCurrentTicks();
  delay(20);
}
void showCurrentTicks(){
  Serial.print(mOdometry.getCurrentTicks(MOTOR_LEFT));
  Serial.print(':');
  Serial.println(mOdometry.getCurrentTicks(MOTOR_RIGHT));
  if(mOdometry.getCurrentTicks(MOTOR_LEFT) == 32)
    mMotorControl.drive(0);
}

void tabularOutput(uint8_t output){
  int16_t lSollTicks = mOdometry.getTarget(MOTOR_LEFT);
  int16_t rSollTicks = mOdometry.getTarget(MOTOR_RIGHT);
  int16_t lPwmSet = mOdometry.calculatePwmTarget(MOTOR_LEFT);
  int16_t rPwmSet = mOdometry.calculatePwmTarget(MOTOR_RIGHT);
  mMotorControl.setLeft(lPwmSet);
  mMotorControl.setRight(rPwmSet);
  if(!output)
    return;//istTicks is put out before @ calculatePwmTarget
  Serial.print(lSollTicks);
  Serial.print('\t');
  Serial.print(rSollTicks);
  Serial.print('\t');
  Serial.print(lPwmSet);
  Serial.print('\t');
  Serial.print(rPwmSet);
  Serial.println();
}

void setup(){
  Serial.begin(57600);
  mMotorControl.init();
  mOdometry.init();
  //mMotorControl.drive(500);
  mOdometry.setTarget(MOTOR_BOTH, 80);
  Serial.println("init ok");
}

//0/1 is guessed here, simply switch around appropriately
ISR(INT2_vect){
  mOdometry.hit(MOTOR_RIGHT);
}

ISR(PCINT1_vect){
  mOdometry.hit(MOTOR_LEFT);
}
