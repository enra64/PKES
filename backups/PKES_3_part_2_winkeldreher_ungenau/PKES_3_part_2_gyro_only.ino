#define IMU_OCR_VALUE 750 //current filter has 2.8 ms delay, so all 3 ms should be the best value
//250->1kHz|1ms, so 500->500Hz|2ms
#define IMU_TIMER_MS_DELTA_PER_TICK (IMU_OCR_VALUE / 250)

#include "GlidingAverage.h"
#include "Motor.h"
#include "Integrator.h"
#include "ADC.h"
#include "General.h"
#include "SSD.h"
#include "Imu.h"
#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

Integrator _gyro;
GlidingAverage _avg0, _avg1;
uint8_t _readNext = 0;
bool _readGyro = false;

uint16_t targetAngle = 0;

void setup() {
  Serial.begin(57600);
  _delay_ms(20);
  Serial.println("alive");
  initAdc(true);
  Serial.println("adc init ok");
  initMotors();
  Serial.println("motor init ok");
  initImu(true);
  Serial.println("imu init ok");
  initImuTimer();
  Serial.println("ctc timer init ok");
  ssdInit();
  Serial.println("ssd init ok");
  initIntegrator();
  Serial.println("integrator offset ok");
  requestTargetAngle();
}

void requestTargetAngle(){
  //Serial.println("angle?");
  while(!Serial.available());
  targetAngle = Serial.parseInt();
  //Serial.print("got angle: ");
  //Serial.println(targetAngle);
  transition(300, MOTOR_RIGHT, 2);
}

//inject data into integrator via isr + loop, and wait until we can create our no-movement-reference for the adc offset
void initIntegrator(){
  while(!_gyro.createReference())
    checkGyro();
}

void loop() {
  checkGyro(true, true);
  if(abs(_gyro.getAngle()) > abs(targetAngle)){
    Serial.print(abs(_gyro.getAngle()));
    Serial.print(":");
    Serial.println(abs(targetAngle));
    brake();
    _gyro.reset();
    initIntegrator();
    requestTargetAngle();
  }
}

void checkGyro(){
  checkGyro(false, false);
}

void checkGyro(bool ssdDebug, bool serialDebug){
  if(_readGyro){
    _gyro.readAndProcessGyro();
    _readGyro = false;
    if(ssdDebug)
      ssdDisplayInt(_gyro.getAngle());
    if(serialDebug)
      Serial.println(_gyro.getAngle());
  }
}

void initImuTimer(){
  //disable interrupts for config
  cli();
  //normal waveform generation mode, no output enabled
  TCCR3A = 0;
  //no input capture stuff, no wg bits, cs off
  TCCR3B = 0;
  //prescaler 64 (CS32 only for 256, cs31 only for 8)
  TCCR3B |= (1 << CS31) | (1 << CS30);
  //enable ctc mode
  TCCR3B |= (1 << WGM32);
  //set required value for clear on compare, 250 should be correct for 64 prescaler, since it gives 250k ticks/second -> 1kHz
  OCR3A = IMU_OCR_VALUE;
  //reset counter
  TCNT3 = 0;
  //enable overflow interrupt
  TIMSK3 |= (1 << OCIE3A);
  //reenable interrupts
  sei();
}

ISR(TIMER3_COMPA_vect)
{
  //set trigger for reading gyro, takes too long for an isr
  _readGyro = true;
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
