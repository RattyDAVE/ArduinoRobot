#include "Arduino.h"
#include "WheelEncoder.h"

WheelEncoder::WheelEncoder(int pin)
{
	int encoderValue = 0;

	// Direction TRUE, positive count (Forward). FALSE, negative count (Backward).
	bool direction = TRUE;
        
        
        //See https://www.arduino.cc/en/Reference/AttachInterrupt for pin use.
        // 2 and 3 are the only ones on a UNO.
        
        pinMode(pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(pin), count, FALLING);
        //attachInterrupt(digitalPinToInterrupt(pin), count, CHANGE);
}

void WheelEncoder::count() {
  (direction) ? encoderValue++ : encoderValue--;  
}

void WheelEncoder::reset() {
  encoderValue=0;  
}

int WheelEncoder::get() {
  return encoderValue;  
}

void WheelEncoder::direction(bool d) {
  direction = d;  
}
