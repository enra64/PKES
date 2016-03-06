#include <Arduino.h>

void setup();
void loop();

//End of Auto generated function prototypes by Atmel Studio
void setup() {
  // put your setup code here, to run once:
  pinMode(14, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  digitalWrite(14, LOW);

  delay(500);
  digitalWrite(14, HIGH);
}