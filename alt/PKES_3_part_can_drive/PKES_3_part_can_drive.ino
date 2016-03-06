#include "arnes_7SD.h"
#include "arnes_library.h"
#include <Wire.h>
#include <avr/io.h>
#include "imu.h"
#include "arnes_DAC.h"
#include "ADConverter.h"
#include "gyro.h"

//floating average class used for casting a floating average over the sensor values to kill spikes
FloatingAverage pin1Average(5), pin2Average(5);
Gyro _gyro;
//pins used for the two distance sensors. complete admux register to avoid an operation in isr
#define ADC_PIN_1 0b11110000
#define ADC_PIN_2 0b11110001
//keep track of last read adc pin to enable switching in isr
uint8_t lastRead = ADC_PIN_1;

int16_t _speed = 0;
bool fwd = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("test"); 
  //make 7SD pins output
  DDRC = (1 << SEVEN_SEGMENT_CLOCK) | (1 << SEVEN_SEGMENT_DATA) | (1 << SEVEN_SEGMENT_DATA_ENABLE);
  //set all 7SD pins to off state
  PORTC = 0x00;
  _gyro = Gyro();
  setupImuTimer();
  initAdc();
  initMotors();

  //max is 1023, e.g. 10 bit
  setMotor(512, FOR_2);
  setMotor(512, FOR_1);

  //just read a single voltage from adc, that will trigger the adc isr
  //readVoltageInterrupt(0);

  Serial.println("init ok");
}

void loop() {
  _delay_ms(20);

  //if(_speed > 0){
  //  setMotor(_speed * 2, FOR_1);
  //  setMotor(_speed * 2, FOR_2);  
  //}
  //else{
  //  setMotor(-_speed * 2, REW_1);
  //  setMotor(-_speed * 2, REW_2);
  //}

  //based on overflow, drive forward/reward
  //_speed++;

  Serial.println(_gyro.getAngle(X, ANGLE_DIVIDER_SENSITIVITY_0));
     
  //since we triggered the adc isr in setup, that should take care of filling the avg containers
  //Serial.print("A:");
  //Serial.print(interpretDistanceSensor(pin1Average.read(), 49));
  //Serial.print("\tB:");
  //Serial.println(interpretDistanceSensor(pin2Average.read(), 49));
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
  if(lastRead == ADC_PIN_1){
    pin1Average.write(ADC);
    //lastRead is now readNext...
    lastRead = ADC_PIN_2;
  }
  else{
    pin2Average.write(ADC);
    //lastRead is now readNext...
    lastRead = ADC_PIN_1;
  }
  //renew admux
  ADMUX = lastRead;
  //start next conversion
  ADCSRA |= (1<<ADSC);
}
