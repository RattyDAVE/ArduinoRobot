/******************************************************************************
MostBasicFollower.ino

A very simple method for following a line with a redbot and the line follower array.

Marshall Taylor, SparkFun Engineering

5-27-2015

Library:
https://github.com/sparkfun/SparkFun_Line_Follower_Array_Arduino_Library
Product:
https://github.com/sparkfun/Line_Follower_Array

This example demonstrates the easiest way to interface the redbot sensor bar.
  "SensorBar mySensorBar(SX1509_ADDRESS);" creates the sensor bar object.
  "mySensorBar.init();" gets the bar ready.
  "mySensorBar.getDensity()" gets the number of points sensed.
  "mySensorBar.getPosition()" gets the average center of sensed points.

The loop has three main points of operation.
  1.  check if the density is reasonable
  2.  get the position
  3.  choose a drive mode based on position

Note:
The wheel direction polarity can be switched with RIGHT/LEFT_WHEEL_POL (or by flipping the wires)
  #define RIGHT_WHEEL_POL 1
  #define LEFT_WHEEL_POL 1
To check, hold the bot centered over a line so that the two middle sensors detect
  then observe.  Does the bot try to drive forward?
  
Resources:
sensorbar.h

Development environment specifics:
arduino > v1.6.4
hw v1.0

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/


#include "Wire.h"
#include "sensorbar.h"
#include <RedBot.h>  // This line "includes" the RedBot library into your sketch.
// Provides special objects, methods, and functions for the RedBot.
//The Redbot Library can be found here: https://github.com/sparkfun/RedBot

// Uncomment one of the four lines to match your SX1509's address
//  pin selects. SX1509 breakout defaults to [0:0] (0x3E).
const uint8_t SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
//const byte SX1509_ADDRESS = 0x3F;  // SX1509 I2C address (01)
//const byte SX1509_ADDRESS = 0x70;  // SX1509 I2C address (10)
//const byte SX1509_ADDRESS = 0x71;  // SX1509 I2C address (11)

SensorBar mySensorBar(SX1509_ADDRESS);

RedBotMotors motors; // Instantiate the motor control object. This only needs
// to be done once.

//Define motor polarity for left and right side -- USE TO FLIP motor directions if wired backwards
#define RIGHT_WHEEL_POL 1
#define LEFT_WHEEL_POL 1

//Define the states that the decision making machines uses:
#define IDLE_STATE 0
#define READ_LINE 1
#define GO_FORWARD 2
#define GO_LEFT 3
#define GO_RIGHT 4

uint8_t state;


void setup()
{
  Serial.begin(9600);  // start serial for output
  Serial.println("Program started.");
  Serial.println();
  
  //Default: the IR will only be turned on during reads.
  mySensorBar.setBarStrobe();
  //Other option: Command to run all the time
  //mySensorBar.clearBarStrobe();

  //Default: dark on light
  mySensorBar.clearInvertBits();
  //Other option: light line on dark
  //mySensorBar.setInvertBits();
  
  //Don't forget to call .begin() to get the bar ready.  This configures HW.
  uint8_t returnStatus = mySensorBar.begin();
  if(returnStatus)
  {
	  Serial.println("sx1509 IC communication OK");
  }
  else
  {
	  Serial.println("sx1509 IC communication FAILED!");
  }
  Serial.println();
  
}

void loop()
{


  uint8_t nextState = state;
  switch (state) {
  case IDLE_STATE:
    motors.stop();       // Stops both motors
    nextState = READ_LINE;
    break;
  case READ_LINE:
    if( mySensorBar.getDensity() < 7 )
    {
      nextState = GO_FORWARD;
      if( mySensorBar.getPosition() < -50 )
      {
        nextState = GO_LEFT;
      }
      if( mySensorBar.getPosition() > 50 )
      {
        nextState = GO_RIGHT;
      }
    }
    else
    {
      nextState = IDLE_STATE;
    }
    break;
  case GO_FORWARD:
    driveBot(200);
    nextState = READ_LINE;
    break;
  case GO_LEFT:
    turnBot(-.75);
    nextState = READ_LINE;
    break;
  case GO_RIGHT:
    turnBot(.75);
    nextState = READ_LINE;
    break;
  default:
    motors.stop();       // Stops both motors
    break;
  }
  state = nextState;
  //delay(100);
}

//When using driveBot( int16_t driveInput ), pass positive number for forward and negative number for backwards
//driveInput can be -255 to 255
void driveBot( int16_t driveInput )
{
	int16_t rightVar;
	int16_t leftVar;
	rightVar = driveInput * RIGHT_WHEEL_POL;
	leftVar = -1 * driveInput * LEFT_WHEEL_POL;
	motors.rightMotor(rightVar);
    motors.leftMotor(leftVar);
	
}

//When using ( int16_t driveInput, float turnInput ), pass + for forward, + for right
//driveInput can be -255 to 255
//turnInput can be -1 to 1, where '1' means turning right, right wheel stopped
void driveTurnBot( int16_t driveInput, float turnInput )
{
	int16_t rightVar;
	int16_t leftVar;
	//if driveInput is negative, flip turnInput
	if( driveInput < 0 )
	{
		turnInput *= -1;
	}
	
	//If turn is positive
	if( turnInput > 0 )
	{
		rightVar = driveInput * RIGHT_WHEEL_POL * ( 1 - turnInput );
		leftVar = -1 * driveInput * LEFT_WHEEL_POL;
	}
	else
	{
		rightVar = driveInput * RIGHT_WHEEL_POL;
		leftVar = -1 * driveInput * LEFT_WHEEL_POL * (1 - ( turnInput * -1));
	}

	motors.rightMotor(rightVar);
    motors.leftMotor(leftVar);
		delay(5);
		
}

//When using turnBot( float turnInput ), pass + for spin right.
//turnInput can be -1 to 1, where '1' means spinning right at max speed
void turnBot( float turnInput )
{
	int16_t rightVar;
	int16_t leftVar;
	//If turn is positive
	if( turnInput > 0 )
	{
		rightVar = -1 * 255 * RIGHT_WHEEL_POL * turnInput;
		leftVar = -1 * 255 * LEFT_WHEEL_POL * turnInput;
	}
	else
	{
		rightVar = 255 * RIGHT_WHEEL_POL * turnInput * -1;
		leftVar = 255 * LEFT_WHEEL_POL * turnInput * -1;
	}

	motors.rightMotor(rightVar);
    motors.leftMotor(leftVar);
	delay(5);
	
}

