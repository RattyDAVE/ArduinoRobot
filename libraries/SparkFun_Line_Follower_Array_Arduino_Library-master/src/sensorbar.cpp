/******************************************************************************
sensorbar.cpp

SparkFun Line Follower Array Arduino Library

Marshall Taylor, SparkFun Engineering

5-27-2015

Library:
https://github.com/sparkfun/SparkFun_Line_Follower_Array_Arduino_Library
Product:
https://github.com/sparkfun/Line_Follower_Array

This is a library for reading the sensor bar's data by I2C.  It was originally
adapted from the "SparkFun SX1509 IO Expander Breakout Arduino Library".

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

#include "sensorbar.h"
#include "stdint.h"
#include "Wire.h"
#include "sx1509_registers.h"

//****************************************************************************//
//
//  Config and begin functions
//
//****************************************************************************//
SensorBar::SensorBar(uint8_t address, uint8_t resetPin, uint8_t interruptPin, uint8_t oscillatorPin)
{
  // Store the received parameters into member variables
  deviceAddress = address;
  pinInterrupt = interruptPin;
  pinOscillator = oscillatorPin;
  pinReset = resetPin;
  invertBits = 0;
  barStrobe = 0; //Default always on
  
}

void SensorBar::debounceConfig(uint8_t configValue)
{
  // First make sure clock is configured
  uint8_t tempuint8_t = readByte(REG_MISC);
  if ((tempuint8_t & 0x70) == 0)
  {
    tempuint8_t |= (1 << 4);	// Just default to no divider if not set
    writeByte(REG_MISC, tempuint8_t);
  }
  tempuint8_t = readByte(REG_CLOCK);
  if ((tempuint8_t & 0x60) == 0)
  {
    tempuint8_t |= (1 << 6);	// default to internal osc.
    writeByte(REG_CLOCK, tempuint8_t);
  }

  configValue &= 0b111;	// 3-bit value
  writeByte(REG_DEBOUNCE_CONFIG, configValue);
}

void SensorBar::debounceEnable(uint8_t pin)
{
  unsigned int debounceEnable = readWord(REG_DEBOUNCE_ENABLE_B);
  debounceEnable |= (1 << pin);
  writeWord(REG_DEBOUNCE_ENABLE_B, debounceEnable);
}

//Run this once during initialization to configure the SX1509 as a sensor bar
//Returns 1 for success
uint8_t SensorBar::begin(void)
{
  uint8_t returnVar = 0;
  if (pinInterrupt != 255)
  {
    pinMode(pinInterrupt, INPUT_PULLUP);
  }

  // Begin I2C
  Wire.begin();

  // Reset the SX1509
  reset();

  // Communication test. We'll read from two registers with different
  // default values to verify communication.
  unsigned int testRegisters = 0;
  testRegisters = readWord(REG_INTERRUPT_MASK_A);	// This should return 0xFF00
  // Then read a uint8_t that should be 0x00
  if (testRegisters == 0xFF00)
  {
	//Success!  Configure the device.
    writeByte(REG_DIR_A, 0xFF);
    writeByte(REG_DIR_B, 0xFC);
    writeByte(REG_DATA_B, 0x01);
	
    returnVar = 1;
  }
  else
  {
    returnVar = 0;
  }
 
  return returnVar;
}

//Do a software reset
void SensorBar::reset( void )
{
  // No hardware option, try software reset
  writeByte(REG_RESET, 0x12);
  writeByte(REG_RESET, 0x34);

}

