#ifndef INTEGRATOR
#define INTEGRATOR

#include <avr/io.h>
#include "Imu.h"
#include "General.h"

#define TIMER_MS_PER_BUFFER_REFRESH (IMU_TIMER_MS_DELTA_PER_TICK * GL_AVG_SIZE)

#define IMU_ADC_VOLTAGE 3.3f
#define INT16_MAX 32768

class Integrator{
  int16_t _offset;
  long lastms = 0;
  float _angle;
  
  public:  
  //use this to get current angle
  float getAngle(){
    return _angle;
  }

  int16_t readGyro(){
    return MPU_9150_readGyroAxis(Z_AXIS) - _offset;
  }

  //call this from outside to trigger a new gyro measurement
  void readAndProcessGyro(){
    float gRate = (float)((float)readGyro() * (float)IMU_ADC_VOLTAGE) / (float)INT16_MAX;
    gRate /= 0.0115; //magic number... should be 3.3V / 500° = 0.0066 // 0.0115
    //called all 20ms, so divide by (1000 / 20)
    gRate /= 50;
    _angle += gRate;
  }

  //reset the class
  void reset(){
    _angle = 0;
    createOffset();
  }

  //save the buffer value on no movement to be able to combat drift
  void createOffset(){
    int32_t sum = 0;
    for(int i = 0; i < 1000; i++)
      sum += MPU_9150_readGyroAxis(Z_AXIS);
    _offset = (int16_t) (sum / 1000);
    Serial.print("offset:");
    Serial.println(_offset);
  }
};
#endif
