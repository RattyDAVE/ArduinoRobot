#include <LEDDisplay.h>

int i=0;
void setup()
{
  LEDDisplay* d = new LEDDisplay(32, true);
  d->startInterrupt();
}

int current = 0;

void loop()
{
  // Text for @TCCdesign arduino@
  char tcc[] = {0x1a, 0x0c, 0x0c, 0x1d, 0x0d, 0x0e, 0x19, 0x12, 0x10,
                0x15, 0x0a, 0x18, 0x0d, 0x01b, 0x12, 0x15, 0x16,
                0x1d, 0x1d, 0x1d, 0x1d, 0x1d
              };

  if(current > (sizeof(tcc) - LED_NUM_DIGITS)) {
   current = 0;
  }
  LEDDisplay* d = LEDDisplay::instance();
  for(int i=0;i<LED_NUM_DIGITS;++i) {
    d->setValueRaw(i, tcc[i+current]);
  }

  delay(500);
  current++;
}
