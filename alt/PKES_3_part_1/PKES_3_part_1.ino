#include "arnes_7SD.h"
#include "arnes_library.h"
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "imu.h"
#include "Motor.h"
#include "ADConverter.h"
#include "gyro.h"

//floating average class used for casting a floating average over the sensor values to kill spikes
FloatingAverage pin1Average(5), pin2Average(5);
Gyro _gyro;
//keep track of last read adc pin to enable switching in isr
uint8_t lastRead = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  //_delay_ms(200);
  //Serial.println("test");
  //make 7SD pins output
  DDRC = (1 << SEVEN_SEGMENT_CLOCK) | (1 << SEVEN_SEGMENT_DATA) | (1 << SEVEN_SEGMENT_DATA_ENABLE);
  //set all 7SD pins to off state
  PORTC = 0x00;
  _gyro = Gyro();
  setupImuTimer();
  //initAdc(true);//free running mode activated
  initMotors();

  //max is 1023, e.g. 10 bit
  setMotor(796, FOR_2);
  setMotor(0, FOR_1);

  _delay_ms(1000);

  setMotor(0, FOR_2);
  setMotor(0, FOR_1);

  //just read a single voltage from adc, that will trigger the adc isr
  readVoltageInterrupt(0);

  Serial.println("init ok");
}

void loop() {
  _delay_ms(20);
  
  SevenSegmentDisplay::displayInt(pin1Average.readAvg());
  Serial.println(pin2Average.readAvg());
  
  //Serial.println(_gyro.getAngle(Z, ANGLE_DIVIDER_SENSITIVITY_0));
}

void setupImuTimer(){
  //disable interrupts for config
  cli();
  //normal waveform generation mode, no output enabled
  TCCR3A = 0;
  //prescaler 256
  TCCR3B = (1 << CS32);
  //enable overflow interrupt
  TIMSK3 |= (1 << TOIE3);
  //reenable interrupts
  sei();
}

ISR (TIMER3_OVF_vect)
{
  _gyro.readAndProcessGyro();
}

ISR(ADC_vect){
  //write new value to floating average helper class
  if(lastRead == 0){
    pin1Average.write(ADC);
    //lastRead is now readNext...
    lastRead &= ~(1 << 0);
  }
  else{
    pin2Average.write(ADC);
    //lastRead is now readNext...
    lastRead |= (1 << 0);
  }
  //renew admux
  ADMUX = lastRead;
  //start next conversion
  //ADCSRA |= (1<<ADSC);
}
//13. januar fuer 4. praktische
