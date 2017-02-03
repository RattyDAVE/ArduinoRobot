/******************************************************************************
ReadBarOnly.ino

A sketch that demonstrates how the data comes from the sensor bar.

Marshall Taylor, SparkFun Engineering

5-27-2015

Library:
https://github.com/sparkfun/SparkFun_Line_Follower_Array_Arduino_Library
Product:
https://github.com/sparkfun/Line_Follower_Array

This sketch simply reads the data from the bar and prints it to the serial buffer.
  "SensorBar mySensorBar(SX1509_ADDRESS);" creates the sensor bar object.
  "mySensorBar.begin();" gets the bar ready.
  "mySensorBar.getDensity()" gets the number of points sensed.
  "mySensorBar.getPosition()" gets the average center of sensed points.
  
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

// Uncomment one of the four lines to match your SX1509's address
//  pin selects. SX1509 breakout defaults to [0:0] (0x3E).
const uint8_t SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
//const byte SX1509_ADDRESS = 0x3F;  // SX1509 I2C address (01)
//const byte SX1509_ADDRESS = 0x70;  // SX1509 I2C address (10)
//const byte SX1509_ADDRESS = 0x71;  // SX1509 I2C address (11)

SensorBar mySensorBar(SX1509_ADDRESS);

void setup()
{
  Serial.begin(9600);  // start serial for output
  Serial.println("Program started.");
  Serial.println();
  
  //For this demo, the IR will only be turned on during reads.
  mySensorBar.setBarStrobe();
  //Other option: Command to run all the time
  //mySensorBar.clearBarStrobe();

  //Default dark on light
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
	  while(1);
  }
  Serial.println();
  
}

void loop()
{
  //Get the data from the sensor bar and load it into the class members
  uint8_t rawValue = mySensorBar.getRaw();
  
  //Print the binary value to the serial buffer.
  Serial.print("Bin value of input: ");
  for( int i = 7; i >= 0; i-- )
  {
    Serial.print((rawValue >> i) & 0x01);
  }
  Serial.println("b");

  //Print the hex value to the serial buffer.  
  Serial.print("Hex value of bar: 0x");
  if(rawValue < 0x10) //Serial.print( , HEX) doesn't pad zeros. Do it here
  {
	  //Pad a 0;
	  Serial.print("0");
  }
  Serial.println(rawValue, HEX);
  
  //Print the position and density quantities
  Serial.print("Position (-127 to 127): ");
  Serial.println(mySensorBar.getPosition());
  Serial.print("Density, bits detected (of 8): ");
  Serial.println(mySensorBar.getDensity());
  Serial.println("");
  
  //Wait 2/3 of a second
  delay(666);

}



