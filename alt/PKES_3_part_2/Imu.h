#ifndef IMU_H_
#define IMU_H_

#define MPU9150_ACCEL_XOUT_H       0x3B   // R  
#define MPU9150_ACCEL_XOUT_L       0x3C   // R  
#define MPU9150_ACCEL_YOUT_H       0x3D   // R  
#define MPU9150_ACCEL_YOUT_L       0x3E   // R  
#define MPU9150_ACCEL_ZOUT_H       0x3F   // R  
#define MPU9150_ACCEL_ZOUT_L       0x40   // R  
#define MPU9150_TEMP_OUT_H         0x41   // R  
#define MPU9150_TEMP_OUT_L         0x42   // R  
#define MPU9150_GYRO_XOUT_H        0x43   // R  
#define MPU9150_GYRO_XOUT_L        0x44   // R  
#define MPU9150_GYRO_YOUT_H        0x45   // R  
#define MPU9150_GYRO_YOUT_L        0x46   // R  
#define MPU9150_GYRO_ZOUT_H        0x47   // R  
#define MPU9150_GYRO_ZOUT_L        0x48   // R  

//MPU9150 Compass
#define MPU9150_CMPS_XOUT_L        0x4A   // R
#define MPU9150_CMPS_XOUT_H        0x4B   // R
#define MPU9150_CMPS_YOUT_L        0x4C   // R
#define MPU9150_CMPS_YOUT_H        0x4D   // R
#define MPU9150_CMPS_ZOUT_L        0x4E   // R
#define MPU9150_CMPS_ZOUT_H        0x4F   // R

#define MPU9150_PWR_MGMT_1   0x6B   // R/W
#define MPU9150_I2C_ADDRESS  0x69

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define ACCEL 0
#define GYRO 1

#include <avr/io.h>
#include "General.h"
#include <Wire.h>
#include <Arduino.h>


//Variables where our values can be stored
int cmps[3];
int accl[3];
int gyro[3];
int temp;

int16_t MPU9150_readSensor(int addrL, int addrH){
  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addrL);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU9150_I2C_ADDRESS, 1, true);
  byte L = Wire.read();

  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addrH);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU9150_I2C_ADDRESS, 1, true);
  byte H = Wire.read();

  return (int16_t)((H<<8)+L);
}

int MPU9150_readSensor(int addr){
  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addr);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU9150_I2C_ADDRESS, 1, true);
  return Wire.read();
}

int MPU9150_writeSensor(int addr,int data){
  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission(true);

  return 1;
}

int16_t MPU_9150_readGyroAxis(uint8_t which){
  switch(which){
    case X_AXIS:
      return MPU9150_readSensor(MPU9150_GYRO_XOUT_L,MPU9150_GYRO_XOUT_H);
    case Y_AXIS:
      return MPU9150_readSensor(MPU9150_GYRO_YOUT_L,MPU9150_GYRO_YOUT_H);
    default:
    return MPU9150_readSensor(MPU9150_GYRO_ZOUT_L,MPU9150_GYRO_ZOUT_H);
  }
}

void MPU_9150_readSensor(Vector* result, uint8_t which){
  switch(which){
    case ACCEL:
      result->x = MPU9150_readSensor(MPU9150_ACCEL_XOUT_L,MPU9150_ACCEL_XOUT_H);
      result->y = MPU9150_readSensor(MPU9150_ACCEL_YOUT_L,MPU9150_ACCEL_YOUT_H);
      result->z = MPU9150_readSensor(MPU9150_ACCEL_ZOUT_L,MPU9150_ACCEL_ZOUT_H);
      break;
    case GYRO:
      result->x = MPU9150_readSensor(MPU9150_ACCEL_XOUT_L,MPU9150_ACCEL_XOUT_H);
      result->y = MPU9150_readSensor(MPU9150_ACCEL_YOUT_L,MPU9150_ACCEL_YOUT_H);
      result->z = MPU9150_readSensor(MPU9150_ACCEL_ZOUT_L,MPU9150_ACCEL_ZOUT_H);
      break;
  }
}

void MPU9150_setupCompass(){

  MPU9150_writeSensor(0x0A, 0x00); //PowerDownMode
  MPU9150_writeSensor(0x0A, 0x0F); //SelfTest
  MPU9150_writeSensor(0x0A, 0x00); //PowerDownMode

  MPU9150_writeSensor(0x24, 0x40); //Wait for Data at Slave0
  MPU9150_writeSensor(0x25, 0x8C); //Set i2c address at slave0 at 0x0C
  MPU9150_writeSensor(0x26, 0x02); //Set where reading at slave 0 starts
  MPU9150_writeSensor(0x27, 0x88); //set offset at start reading and enable
  MPU9150_writeSensor(0x28, 0x0C); //set i2c address at slv1 at 0x0C
  MPU9150_writeSensor(0x29, 0x0A); //Set where reading at slave 1 starts
  MPU9150_writeSensor(0x2A, 0x81); //Enable at set length to 1
  MPU9150_writeSensor(0x64, 0x01); //overvride register
  MPU9150_writeSensor(0x67, 0x03); //set delay rate
  MPU9150_writeSensor(0x01, 0x80);

  MPU9150_writeSensor(0x34, 0x04); //set i2c slv4 delay
  MPU9150_writeSensor(0x64, 0x00); //override register
  MPU9150_writeSensor(0x6A, 0x00); //clear usr setting
  MPU9150_writeSensor(0x64, 0x01); //override register
  MPU9150_writeSensor(0x6A, 0x20); //enable master i2c mode
  MPU9150_writeSensor(0x34, 0x13); //disable slv4
}

void initImu(bool enableLowPassFilter){
  Wire.begin();
  
  // Clear the 'sleep' bit to start the sensor.
  MPU9150_writeSensor(MPU9150_PWR_MGMT_1, 0);

  if(enableLowPassFilter){
    //set low pass filter
    int filterConfig = MPU9150_readSensor(0x1A);
  
    //clear last three bits
    filterConfig &= ~(111 << 0);
  
    //set filter to 98Hz
    filterConfig |= (1 << 1);
  
    MPU9150_writeSensor(0x1A, filterConfig);
  }
}


#endif //IMU_H
