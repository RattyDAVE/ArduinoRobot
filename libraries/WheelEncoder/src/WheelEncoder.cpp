#include "Arduino.h"
#include "WheelEncoder.h"

WheelEncoder::WheelEncoder()
{
	int encoderValue = 0;

	// Direction TRUE, positive count (Forward). FALSE, negative count (Backward).
	boolean dir = true;
        
        
    //See https://www.arduino.cc/en/Reference/AttachInterrupt for pin use.
    // 2 and 3 are the only ones on a UNO.
        
    //pinMode(pin, INPUT);
    //attachInterrupt(pin, count, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(pin), count, CHANGE);
}

void WheelEncoder::count() {
  (encoderValue) ? encoderValue++ : encoderValue--;  
}

void WheelEncoder::reset() {
  encoderValue=0;  
}

int WheelEncoder::get() {
  return encoderValue;  
}

void WheelEncoder::direction(bool d) {
  dir = d;  
}
