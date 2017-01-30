#include <SunFounderLineFollower.h>
int sensorbits;
SunFounderLineFollower sflf();

void setup(){
}

void loop(){
  sensorbits=sflf.readSensor();

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

}
