#include <SunFounderLineFollower.h>

byte sensorbits;
int * sensorarray;
SunFounderLineFollower sflf;

void setup(){
  Serial.begin(115200);
}

void loop(){
  sensorbits=sflf.readSensor();
  printBits(byte sensorbits);
  Serial.println("");

  //if (sensorbits = B00000000) NO LINE
  //else if (sensorbits = B11111111) T STOP
  //else if (sensorbits & B00000001) HARD RIGHT
  //else if (sensorbits & B00000010) MED RIGHT
  //else if (sensorbits & B00000100) SOFT RIGHT
  //else if (sensorbits & B00001000) Centre
  //else if (sensorbits & B00010000) Centre
  //else if (sensorbits & B00100000) SOFT LEFT
  //else if (sensorbits & B01000000) MED LEFT
  //else if (sensorbits & B10000000) HARD LEFT
  
  
  sensorarray=sflf.getreadings();
  for (int i=0; i <= 16; i++){
      printBits(sensorarray[i]);
      Serial.print('\t');
  }
  Serial.println("");
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}