void SensorBar::enableInterrupt(uint8_t pin, uint8_t riseFall)
{
  // Set REG_INTERRUPT_MASK
  unsigned int tempWord = readWord(REG_INTERRUPT_MASK_B);
  tempWord &= ~(1 << pin);	// 0 = event on IO will trigger interrupt
  writeWord(REG_INTERRUPT_MASK_B, tempWord);

  uint8_t sensitivity = 0;
  switch (riseFall)
  {
    case CHANGE:
      sensitivity = 0b11;
      break;
    case FALLING:
      sensitivity = 0b10;
      break;
    case RISING:
      sensitivity = 0b01;
      break;
  }

  // Set REG_SENSE_XXX
  // Sensitivity is set as follows:
  // 00: None
  // 01: Rising
  // 10: Falling
  // 11: Both
  uint8_t pinMask = (pin & 0x07) * 2;
  uint8_t senseRegister;

  // Need to select between two words. One for bank A, one for B.
  if (pin >= 8)	senseRegister = REG_SENSE_HIGH_B;
  else			senseRegister = REG_SENSE_HIGH_A;

  tempWord = readWord(senseRegister);
  tempWord &= ~(0b11 << pinMask);	// Mask out the bits we want to write
  tempWord |= (sensitivity << pinMask);	// Add our new bits
  writeWord(senseRegister, tempWord);
}

unsigned int SensorBar::interruptSource(void)
{
  unsigned int intSource = readWord(REG_INTERRUPT_SOURCE_B);
  writeWord(REG_INTERRUPT_SOURCE_B, 0xFFFF);	// Clear interrupts
  return intSource;
}

void SensorBar::configClock(uint8_t oscSource, uint8_t oscPinFunction, uint8_t oscFreqOut, uint8_t oscDivider)
{
  // RegClock constructed as follows:
  //	6:5 - Oscillator frequency souce
  //		00: off, 01: external input, 10: internal 2MHz, 1: reserved
  //	4 - OSCIO pin function
  //		0: input, 1 ouptut
  //	3:0 - Frequency of oscout pin
  //		0: LOW, 0xF: high, else fOSCOUT = FoSC/(2^(RegClock[3:0]-1))
  oscSource = (oscSource & 0b11) << 5;		// 2-bit value, bits 6:5
  oscPinFunction = (oscPinFunction & 1) << 4;	// 1-bit value bit 4
  oscFreqOut = (oscFreqOut & 0b1111);	// 4-bit value, bits 3:0
  uint8_t regClock = oscSource | oscPinFunction | oscFreqOut;
  writeByte(REG_CLOCK, regClock);

  // Config RegMisc[6:4] with oscDivider
  // 0: off, else ClkX = fOSC / (2^(RegMisc[6:4] -1))
  oscDivider = (oscDivider & 0b111) << 4;	// 3-bit value, bits 6:4
  uint8_t regMisc = readByte(REG_MISC);
  regMisc &= ~(0b111 << 4);
  regMisc |= oscDivider;
  writeByte(REG_MISC, regMisc);
}

//Call .setBarStrobing(); to only illuminate while reading line
void SensorBar::setBarStrobe( void )
{
	barStrobe = 1; //Do strobe
}

//Call .clearBarStrobing(); to illuminate all the time
void SensorBar::clearBarStrobe( void )
{
	barStrobe = 0; //Always on
}

// .setInvertBits(); to make the bar functions look for a white line on dark surface
void SensorBar::setInvertBits( void )
{
	invertBits = 1; //Do strobe
}

// .clearInvertBits(); to make the bar look for a dark line on a reflective surface
void SensorBar::clearInvertBits( void )
{
	invertBits = 0; //Always on
}

//****************************************************************************//
//
//  Bar functions
//
//****************************************************************************//

uint8_t SensorBar::getRaw( void )
{
  //Get the information from the wire, stores in lastBarRawValue
  scan();
  
  return lastBarRawValue;

}

