#include "Arduino.h"
#include "WheelEncoder.h"

WheelEncoder::WheelEncoder()
{
	int encoderValue = 0;

	// Direction TRUE, positive count (Forward). FALSE, negative count (Backward).
	bool direction = TRUE; 
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
