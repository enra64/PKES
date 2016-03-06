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

void setup() {
  Serial.begin(57600);
  _delay_ms(20);
  Serial.println("alive");
  //initAdc(true);
  //Serial.println("adc init ok");
  //initMotors();
  //Serial.println("motor init ok");
  initImu(true);
  Serial.println("imu init ok");
  initImuTimer();
  Serial.println("ctc timer init ok");
  //ssdInit();
  //Serial.println("ssd init ok");
  initIntegrator();
  Serial.println("integrator offset ok");
  //start motor
  //transition(1023, MOTOR_LEFT, 1);
  //setMotor(1023, FOR_1);
  //Serial.println("transition ok");
}

//inject data into integrator via isr + loop, and wait until we can create our no-movement-reference for the adc offset
void initIntegrator(){
  while(!_gyro.createReference())
    checkGyro(false);
}

void loop() {
  checkGyro(true);
  Serial.println(_gyro.getAngle());
}

void checkGyro(bool ssdAngle){
  if(_readGyro){
    _gyro.readAndProcessGyro();
    _readGyro = false;
    if(ssdAngle)
      ssdDisplayInt(_gyro.getAngle());
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
