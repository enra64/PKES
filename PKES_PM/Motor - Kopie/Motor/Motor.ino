#include "wonki.h"
void setup() {
  DDRB |= (1<<PB6); //Motortreiber Timer 1
  DDRB |= (1<<PB5); //Motortreiber Timer 1
  DDRH |= (1<<PH3); //Motortreiber Timer 4
  DDRH |= (1<<PH4); //Motortreiber Timer 4
  
  initTimer1();
  initTimer4();
  //setup für entfernungsmesser
  setup_wonki();
}
#define DISTANCE_MAGIC 15

void loop(){
  //Serial.println(loop_wonki());
  int Distancevalue= loop_wonki();
  uint8_t duty=100;
  //Serial.println(loop_wonki());
  if(abs(Distancevalue)<=DISTANCE_MAGIC && Distancevalue<0){
      //while (abs(Distancevalue)<=15){
        backwardLeft(duty);
        forwardRight(duty);
      //}
      _delay_ms(400);
  }
  else if(Distancevalue > 0 && abs(Distancevalue)<=DISTANCE_MAGIC){
      //while(Distancevalue<=15){
        forwardLeft(duty);
        backwardRight(duty);
      //}
      _delay_ms(400);
    }  
  else{
  backwardLeft(duty);
  backwardRight(duty);
  //forwardLeft(duty);
  //forwardRight(duty);
  }
  

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

