#include "GlidingAverage.h"
#include <avr/io.h>

GlidingAverage _avg;

void setup() {
  Serial.begin(57600);
  while (!Serial);
  Serial.println("ok");
}

void loop() {
  // put your main code here, to run repeatedly:
  int16_t buf = analogRead(0);
  _avg.write(buf);
  Serial.print(_avg.readAvg());
  Serial.print(" ");
  Serial.print(buf);
  Serial.println();
  //Serial.println(buf);
  delay(300);
}
