#ifndef GENERAL
#define GENERAL

//processor speed for delay.h
#define F_CPU 16000000L

//includes
#include <Arduino.h>
#include <stdbool.h>//bool  
#include <avr/io.h>//uint8_t
#include <avr/interrupt.h>//enable interrupts, currently mainly for serial communication

//struct defines
struct imu_data{
	int16_t x, y, z;
};

class Vector {
  public:
  int16_t x=0, y=0, z=0;

  int16_t magnitude(){
    return sqrt(x*x+y*y+z*z);
  }

  void add(Vector* v){
    x += v-> x;
    y += v-> y;
    z += v-> z;
  }

  void mult(int16_t m){
    x *= m;
    y *= m;
    z *= m;
  }

  void div(int16_t m){
    x /= m;
    y /= m;
    z /= m;
  }

  void set(int16_t nx, int16_t ny, int16_t nz){
    x = nx;
    y = ny;
    z = nz;
  }

  int16_t normalize(){
    int16_t mag = magnitude();
    x /= mag;
    y /= mag;
    z /= mag;
  }

  void copy(Vector* val){
    x = val->x;
    y = val->y;
    z = val->z;
  }
};

//led pins
#define LED_1 PJ1
#define LED_2 PA5
#define LED_3 PA4
#define LED_4 PD3

//general function prototypes
/*
 * Write high or low to a pin on a port
 * writeDigital(&PORTC, PC2, true)
 */
void writeDigital(volatile uint8_t* port, int pin, bool val){
  if(val)
    *port |=  (1<<pin);//set bit
  else
    *port &=~ (1<<pin);//delete bit
}
//fastest way to do it btw
int digitCount (int n) {
  //lets hope n is != minint, because in c, that fucks up everything
    if (n < 0) n = -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    /*      2147483647 is 2^31-1 - add more ifs as needed
       and adjust this final return as well. */
    return 10;
}
/*get digit from number; negative values are ignored*/
uint8_t getDigit(double val, int pos){
  // 543210.123456 //after point is negative position
  //be positive!
  if(val < 0)
    val = -val;
  //is before point
  if(pos >= 0)
    return (int)(((int) val) / pow(10,pos)) % 10;
  //is after point
  return (val - (int) val) * pow(10, -pos);
}

int8_t sign(float a){
  if(a < 0)
    return -1;
  return 1;
}

#endif /* LIBRARY_H_ */
