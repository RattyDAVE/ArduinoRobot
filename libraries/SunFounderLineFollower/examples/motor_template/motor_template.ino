#include <SunFounderLineFollower.h>

SunFounderLineFollower sflf;
int sensitivity = 9000;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int byteprocessed = sflf.byteprocessed(sensitivity);

  if ( bitRead(byteprocessed, 0) || bitRead(byteprocessed, 1) || bitRead(byteprocessed, 2) ) {
  //Right Turn
  motorRight();
  }

  else if ( bitRead(byteprocessed, 3) || bitRead(byteprocessed, 4) ) {
  //Forward
  motorForward();
  }

  else if ( bitRead(byteprocessed, 5) || bitRead(byteprocessed, 6) || bitRead(byteprocessed, 7) ){
  //Left Turn
  motorLeft();
  }
  else {
    //Lost the line... STOP!!!!
    motorStop();
  }

  
  delay(50); 
}

int motorRight() {
  Serial.println("Turning Right");
  //Put here motor right code....
}
int motorStop() {
  Serial.println("LOST LINE");
  //Put here motor stop code
}
int motorLeft() {
  Serial.println("Turning Left");
  //Put here motor Left Code
}

int motorForward() {
  Serial.println("Forward!!!");
  //Put here motor Forward!
}


