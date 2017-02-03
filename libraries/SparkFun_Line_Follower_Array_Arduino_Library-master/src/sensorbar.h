/******************************************************************************
sensorbar.h

SparkFun Line Follower Array Arduino Library

Marshall Taylor, SparkFun Engineering

5-27-2015

Library:
https://github.com/sparkfun/SparkFun_Line_Follower_Array_Arduino_Library
Product:
https://github.com/sparkfun/Line_Follower_Array

This is a library for reading the sensor bar's data by I2C.  It was originally
adapted from the "SparkFun SX1509 IO Expander Breakout Arduino Library" that
was written by Jim Lindblom.

Resources:
Relies on the I2C driver (wire.h).  Declaring an object of type SensorBar and
then calling .begin() causes wire.h to operate

Development environment specifics:
Tested on the RedBot 328 based arduino board.

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef SENSORBAR_H
#define SENSORBAR_H

#include "stdint.h"
//#include "Arduino.h"

#define RECEIVE_TIMEOUT_VALUE 1000	// Timeout for I2C receive

// These are used for setting LED driver to linear or log mode:
#define LINEAR		0
#define LOGARITHMIC	1

// These are used for clock config:
#define INTERNAL_CLOCK	2
#define EXTERNAL_CLOCK	1

class SensorBar
{
  public:
    //New functions for bar specific operation
    SensorBar( uint8_t address, uint8_t resetPin = 255, uint8_t interruptPin = 255, uint8_t oscillatorPin = 255 );
    uint8_t begin( void );
    uint8_t getRaw( void );
    int8_t getPosition( void );
    uint8_t getDensity( void );
	void setBarStrobe( void );
	void clearBarStrobe( void );
	void setInvertBits( void ); //Dark (no reflection) = 1, position returns center of dark
	void clearInvertBits( void ); //Light (reflection) = 1, position returns center of light

	//Functions pulled from the SX1509 driver
    void reset( void );
	void debounceConfig( uint8_t configValue );
    void debounceEnable( uint8_t pin );
    void enableInterrupt( uint8_t pin, uint8_t riseFall );
    unsigned int interruptSource( void );
    void configClock( uint8_t oscSource = 2, uint8_t oscPinFunction = 0, uint8_t oscFreqOut = 0, uint8_t oscDivider = 1 );


  private:
    //Holding variables
    uint8_t lastBarRawValue;
	uint8_t lastBarPositionValue;
	
	//Settings
	uint8_t deviceAddress; // I2C Address of SX1509
	uint8_t barStrobe; // 0 = always on, 1 = power saving by IR LED strobe
	uint8_t invertBits; // 1 = invert

    // Pin definitions:
    uint8_t pinInterrupt;
    uint8_t pinOscillator;
    uint8_t pinReset;

    // Read Functions:
    void scan( void );
    uint8_t readByte(uint8_t registerAddress);
    unsigned int readWord(uint8_t registerAddress);
    void readBytes(uint8_t firstRegisterAddress, uint8_t * destination, uint8_t length);
    // Write functions:
    void writeByte(uint8_t registerAddress, uint8_t writeValue);
    void writeWord(uint8_t registerAddress, unsigned int writeValue);
    void writeBytes(uint8_t firstRegisterAddress, uint8_t * writeArray, uint8_t length);
};

//****************************************************************************//
//
//  Circular Buffer
//
//****************************************************************************//

//Class CircularBuffer is int16_t
//Does not care about over-running real data ( if request is outside length's bounds ).
//For example, the underlying machine writes [48], [49], [0], [1] ... 

class CircularBuffer
{
public:
    CircularBuffer( uint16_t inputSize );
	~CircularBuffer();
    int16_t getElement( uint16_t ); //zero is the push location
    void pushElement( int16_t );
    int16_t averageLast( uint16_t );
    uint16_t recordLength( void );
private:
	uint16_t cBufferSize;
    int16_t *cBufferData;
    int16_t cBufferLastPtr;
    uint8_t cBufferElementsUsed;
};



#endif // SENSORBAR_H
