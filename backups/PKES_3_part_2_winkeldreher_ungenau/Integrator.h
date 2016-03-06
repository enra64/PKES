#ifndef INTEGRATOR
#define INTEGRATOR

#include <avr/io.h>
#include "GlidingAverage.h"
#include "Imu.h"
#include "General.h"

#define TIMER_MS_PER_BUFFER_REFRESH (IMU_TIMER_MS_DELTA_PER_TICK * GL_AVG_SIZE)//96ms

//high precision values
#define ANGLE_DIVIDER_SENSITIVITY_0 131
#define ANGLE_DIVIDER_SENSITIVITY_1 66
#define ANGLE_DIVIDER_SENSITIVITY_2 33
#define ANGLE_DIVIDER_SENSITIVITY_3 16

#define IMU_ADC_READING_MINIMUM_ACCEPTED_VALUE 15
#define BUFFER_READING_MINIMUM_ACCEPTED_VALUE 5

class Integrator{
  GlidingAverage _buffer;
  int16_t _angle;
  uint8_t _writeCounter = 0;
  bool _hasUpdated = false;

  //whether the offset has been set yet, deny access if not, also deny setting offset if buffer not full
  bool _offsetSaved = false;
  int16_t _offset = 0;
  bool _bufferFilledOnce = false;
  
  //configuration values
  uint8_t _sensitivity = ANGLE_DIVIDER_SENSITIVITY_0;//default value
  uint8_t _axis = Z_AXIS; //default value

  void processAngleDelta(){
    _writeCounter = 0;
    _bufferFilledOnce = true;
    int32_t avg = _buffer.readAvg();
    //Serial.print("integrator buffer avg: ");
    //Serial.println(avg);
    //ignore tiny values to avoid drift
    if(abs(avg) < BUFFER_READING_MINIMUM_ACCEPTED_VALUE)
      avg = 0;
    //avg *= 5;
    //avg /= 120;
    avg *= 6; // magic number
    avg /= _sensitivity * 10;// avg / sens * (1 / 0.096s)
    _angle += avg;
    _hasUpdated = true;
  }

  //save gyro axis data, and integrate whenever the buffer has been completely refreshed (currently at 32 values)
  void injectGyroData(int16_t axisAdc){
    axisAdc -= _offset;
    //if the offset has already been calculated, we can block out values smaller than 15, since that is a tiny adc reading, but affects drift by a large amount
    if(_offsetSaved && abs(axisAdc) < IMU_ADC_READING_MINIMUM_ACCEPTED_VALUE)
      axisAdc = 0;
    //Serial.print("cleaned imu adc value: ");
    //Serial.println(axisAdc);
    _buffer.write(axisAdc);
    _writeCounter++;
    if(_writeCounter == GL_AVG_SIZE)  
      processAngleDelta();
  }
  
  public:
  //setter functions so this class can do more than z axis @ 250 deg/s, even though that is default
  void setSensitivity(uint8_t value){ _sensitivity = value; }
  void setAxis(uint8_t axis){ _axis = axis; }

  //keep track of whether the latest data has been polled
  bool hasUpdated(){
    if(_hasUpdated){
      _hasUpdated = false;
      return true;
    }
    return false; 
  }
  
  //use this to get current angle
  int16_t getAngle(){
    //do not send valid data if no offset has been set
    if(!_offsetSaved) return -1;
    return _angle;
  }

  //call this from outside to trigger a new gyro measurement
  void readAndProcessGyro(){
    injectGyroData(MPU_9150_readGyroAxis(_axis));
  }

  void reset(){
    _angle = 0;
    _buffer.clear();
    _offsetSaved = false;
    _bufferFilledOnce = false,
    _offset = 0;
    _writeCounter = 0;
  }

  //save the buffer value on no movement to be able to combat drift
  bool createReference(){
    if(_bufferFilledOnce){
      _angle = 0;
      _offset = _buffer.readAvg();
      _buffer.clear();
      _offsetSaved = true;
      //Serial.print("offset is:");
      //Serial.println(_offset);
      return true;
    }
    return false;
  }
};
#endif
