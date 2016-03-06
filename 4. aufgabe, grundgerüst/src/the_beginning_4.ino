#include <Arduino.h>
#include "motor.h"
#include "odometry.h"

Motor mMotorControl;
Odometry mOdometry;

void loop() {
  Serial.print(mOdometry.getPwmTarget(0));
}

void setup(){
  mMotorControl.init();
  mOdometry.init();
  mOdometry.setTarget(2, 0);
}

int main(void)
{
    init();
    setup();
    while (true)
      loop();
    return 0;
}

//0/1 is guessed here, simply switch around appropriately
ISR(INT2_vect){
  mOdometry.hit(1);
}

ISR(PCINT1_vect){
  mOdometry.hit(0);
}