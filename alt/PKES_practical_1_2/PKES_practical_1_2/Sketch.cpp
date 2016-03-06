#include <Arduino.h>

//Beginning of Auto generated function prototypes by Atmel Studio
void setup();
void sendNumbers();
void loop();
void sendTransmission();
void sendDataBit(boolean highOrLow);
//End of Auto generated function prototypes by Atmel Studio
#define CLOCK 35
#define DATA 34
#define DATA_ENABLE 33
#define DATA_ARRAY_LENGTH 37

byte data[3];

//               0     1     2     3     4     5     6     7     8     9     -
//byte number[] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6, 0x2};

byte number[] = {
0b11111100,
0b01100000,
0b11011010,
0b11110010,
0b01100110,
0b10110110,
0b10111110,
0b11100000,
0b11111110,
0b11110110,
0b00000010};

void setup() {
  // put your setup code here, to run once:
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA_ENABLE, OUTPUT);
  pinMode(DATA, OUTPUT);

  data[0] = number[0];
  data[1] = number[1];
  data[2] = number[2];

  //data[0] = 0xFF;
  //data[1] = 0xFF;
  //data[2] = 0xFF;
  //abcdef vs cdefgh
  //11111100 vs 00111111
  //bc vs fg
  //01100000 vs 00000110
  //abcd  g  vs b de gp
  //11110010 vs 01011011

  Serial.begin(57600);
}

void sendNumbers(){
  //per datasheet, prepend a 1
  sendDataBit(1);
  //send all three data bytes
  Serial.println("begin");
  for(byte digitPosition = 0; digitPosition < 3; digitPosition++){
    //sends a single digit
    //reverse? for(int i = 0; i <= 7; i++){
    for(int i = 7; i >= 0; i++){
      //high or low via bitshifting a mask
      bool val = (data[digitPosition] & (1 << i)) != 0;
      sendDataBit(val);
      Serial.print(val);
    }
    Serial.println("digit end");
  }
  Serial.println("end");
  //not wonky shit e.g. per datasheet
  for(int i = 0; i < 12; i++)
    sendDataBit(0);
}

void loop() {
  sendTransmission();
  delay(10000);
}

void sendTransmission(){
  //enable write
  digitalWrite(DATA_ENABLE, LOW);
  delayMicroseconds(1);
  //write out complete data array
  sendNumbers();
  //"write to display"
  digitalWrite(DATA_ENABLE, HIGH);
}

//data enable low, data high, clock high
void sendDataBit(boolean highOrLow){
  digitalWrite(DATA, highOrLow);
  delayMicroseconds(200);
  digitalWrite(CLOCK, HIGH);
  delayMicroseconds(200);
  digitalWrite(CLOCK, LOW);
}
