#ifndef BUTTON_H
#define BUTTON_H

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ESP8266)
#include "Arduino.h"  
#else  
#include "WProgram.h"  
#endif

#define PULLUP HIGH
#define PULLDOWN LOW

#define CURRENT 0
#define PREVIOUS 1
#define CHANGED 2

class Button{
  public:
    Button(uint8_t buttonPin, uint8_t buttonMode=PULLDOWN);
    void pullup();
    void pulldown();
    bool isPressed();
    bool wasPressed();
    bool stateChanged();
    bool uniquePress();
  private: 
    uint8_t pin;
    uint8_t mode;
    uint8_t state;
};

#endif


