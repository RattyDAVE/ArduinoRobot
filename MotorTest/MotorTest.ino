#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *RightMotor = AFMS.getMotor(2);

void setup(){
	AFMS.begin();
}

void loop(){
	LeftMotor->setSpeed(255); 
	RightMotor->setSpeed(255); 
	LeftMotor->run(FORWARD);
	Right->run(FORWARD);
	delay(5000);
	LeftMotor->run(RELEASE);
	Right->run(RELEASE);
	delay(5000);
}
