#include <Arduino.h>
#include "motor.h"
#include "odometry.h"

Motor mMotorControl;
Odometry mOdometry;

void loop() {
  //Serial.print(mOdometry.getCurrentTicks(MOTOR_RIGHT));
  //Serial.print(":");
  //Serial.println(mOdometry.getCurrentTicks(MOTOR_LEFT));
  //tabularOutput(true);
  //showCurrentTicks();
  assignmentCheck();
  delay(20);
}

int state = 0;
int16_t target[] = {20, 180, 20};
uint32_t stateChangeTicks = 0;
uint8_t resavedTicks = 0;

void assignmentCheck(){
  if(state == 0){
    driveForward1();
  }
  else if(state == 1){
    turn();
  }
  else if(state == 2){
    if(!resavedTicks){
      stateChangeTicks = mOdometry.getCurrentTicks(MOTOR_RIGHT);
      Serial.print("sct: ");
      Serial.println(stateChangeTicks);
      resavedTicks = 1;
    }
    driveForward1();
  }
  else{
    mMotorControl.drive(0);
  }
}

void driveForward1(){
  uint32_t currentTicks = mOdometry.getCurrentTicks(MOTOR_RIGHT);
  Serial.print("forward 1");
  mMotorControl.setLeft(mOdometry.calculatePwmTarget(MOTOR_LEFT));
  mMotorControl.setRight(mOdometry.calculatePwmTarget(MOTOR_RIGHT));
  Serial.print("c_ticks: ");
  Serial.print(currentTicks);
  Serial.print(" st_ticks: ");
  Serial.println(stateChangeTicks);
  
  if((currentTicks - stateChangeTicks) >= getTicksForDrive(target[state])){
    mMotorControl.setRight(-150);
    mMotorControl.setLeft(-400);
    delay(30);
    mMotorControl.drive(0);
    state++;
    _delay_ms(2000);
    stateChangeTicks = currentTicks;
  }
}

void turn(){
  uint32_t currentTicks = mOdometry.getCurrentTicks(MOTOR_RIGHT);
  mMotorControl.setRight(300);
  Serial.print("rot ");  
  Serial.print("c_ticks: ");
  Serial.print(currentTicks);
  Serial.print(" st_ticks: ");
  Serial.println(stateChangeTicks);
  
  if((currentTicks - stateChangeTicks) > getTicksForTurn(target[state])){
    state++;
    mMotorControl.setRight(-150);
    mMotorControl.setLeft(-400);
    delay(30);
    mMotorControl.drive(0);
    _delay_ms(2000);
    mOdometry.reset();
    stateChangeTicks = currentTicks;
    //mOdometry.ticksPerSecond(MOTOR_LEFT);
    //mOdometry.ticksPerSecond(MOTOR_RIGHT);
  }
}

int16_t getTicksForTurn(int16_t degree){
  return (float) degree * 0.58;
}

int16_t getTicksForDrive(int16_t cm){
  return ((float) cm) * 2;//2.13
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

void getCommands(){
  Serial.println("give commands: forward\\nangle\\nforward\\n");
  while(!Serial.available());
  target[0] = Serial.parseInt();
  while(!Serial.available());
  target[1] = Serial.parseInt();
  while(!Serial.available());
  target[2] = Serial.parseInt();
}
//burns

void setup(){
  Serial.begin(57600);
  mMotorControl.init();
  mOdometry.init();
  //getCommands();
  //mMotorControl.drive(500);
  mOdometry.setTarget(MOTOR_BOTH, 100);
  Serial.println("init ok");
}

//0/1 is guessed here, simply switch around appropriately
ISR(INT2_vect){
  mOdometry.hit(MOTOR_RIGHT);
}

ISR(PCINT1_vect){
  mOdometry.hit(MOTOR_LEFT);
}
