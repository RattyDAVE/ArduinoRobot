#include <WheelEncoder.h>

#define LEFT_PIN 2
#define RIGHT_PIN 3

WheelEncoder Left_Wheel;
WheelEncoder Right_Wheel;
// The followin lines below is a wrapper for the class methods. This is needed for the attachInterupt function.
void WheelLeftCount() {Left_Wheel.count();}
void WheelRightCount() {Right_Wheel.count();}
// End

void setup(){
  Serial.begin(230400);
  attachInterrupt(digitalPinToInterrupt(LEFT_PIN), WheelLeftCount, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_PIN), WheelRightCount, CHANGE);
}

void loop(){
  Serial.print(Left_Wheel.get());
  Serial.print("/t");
  Serial.println(Right_Wheel.get());
  delay(1000);
}

