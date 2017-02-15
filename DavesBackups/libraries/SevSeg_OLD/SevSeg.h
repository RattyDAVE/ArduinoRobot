//Written by Dean Reading, 2012.  deanreading@hotmail.com
//See .cpp file for info

//SevSeg Version 2.3

#ifndef SevSeg_h
#define SevSeg_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NUM_DIGITS 4

class SevSeg
{

public:
  SevSeg();

  //Public Functions
  void PrintOutput();
  void NewNum(int number_in, byte DecPlace_in); 
  void Begin(boolean mode_in, byte D1, byte D2, byte D3, byte D4, byte S1, byte S2, byte S3, byte S4, byte S5, byte S6, byte S7, byte S8);
  void Brightness(int brightness_in); //A number from 0..100
  int Brightness(); //To return the current brightness

    //Public Variables


private:
    //Private Functions
  void CreateArray();
  void FindNums();

  //Private Variables
  boolean mode,DigitOn,DigitOff,SegOn,SegOff;
  byte DigitPins[4];
  byte SegmentPins[8];
  boolean lights[NUM_DIGITS][8];
  byte nums[4];
  int number;
  byte DecPlace;
  boolean negative;
  int brightness;

};

#endif

