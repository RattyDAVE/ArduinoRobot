#include <LEDDisplay.h>

LEDDisplay *d;
int i=0;
void setup()
{
  d = new LEDDisplay(32, true);
  d->setValueDec(0);
  d->startInterrupt();
}

void loop()
{
  int i = analogRead(0);
  d->setValueDec(i);
  // change the line below to render in hex instead of decimal
  //  d->setValueHex(i);
  delay(100);
}
