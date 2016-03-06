#include <Wire.h>
#include <imu.h>
#include <avr/io.h>
#include <util/delay.h>
float currentAngle=0;
int offset;
int targetDegree = 0;
int duty=100;
void MPU9150_readSensor(void);


void setup() {
  DDRB |= (1<<PB6); //Motortreiber Timer 1
  DDRB |= (1<<PB5); //Motortreiber Timer 1
  DDRH |= (1<<PH3); //Motortreiber Timer 4
  DDRH |= (1<<PH4); //Motortreiber Timer 4
  
  initTimer1();
  initTimer4();

  Serial.begin(57600);
  // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin();
Serial.println("alua akbar");
  // Clear the 'sleep' bit to start the sensor.
  MPU9150_writeSensor(MPU9150_PWR_MGMT_1, 0);
  Serial.println("es funktuioniert");
  offset=gyro_init();
Serial.println("vieleicht");
  uint8_t IMUConfigreg = MPU9150_readSensor(26);
  IMUConfigreg &= ~(111 << 0); //low pass filter register clearen  
  IMUConfigreg |= (11 << 0); //low pass filter auf konfiguration 3 setzen
  MPU9150_writeSensor(26, IMUConfigreg);
  Serial.println("nein");
  while(!Serial.available());
  Serial.println("nutte");
  targetDegree = Serial.read();
    
}

void loop() {
  if(abs(targetDegree)>abs(currentAngle)){
    currentanglefunc();
    Serial.println(currentAngle);
    if(targetDegree<0){
      forwardRight(duty);
      backwardLeft(duty);
    }
    else{
      forwardLeft(duty);
      backwardRight(duty);
    }
  }
  else{
    forwardLeft(0);
    backwardRight(0);
  }
  _delay_ms(9);
}

void currentanglefunc(){
 //This line converts the 0-1023 signal to 0-5V
  float gyroRate = ((float)((float)MPU9150_readSensor(MPU9150_GYRO_ZOUT_L, MPU9150_GYRO_ZOUT_H)-offset) * 3.3f) / (float) 32768;
  Serial.println(gyroRate);
  //This line divides the voltage we found by the gyro's sensitivity
  gyroRate /= 0.0066;

  //Ignore the gyro if our angular velocity does not meet our threshold
  if (true || gyroRate >= 1 || gyroRate <= -1) {
    //This line divides the value by 100 since we are running in a 10ms loop (1000ms/10ms)
    gyroRate /= 100;
    currentAngle += gyroRate;
  }

  if (currentAngle < 0)
    currentAngle += 360;
  else if (currentAngle > 359)
    currentAngle -= 360;
}




   
void forwardLeft(uint8_t duty){
  TCCR4A &= ~(1<< COM4B1);
  TCCR4A |= (1<< COM4A1);
  OCR4A = duty;
}
void forwardRight(uint8_t duty){
  TCCR1A &= ~(1<< COM1B1);
  TCCR1A |= (1<< COM1A1);
  OCR1A = duty;
}
void backwardLeft(uint8_t duty){
  TCCR4A &= ~(1<< COM4A1);
  TCCR4A |= (1<< COM4B1);
  OCR4B = duty;
}

void backwardRight(uint8_t duty){
  TCCR1A &= ~(1<< COM1A1);
  TCCR1A |= (1<< COM1B1);
  OCR1B = duty;
}

void initTimer4(){
  //Prescaler auf 8
  TCCR4B |= (1<<CS41);
  //TCCR4B |= (1<<CS40);
  
  //toggle fast PWM mode
  TCCR4B |= (1<< WGM42);
  TCCR4A |= (1<< WGM40);
 
  //Timer/Counter auf 0 setzen
  TCNT4 = 0;
}

void initTimer1(){
  //Prescaler auf 8
  TCCR1B |= (1<<CS11);
  //TCCR1B |= (1<<CS10);
  
  //toggle fast PWM mode
  TCCR1B |= (1<< WGM12);
  TCCR1A |= (1<< WGM10);

  //non inverting mode
  TCCR1A |= (1<< COM1B1);
  
  
  //Timer/Counter auf 0 setzen
  TCNT1 = 0;
}
int gyro_init(){
  int mittelwert_ausgang;
  for(int i=0;i<1000;i++)
    mittelwert_ausgang +=MPU9150_readSensor(MPU9150_GYRO_ZOUT_L,MPU9150_GYRO_ZOUT_H);
 return mittelwert_ausgang / 1000;
}
