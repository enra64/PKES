#include <Arduino.h>
#include "motor.h"
#include "odometry.h"

Motor mMotorControl;
Odometry mOdometry;

uint8_t outputFlag = true;

void loop() {
  //Serial.print(mOdometry.getCurrentTicks(MOTOR_RIGHT));
  //Serial.print(":");
  //Serial.println(mOdometry.getCurrentTicks(MOTOR_LEFT));
  tabularOutput(true);
  //showCurrentTicks();
  delay(5);
}

void tabularOutput(uint8_t output){
  int16_t lPwmSet = mOdometry.calculatePwmTarget(MOTOR_LEFT, output);
  int16_t rPwmSet = mOdometry.calculatePwmTarget(MOTOR_RIGHT, output);
  mMotorControl.setLeft(lPwmSet);
  mMotorControl.setRight(rPwmSet);
  if(!output)
    return;//istTicks is put out before @ calculatePwmTarget
  Serial.print(lPwmSet);
  Serial.print('\t');
  Serial.print(rPwmSet);
  Serial.print('\n');
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
