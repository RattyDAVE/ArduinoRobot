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
  float d = (float)analogRead(0) / (1024.0 / 5.0) ;
  LEDDisplay::instance()->setValueFloat(d, 3);
  delay(50);
}
