#include "Arduino.h"
#include "Morse.h"
#include <Wire.h>
#define SENSOR_SENSITIVITY 127


SunFounderLineFollower::SunFounderLineFollower()
{
    Wire.begin();
}

int SunFounderLineFollower::readSensor()
{
  int _linSensor;
  unsigned char _data[16];
  int howmany=Wire.requestFrom(9, 16);    // request 16 bytes from slave device #9
  for (int i=0; i <= howmany; i=i+2){
    _data[i] = Wire.read(); // receive a byte as character
    _data[i+1] = Wire.read(); // receive a byte as character
    if (_data[i] > SENSOR_SENSITIVITY) bitSet(_linSensor,i);
    else bitClear(_linSensor,i);
  }
  return _linSensor;
}