int8_t SensorBar::getPosition( void )
{
  //Assign values to each bit, -127 to 127, sum, and divide
  int16_t accumulator = 0;
  uint8_t bitsCounted = 0;
  int16_t i;

  //Get the information from the wire, stores in lastBarRawValue
  scan();

  //count bits
  for ( i = 0; i < 8; i++ )
  {
    if ( ((lastBarRawValue >> i) & 0x01) == 1 )
    {
      bitsCounted++;
    }
  }

  //Find the vector value of each positive bit and sum
  for ( i = 7; i > 3; i-- ) //iterate negative side bits
  {
    if ( ((lastBarRawValue >> i) & 0x01) == 1 )
    {
      accumulator += ((-32 * (i - 3)) + 1);
    }
  }
  for ( i = 0; i < 4; i++ ) //iterate positive side bits
  {
    if ( ((lastBarRawValue >> i) & 0x01) == 1 )
    {
      accumulator += ((32 * (4 - i)) - 1);
    }
  }

  if ( bitsCounted > 0 )
  {
    lastBarPositionValue = accumulator / bitsCounted;
  }
  else
  {
    lastBarPositionValue = 0;
  }

  return lastBarPositionValue;
}

uint8_t SensorBar::getDensity( void )
{
  uint8_t bitsCounted = 0;
  uint8_t i;

  //get input from the I2C machine
  scan();

  //count bits
  for ( i = 0; i < 8; i++ )
  {
    if ( ((lastBarRawValue >> i) & 0x01) == 1 )
    {
      bitsCounted++;
    }
  }
  return bitsCounted;
}


//****************************************************************************//
//
//  Utilities
//
//****************************************************************************//
void SensorBar::scan( void )
{
  if( barStrobe == 1 )
  {
    writeByte(REG_DATA_B, 0x02); //Turn on IR
    delay(2);  //Additional delay required after IR is turned on to allow LEDs to achieve full brightness
    writeByte(REG_DATA_B, 0x00); //Turn on feedback
  }
  else
  {
	writeByte(REG_DATA_B, 0x00); //make sure both IR and indicators are on
  }
  //Operate the I2C machine
  lastBarRawValue = readByte( REG_DATA_A );  //Peel the data off port A
  
  if( invertBits == 1 ) //Invert the bits if needed
  {
	  lastBarRawValue ^= 0xFF;
  }
  
  if( barStrobe == 1 )
  {
	  writeByte(REG_DATA_B, 0x03); //Turn off IR and feedback when done
  }
  //delay(8);

}

// readByte(uint8_t registerAddress)
//	This function reads a single uint8_t located at the registerAddress register.
//	- deviceAddress should already be set by the constructor.
//	- Return value is the uint8_t read from registerAddress
//
//  Currently returns 0 if communication has timed out
//
uint8_t SensorBar::readByte(uint8_t registerAddress)
{
  uint8_t readValue;
  unsigned int timeout = RECEIVE_TIMEOUT_VALUE;

  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t) 1);

  while ((Wire.available() < 1) && (timeout != 0))
    timeout--;

  if (timeout == 0)
    return 0;

  readValue = Wire.read();

  return readValue;
}

// readWord(uint8_t registerAddress)
//	This function will read a two-uint8_t word beginning at registerAddress
//	- A 16-bit unsigned int will be returned.
//		- The msb of the return value will contain the value read from registerAddress
//		- The lsb of the return value will contain the value read from registerAddress + 1
unsigned int SensorBar::readWord(uint8_t registerAddress)
{
  unsigned int readValue;
  unsigned int msb, lsb;
  unsigned int timeout = RECEIVE_TIMEOUT_VALUE * 2;

  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t) 2);

  while ((Wire.available() < 2) && (timeout != 0))
    timeout--;

  if (timeout == 0)
    return 0;

  msb = (Wire.read() & 0x00FF) << 8;
  lsb = (Wire.read() & 0x00FF);
  readValue = msb | lsb;

  return readValue;
}

// readBytes(uint8_t firstRegisterAddress, uint8_t * destination, uint8_t length)
//	This function reads a series of uint8_ts incrementing from a given address
//	- firstRegsiterAddress is the first address to be read
//	- destination is an array of uint8_ts where the read values will be stored into
//	- length is the number of uint8_ts to be read
//	- No return value.
void SensorBar::readBytes(uint8_t firstRegisterAddress, uint8_t * destination, uint8_t length)
{
  uint8_t readValue;

  Wire.beginTransmission(deviceAddress);
  Wire.write(firstRegisterAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, length);

  while (Wire.available() < length)
    ;

  for (int i = 0; i < length; i++)
  {
    destination[i] = Wire.read();
  }
}

