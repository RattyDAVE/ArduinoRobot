#include <LEDDisplay.h>

void setup()
{
  // LED connections start at output 32,
  // common must be HIGH to be on. 
  LEDDisplay* d = new LEDDisplay(32, true);
  d->startInterrupt();
}

void loop()
{
  for(int i=0;i<100;++i) {
    LEDDisplay::instance()->setValueDec(i);
    ++i;
    delay(250);
  }

  for(unsigned int i=50000;i<53000;i+=100) {
    LEDDisplay::instance()->setValueDec(i);
    ++i;
    delay(250);
  }

  for(unsigned int i=50000;i<53000;i+=123) {
    LEDDisplay::instance()->setValueHex(i);
    ++i;
    delay(250);
  }

  float d = 1.234;  
  while(d<9.9) {
    LEDDisplay::instance()->setValueFloat(d, 2);
    d += 0.543;
    delay(250);
  }
}
