#include <avr/io.h>
#include <util/delay.h>
const uint16_t CONV_TBL_47[]{587,3,550,4,451,5,388,6,338,7,305,8,273,9,244,10,225,11,209,12,195,13,184,14,172,15,160,16,152,17,144,18,140,19,137,20,129,21,124,22,120,23,116,24,110,25};
const uint16_t CONV_TBL_39[]{572,3,536,4,455,5,394,6,340,7,302,8,270,9,248,10,227,11,209,12,194,13,180,14,175,15,165,16,156,17,148,18,143,19,137,20,125,21,117,22,108,23,100,24,95,25};
const uint16_t CONV_TBL_49[]={600,3,500,4,444,5,380,6,335,7,300,8,275,9,250,10,240,11,218,12,205,13,190,14,180,15,172,16,163,17,156,18,150,19,143,20,142,21,141,22,140,23,135,24,130,25};
uint16_t adcresult;
float normalresult;
int roundedCM;
uint16_t mittelwert;
byte ziffern[13][8] = {
  {1, 1, 1, 1, 1, 1, 0, 0}, //0
  {0, 1, 1, 0, 0, 0, 0, 0}, //1
  {1, 1, 0, 1, 1, 0, 1, 0}, //2
  {1, 1, 1, 1, 0, 0, 1, 0}, //3
  {0, 1, 1, 0, 0, 1, 1, 0}, //4
  {1, 0, 1, 1, 0, 1, 1, 0}, //5
  {1, 0, 1, 1, 1, 1, 1, 0}, //6
  {1, 1, 1, 0, 0, 0, 0, 0}, //7
  {1, 1, 1, 1, 1, 1, 1, 0}, //8
  {1, 1, 1, 1, 0, 1, 1, 0}, //9
  {0, 0, 0, 0, 0, 0, 0, 0}, //aus
  {0, 0, 0, 0, 0, 0, 1, 0}, //minus
  {0, 1, 1, 0, 0, 0, 1, 0}  //minus eins
};
#define clock PC2
#define dataenable PC4
#define data PC3
byte DatenBitsArray[36] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void setup() 
{
  DDRC |= (1<<clock);
  DDRC |= (1<<data);
  DDRC |= (1<<dataenable);
  Init();
  Serial.begin(57600);
}

void loop() {
  for(int i = 0;i<100;i++){
    mittelwert = mittelwert + adc_read(0);
  }
  mittelwert= mittelwert/100;
  darstellung(interpretDistanceSensor(mittelwert,49));
  _delay_ms(200);
}

void Init( void )
{
   ADCSRA = 0;
   //ADC prescaler= 128  ändert die spannung
   ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 );
   //Setzt die Volt referenz
   ADMUX |= ( 1 << REFS0 ); 
   // ADC on
   ADCSRA |= ( 1 << ADEN );
   //macht den ersten durchlauf (dauert am längsten)
   ADCSRA |= ( 1 << ADSC );
   
   int16_t b = ADC;
}
uint16_t adc_read(uint8_t channel)
{   
  _delay_ms(1);
   //Clear letzten read channel
   ADMUX &= 0xf0;
   //Wähle zu lesenden adc channel
   ADMUX |= channel;
   //Starte neue conversion
   ADCSRA |= ( 1 << ADSC );
   //Lösche ADIF Flag
   ADCSRA |= ( 1 << ADIF );
   //warte bis Conversion fertig
   while( ADCSRA & ( 1 << ADIF ));
   //Lösche ADIF flag
   ADCSRA |= ( 1 << ADIF );
   Serial.println("adcread");
   //return undefinierten int wert als ergebnis 
   return ADC;
}

void darstellung (int counter){
  _delay_ms(1);
  byte zahl[3] = {counter / 100, counter / 10 % 10, counter % 10};
  if (counter < 10){
    zahl[0] = 10;
    zahl[1] = 10;    
  }
  if (counter < 100)
    zahl[0] = 10;
  int BitPointer = 1;
  for (int zahlindex = 0; zahlindex < 3; zahlindex++) {
    for (int resetter = 0; resetter < 8; resetter++) {

      DatenBitsArray[BitPointer] = ziffern[zahl[zahlindex]][resetter];
      BitPointer++;
    }
  }
  sendData(DatenBitsArray);
  _delay_ms(1);
}
void sendData(unsigned char ar[]) {
  PORTC &= ~(1<<dataenable);
  _delay_ms(1);
  for (int i = 0; i < 36; i = i + 1) {
    if(ar[i] == 1)
      PORTC |= (1<<data);
    else
      PORTC &= ~(1<<data);
    _delay_ms(1);
    PORTC |= (1<<clock);
    _delay_ms(1);
    PORTC &= ~(1<<clock);
    _delay_ms(1);
  }
  PORTC |= (1<<dataenable);
  _delay_ms(100);
}
//type must be 49(0), 39(2) or 47(1)
int interpretDistanceSensor(int16_t value, uint8_t type){
  if(type == 49 || type == 0)
    for(int i = 0; i < 46; i+=2)
      if(value > CONV_TBL_49[i])
        return CONV_TBL_49[i+1];
  else if(type == 47 || type == 1)
    for(int i = 0; i < 46; i+=2)
      if(value > CONV_TBL_47[i])
        return CONV_TBL_47[i+1];
  else if(type == 39 || type == 0)
    for(int i = 0; i < 46; i+=2)
      if(value > CONV_TBL_39[i])
        return CONV_TBL_39[i+1];
  else
    return -1;
  return 999;
}
