#include "Arduino.h"
#include "SunFounderLineFollower.h"
#include <Wire.h>


SunFounderLineFollower::SunFounderLineFollower()
{
	
	//SDA=D2 and SCL=D1
    //Wire.begin(D1,D2);
    Wire.begin();
    //Calibration Min and Max
    int  cal_max[8];
    int  cal_min[8];
      
}

void SunFounderLineFollower::calibrate()
{
  int raw[8];
  Wire.requestFrom(9, 16);
    for (int x=0; x <= 100; x++) {
	for (int i=0; i <= 7; i++) {
	  raw[i] = Wire.read() << 8 | Wire.read();
	  
	  if (cal_min[i] < 1){
		  cal_min[i] = raw[0];
	   }	  
	  cal_min[i] = ((raw[i] > 0) && (raw[i] < cal_min[i])) ? raw[i] : cal_min[i];
	  cal_max[i] = (raw[i] > cal_max[i]) ? raw[i] : cal_max[i];
	}
  }
}

void SunFounderLineFollower::calibrate_show()
{
  Serial.print("cal_show min\t");
  	for (int i=0; i <= 7; i++) {
	    Serial.print(cal_min[i]);
        Serial.print("\t"); 
	}    
	Serial.println("");

  Serial.print("cal_show max\t");
  	for (int i=0; i <= 7; i++) {
	    Serial.print(cal_max[i]);
        Serial.print("\t"); 
	}
	Serial.println("");    
 }

void SunFounderLineFollower::calibrate_reset()
{
  	for (int i=0; i <= 7; i++) { 
		cal_min[i]=0; 
		cal_max[i]=0;
	}
 }

void SunFounderLineFollower::test()
{
  Serial.print("test \t"); 
  int  r[8];
  unsigned char _data[16];
  int reqlen=Wire.requestFrom(9, 16);    // request 16 bytes from slave device #9
  for (int i=0; i <= reqlen-1; i=i+2){
    _data[i] = Wire.read(); // receive a byte as character
    _data[i+1] = Wire.read(); // receive a byte as character
    r[i/2] = _data[i] << 8 | _data[i+1];
    Serial.print(r[i/2]);
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

int SunFounderLineFollower::byteprocessed(int s) {
  int r = 0;
  Wire.requestFrom(9, 16);
  for (int i=0; i <= 7; i++) (( Wire.read() << 8 | Wire.read() ) > s)?r|=1<<i:r&=~(1<<i);
  return r;
}
