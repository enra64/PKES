#include "GlidingAverage.h"
#include "ADC.h"
#include <avr/io.h>
#include <avr/interrupt.h>

GlidingAverage _avg0, _avg1;
uint8_t _readNext = 0;

void setup() {
  Serial.begin(57600);
  initAdc(true);
  while (!Serial);
  Serial.println("ok");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(_avg0.readAvg());
  Serial.print('a');
  Serial.print(_avg1.readAvg());
  Serial.print('b');
  Serial.println();
  delay(300);
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
