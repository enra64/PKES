#include <Arduino.h>
#include "motor.h"
#include "odometry.h"

Motor mMotorControl;
Odometry mOdometry;

void loop() {
  
  Serial.print(mOdometry.getCurrentTicks(0));
  Serial.print(":");
  Serial.print(mOdometry.getCurrentTicks(1));
  mOdometry.ticksPerSecond(0);
  Serial.println();
  delay(50);
}

void setup(){
  Serial.begin(57600);
  mMotorControl.init();
  mOdometry.init();
  mMotorControl.drive(200);
  Serial.println("init ok");
}

/*int main(void)
{
    init();
    setup();
    while (true)
      loop();
    return 0;
}*/

//0/1 is guessed here, simply switch around appropriately
ISR(INT2_vect){
  mOdometry.hit(1);
}

ISR(PCINT1_vect){
  mOdometry.hit(0);
}
