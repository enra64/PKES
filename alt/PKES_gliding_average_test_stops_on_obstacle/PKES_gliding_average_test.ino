#include "GlidingAverage.h"
#include "Motor.h"
#include "ADC.h"
#include "General.h"
#include "SSD.h"
#include <avr/io.h>
#include <avr/interrupt.h>

GlidingAverage _avg0, _avg1;
uint8_t _readNext = 0;

void setup() {
  Serial.begin(57600);
  initAdc(true);
  initMotors();
  ssdInit();
  while (!Serial);
  Serial.println("ok");  
}

void loop() {
  int dst0 = interpretDistanceSensor(_avg0.readAvg(), 49);
  int dst1 = interpretDistanceSensor(_avg1.readAvg(), 49);
  ssdDisplayInt(dst0);
  if(dst0 < 6 || dst1 < 6)
    stop();
  else if (dst0 != 999 && dst1 != 999) // invalid! values
    forward(400);
  delay(100);
}

void testDistance(){
    // put your main code here, to run repeatedly:
  Serial.print(_avg0.readAvg());
  Serial.print('a');
  int32_t avg =_avg0.readAvg(); 
  ssdDisplayInt((int16_t) avg);
  Serial.print(avg);
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
