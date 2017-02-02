#include "Arduino.h"
#include "SunFounderLineFollower.h"
#include <Wire.h>


SunFounderLineFollower::SunFounderLineFollower()
{
	//SDA=D2 and SCL=D1
    Wire.begin(D1,D2);
}

void SunFounderLineFollower::davetest()
{
  Serial.print("davetest \t"); 
  int  _linSensor[8];
  unsigned char _data[16];
  int reqlen=Wire.requestFrom(9, 16);    // request 16 bytes from slave device #9
  for (int i=0; i <= reqlen-1; i=i+2){
    _data[i] = Wire.read(); // receive a byte as character
    _data[i+1] = Wire.read(); // receive a byte as character
    _linSensor[i / 2] = _data[i] << 8 | _data[i+1];
    Serial.print(_linSensor[i / 2]);
    Serial.print("\t"); 
  }
  Serial.println("");
}

int * SunFounderLineFollower::rawarray() {
  int* raw = new int[8];
  Wire.requestFrom(9, 16);
  for (int i=0; i <= 7; i++) raw[i] = Wire.read() << 8 | Wire.read();
  return raw;
}

int SunFounderLineFollower::byteprocessed(int SENSITIVITY) {
  int probyte;
  Wire.requestFrom(9, 16);
  for (int i=0; i <= 7; i++) {
    if ( ( Wire.read() << 8 | Wire.read() ) > SENSITIVITY) bitSet(probyte,i);
    else bitClear(probyte,i);
  }
  return probyte;
}