// writeByte(uint8_t registerAddress, uint8_t writeValue)
//	This function writes a single uint8_t to a single register on the SX509.
//	- writeValue is written to registerAddress
//	- deviceAddres should already be set from the constructor
//	- No return value.
void SensorBar::writeByte(uint8_t registerAddress, uint8_t writeValue)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.write(writeValue);
  Wire.endTransmission();
}

// writeWord(uint8_t registerAddress, ungisnged int writeValue)
//	This function writes a two-uint8_t word to registerAddress and registerAddress + 1
//	- the upper uint8_t of writeValue is written to registerAddress
//		- the lower uint8_t of writeValue is written to registerAddress + 1
//	- No return value.
void SensorBar::writeWord(uint8_t registerAddress, unsigned int writeValue)
{
  uint8_t msb, lsb;
  msb = ((writeValue & 0xFF00) >> 8);
  lsb = (writeValue & 0x00FF);
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.write(msb);
  Wire.write(lsb);
  Wire.endTransmission();
}

// writeBytes(uint8_t firstRegisterAddress, uint8_t * writeArray, uint8_t length)
//	This function writes an array of uint8_ts, beggining at a specific adddress
//	- firstRegisterAddress is the initial register to be written.
//		- All writes following will be at incremental register addresses.
//	- writeArray should be an array of uint8_t values to be written.
//	- length should be the number of uint8_ts to be written.
//	- no return value.
void SensorBar::writeBytes(uint8_t firstRegisterAddress, uint8_t * writeArray, uint8_t length)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(firstRegisterAddress);
  for (int i = 0; i < length; i++)
  {
    Wire.write(writeArray[i]);
  }
  Wire.endTransmission();
}


//****************************************************************************//
//
//  Circular buffer
//
//****************************************************************************//

//Construct a CircularBuffer type with arguments
//  uint16_t inputSize: number of elements
CircularBuffer::CircularBuffer(uint16_t inputSize)
{
	cBufferData = new int16_t[inputSize];
	cBufferLastPtr = 0;
	cBufferElementsUsed = 0;  
	cBufferSize = inputSize;
	
}

CircularBuffer::~CircularBuffer()
{
	delete[] cBufferData;

}

//Get an element at some depth into the circular buffer
//zero is the push location.  Max is cBufferSize - 1
//
//Arguments:
//  uint16_t elementNum: number of element in
//
int16_t CircularBuffer::getElement( uint16_t elementNum )
{
  //Translate elementNum into terms of cBufferLastPtr.
  int16_t virtualElementNum;
  virtualElementNum = cBufferLastPtr - elementNum;
  if( virtualElementNum < 0 )
  {
    virtualElementNum += cBufferSize;
  }
  
  //Output the value
  return cBufferData[virtualElementNum];
}

//Put a new element into the buffer.
//This also expands the size up to the max size
//Arguments:
//
//  int16_t elementVal: value of new element
//
void CircularBuffer::pushElement( int16_t elementVal )
{
  //inc. the pointer
  cBufferLastPtr++;

  //deal with roll
  if( cBufferLastPtr >= cBufferSize )
  {
    cBufferLastPtr = 0;
  }

  //write data
  cBufferData[cBufferLastPtr] = elementVal;

  //increase length up to cBufferSize
  if( cBufferElementsUsed < cBufferSize )
  {
    cBufferElementsUsed++;
  }
}

//Averages the last n numbers and provides that.  Discards fractions
int16_t CircularBuffer::averageLast( uint16_t numElements )
{
  //Add up all the elements
  int32_t accumulator = 0;
  int8_t i;
  for( i = 0; i < numElements; i++ )
  {
    accumulator += getElement( i );
  }
  //Divide by number of elements
  accumulator /= numElements;
  return accumulator;
}

//Returns the current size of the buffer
uint16_t CircularBuffer::recordLength( void )
{
  return cBufferElementsUsed;
}

