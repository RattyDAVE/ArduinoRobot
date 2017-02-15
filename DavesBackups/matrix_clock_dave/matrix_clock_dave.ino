   /*****************************************************************
   *                       Matrix Clock                             *
   *                          V0.1                                  *
   *                                                                *
   *                      By Dan J Hamer                            *
   *                          2014                                  *
   *                   http://carbonfrog.com                        *
   *                                                                *
   *        This work is licensed under a Creative Commons          *
   *        Attribution-ShareAlike 4.0 International License.       *
   *        http://creativecommons.org/licenses/by-sa/4.0/          *
   *****************************************************************/

//#include <avr/pgmspace.h>        // Needed for Sleep functions.

//#include <avr/sleep.h>           // Sleep functions.

//#include <arduino.h>             // Standard library.

#include <EEPROM.h>              // EEPROM library.

//#include <Wire.h>                // Needed for comunication with Real Time Clock.

//#include <DS1307RTC.h>           // Real Time Clock library.

#include <Time.h>                // Time functions library.

//#include "TLC5920.h"             // TLC5920 driver library.

//#include "Characters.h"          // Character graphics for letters, numbers and small numbers.

//#include "Tunes.h"               // Musical notes and alarm tunes.


#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = D4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 8;
int numberOfVerticalDisplays = 1;

Max72xxPanel myMatrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
//TLC5920 myMatrix;                // Create instance of TLC5920 class.

#define DEBUG  0                 // If in debug mode Serial comunication will be initiated and
                                 // various serial mesagges variables and features will be
                                 // implimented to help with debugging.

  /***************************************************************************
  *                                                                          *
  *                      Snooze/Settings(hold down for 2 secs)               *
  *                                        S1                                *
  *                                        []                                *
  *     Up/ Alarm on-off    0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0    Mode         *
  *                   S3 [] 0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0 [] S0           *
  *                         0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0                 *
  *                         0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0                 *
  *                 Down    0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0                 *
  *                   S2 [] 0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0                 *
  *                         0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0                 *
  *                         0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0                 *
  *                                                                          *
  ***************************************************************************/

//#define S0  A0                   // Button 1.
//#define S1  A1                   // Button 2.
//#define S2  A2                   // Button 3.
//#define S3  A3                   // Button 4.
//#define BUZZERPIN  7             // Output pin for Piezo sounder.
//#define EXBLINK  13              // External LED seconds blinker.
#define BLINKTYPE 1              // Blinker Type, 0 = External LED, 1 = On matrix.
#define HOUR12 true              // For time format.
#define HOUR24 false
//#define SLEEPPIN  3              // SLEEP pin, pulling LOW puts the device in sleep mode, release to wake up.
//#define SNOOZETIME 5             // Snooze time in minutes.    

byte oldMinutes = 61;            // Record of minutes to compare against current minutes. (61 forces reset)
byte currentMinutes = 0;         // Temporary holder of current minutes for animations to use.

 
unsigned long currentMillis;     // Keeps track of millisecond time. Used by various functions.
unsigned long FrameTime = 0;     // Keeps track of timeing between animation frames.
unsigned int  FrameRate = 0;     // Animation frame rate.
unsigned int frameCount = 0;     // Keeps counts of the number of frames since the begining of current transition.
boolean alarmActive = false;     // Status for alarm  
boolean alarmTrigger = false;    // Trigger for alarm When alarmActive alarmTrigger and true 
                                 // Alarm will sound.
byte alarmTone = 0;              // Chosen alarm tune.
unsigned long toggleTimer;       // Timer for toggle display

unsigned long debounce;          // Button debounce timer.
unsigned long holdTimer;         // Timer for functions that requrire a button to be held down 
                                 // for a minimum amount of time.
byte clockMode = 0;              // Current mode indicator.
byte saveClockMode = 0;          // Saves clock mode for when in a Control mode ie. alarmToggle   
boolean intro = true;            // Indicates if mode has just been entered after mode change.

typedef struct                   // Alarm data struct
{
  byte  hour;                    // hour
  byte  minute;                  // minutes
  byte  snoozeMinute;            // snooze time minute
  byte  snoozeHour;              // snooze time hour
  byte  melody;                  // Melody type
}  time_alarmSet;                   
time_alarmSet alarmTime;         // Create alarmTime struct

byte tunePosition = 0;           // Keeps track of tune possition while playing musical alarms.

// Button data arrays
boolean buttonPressed [4] = {false, false, false, false } ;  
boolean buttonLatch [4] = {false, false, false, false } ;
boolean buttonDown [4] = {false, false, false, false } ;

boolean halfSeconds = false;    // For seconds flashing

time_t currentTime;             // A temporary value (unsigned long) holder the current time in UNIX time,
                                // ie. the number of seconds passed since the 1st of January 1970.
boolean format = true;          // True = 12 hour format, false = 24 hour format

byte matrixBuffer [3] [8] [16]; // A 3 layer display buffer for the creation of various graphical effects


typedef struct                  // Traces are sprite like graphical elements that can be
{                               // manipulated in a variety of ways.
  boolean active;               // Active or inactive.
  int regen;                    // Frame when the trace will regenerate.
  byte xPos;                    // X position
  byte yPos;                    // Y position 
  byte speed;                   // Speed
  byte length;                  // Length
  byte terminate;               // Termination point, Should be at least 8 + Length
}  trace;
trace streamerTrace[16];        // 16 traces in struct array for streamer animations

byte field = 1;                 // Divides frames into fields to allow traces to move at different speeds 

                                // Messages
byte scrollPosition = 255;      // Message scroll position 255 indicates there is no message scrolling.
char msgSet [] = "Settings";
char msgTim [] = "Set_Time";    // note: '_' is a blank space.
char msgFrm [] = "Set_Time_Format";
char msgDat [] = "Set_Date";
char msgYea [] = "Set_Year";
char msgAlm [] = "Set_Alarm";

//----------------------------------------



void setup() 
{
//  pinMode(BUZZERPIN, OUTPUT);          // Setup Piezo pin.
//  pinMode(EXBLINK, OUTPUT);            // Setup LED seconds flasher pin.
  analogReference(DEFAULT);            // 5 volt reference
//  pinMode(S0, INPUT_PULLUP);           // Button inputs with pullups
//  pinMode(S1, INPUT_PULLUP);
//  pinMode(S2, INPUT_PULLUP);
//  pinMode(S3, INPUT_PULLUP);
  initialiseStreamerTraces();         // Initialises all the traces for streamer animation.
  debounce = 0;                       // Initialise debounce timer, ie millis debounce end time.
  FrameRate = 100;                    // Animation frame rate changed by animation functions.
//  myMatrix.startup();                 // Initialises the TLC5920 driver

#if DEBUG  
  Serial.begin(9600);                 // Start serial comunication.
  while (!Serial) {
    ;                                 // wait for serial port to connect. Needed for Leonardo only
  }
  delay(5000);
#endif
  while (timeStatus()!= timeSet)   
  {
//    setSyncProvider(RTC.get);          // the function to get the time from the RTC
    if (timeStatus()!= timeSet)        // if cominication with the RTC fails
    {
#if DEBUG
      Serial.print(timeSet);
      Serial.println("Unable to sync with the RTC");  // Prints if unable to sync with RTC
#endif   
      setTime(12,00,00,12,01,2013);    // If Clock fails to sync with the DS1307 then set the time
      currentTime = now();             // to 12:00:00 12th of January 2013
//      RTC.set(currentTime);            // attempt to sync again.
    }
#if DEBUG 
    if(timeStatus()== timeSet)
    {
      Serial.print(timeSet);            // Prints code and message to confirm sucssesful sync with DS1307
      Serial.println("sync with the RTC OK");
    }
#endif   
  }
  setSyncInterval(3600);                    // Sync with DS1307 every 30 seconds.
//  pinMode(SLEEPPIN, INPUT_PULLUP);        // Setup sleep pin as an input with pullup.
  attachInterrupt(0, wakeUpNow, RISING);  // Set up interrupt to run wakeUpNow when Sleep pin goes high.
  blinker(false);                         // Deactivate seconds blinker.
  restoreAlarmTime();                     // Get stored alarm from EEPROM.
  blankLayer(2);                          // Clear random garbage from buffer layer 2.
}

void loop() 
{    

  currentMillis = millis();  // Keeps track of millisecond time. Used by various functions.

  ButtonScan();              // Poll all the buttons

  MultiButtonHandler();      // Handles any buttons that might have been pressed

  if( currentMillis >= FrameTime + FrameRate)
  {                          // Runs animation function every frame time
    FrameTime = currentMillis;
    currentMinutes = minute();             // Get current minutes for animations to use.

    switch (clockMode)       // Runs current animation
    {
    case 0:
      streamerTime();        // Traces stream down the display wipeing out the old time and
      break;                 // pasting on the new time every minute.

    case 1:              
      swooperTime();         // Bar sweeps back and forth across the display wipeing out 
      break;                 // the old time and pasting on the new time every minute.

    case 2:
      RandomDotTime();       // Random dots appear all over the disply disolving the old
      break;                 // time and dipositing the new time.

    case 3:
      DayProgressBar();      // Progress bar for the day. Grows accross the display, very
      break;                 // Slowly through out the day.

    case 4:       
      displaySTime();        // Displays the time in hours, minutes, and seconds in
      break;                 // small characters.

    case 5:
      displayTime(HOUR24);   // Displays the time in hours and minutes in a
      break;                 // 24hour format.

    case 6:
      streamerAnimation();   // Just streamer traces animation, no time.
      break;

    case 7:
      displayTime(format);   // Basic time display in hours and minutes.
      break;
      
      
// All modes above 250 are for settings and control-----------------------------------------------------------


    case 254:                
      alarmToggle();        // When the alarm is toggled on or off this function will
      break;                // toggle alarm actrive/inactive and sroll message across display.

    case 255:
      Settings();           // Enters settings mode.
      break;

    default:
      clockMode = 0;        // Returns mode to first clock animation.
    }        
  }
  alarm();                 // services alarm
//  myMatrix.refresh();      // Refreshes the matrix display with what ever is in the matrix array.    
  myMatrix.write();      // Refreshes the matrix display with what ever is in the matrix array.    

}
//------------------------------------------------------------------------------------------------------------




// animations and clocks--------------------------------------------------------------------------------------



void streamerTime()                      // StreamerIime clock animation. Transitions from one minute to the
{                                        // next in a 'Matrix' style animation.
  byte finishedDots = 0;                 // During animation this byte keeps track of the number of dots merged
  static byte totalDots = 0;             // Keeps track of the number of dots to be merged.
  static boolean bypass = false;         // If between minutes this bol signifies no animation needed.
  static boolean wipePaste = 0;          // wipe = 0, paste = 1
  static byte hinder = 0;                // Counter for delay between animation stages ( wipe, paste, finish).
  static boolean runoff = 0;             // Flag to allows traces to continue till there all gone but not make any more.
  static unsigned int iteration = 70;    // functions call iteration
  static unsigned int swell = 150 ;      // Decreases then increases duing animation to create swelling effect.

  if ( currentMinutes != oldMinutes || intro == true)
  {                                    // once a minute or intro
    FrameRate = 20;                    // Set frame rate
    bypass = false;                    // Turn off bypass so animation runs
    intro = false;                     // Trun off intro as animation is now active
    runoff = 0 ;                       // Clear runoff ( 1 indictes in a runoff).
    swell = 150 ;                      // Creates a gradual increase in the number of traces on the display.
    hinder = 30;                       // delay between wiping and pasting
    blankLayer(0);                     // Blank buffer layer 0
    totalDots = compileNumbers();      // compileNumbers compiles the time numbers into matrixBuffer 0 and 
                                       // returns the total number of dots.
    frameCount = 0;                    // Reset frame count.
    oldMinutes = currentMinutes;       // Update old minutes for next time minutes change.
    initialiseStreamerTraces();
  } 


  if(bypass != true)                   // Run animation if bypass not activated
  {
    blankLayer(1);                     // Clears the buffer ready to generate the next frame
    frameCount++;                      // Increment frame count.

    if( swell > 1 && wipePaste == 0)  // When swell is large less traces per second will be generated
    {                                 // When in wipe mode (begining),
    swell--;                          // decrement swell to accelerate trace generation frame by frame.
    }
    else if(swell < 151 && wipePaste == 1 )
    {                                 // When in paste mode
      swell++;                        // increment swell to ramp down trace generation.
    }
    
  drawTraces( 1, field, swell, runoff);  // Draw current frame of traces to a buffer layer based on 
                                         // speed, then terminate finished traces and regenerate new traces
                                         // based on current frame count and swell, if not in runoff.
    field++;                             // Increment field counter,
    if ( field >= 7)                     // when fiel counter reaches 7,
    {
      field = 1;                         // reset to 1.
    }

    finishedDots = mergeLayers(wipePaste);      // mergeLayer combines matrixBuffer layers 0 &  1 into 2 and displays 2, 
                                                // returns the number of layer 2 dots at 4
    if(finishedDots == 0 && wipePaste == 0)     // When true, wipe stage has finished but animation will continue another
    {                                           // 50 times before continuing to paste stage 
      if (hinder == 0)
      {          
        wipePaste = 1;      // Put animation in paste mode
        hinder = 30;        // Delay between pasting and runoff, ready for end
      }
      else
      {
        hinder--;            // Hinder countdown (during wipe to paste stage).
      }
    }
    if(finishedDots >= totalDots && wipePaste == 1 && runoff == 0)
    {
      if (hinder == 0)        // delay (hinder) between paste and runoff has finished
      {          
        runoff = 1;           // Set to indicate that animation is now in runoff stage.
        hinder = 30;          // Delay between runoff and normal running
      }
      else
      {
        hinder--;              // Hinder countdown (during paste to runoff stage).
      }          
    }

    if(runoff == 1)            // If true, now in runoff stage
    {
      if(layerDotCount(1) == 0 )
      {
        if(hinder == 10)       // Animation transition now complete
        {
          initialiseStreamerTraces();         // Initialises all the traces.
        }        
        
        
        if(hinder == 0)       // Animation transition now complete
        {
          blankLayer(1);      // Clear effect from bufferLayer 1.
          mergeLayers(2);     // Display the time that is now in bufferLayer 2.
          wipePaste = 0;      // Reset wipePaste ready for next time the animation runs.
          bypass = true;      // Set bypass so this function just leaves the display 
//          initialiseStreamerTraces();         // Initialises all the traces.
        }                     // and only runs the seconds blinker.
        else
        {
          hinder--;            // Hinder countdown (during runoff to normal running stage).
        }
      }  
    }
  }                            // End of NOT bypass loop.
  blinker(bypass);             // Run blinker if in bypass mode (true), and not if in animation mode (false).
  return;                      // Return to main program loop.
}



void swooperTime()                            // SwooperTime is a clock animation. It makes a transition between one minute and the next 
{                                             // by first wiping the time off with a bar that swoops from side to side and then
                                              // pastes on the new time with a similar swooping bar.
  FrameRate = 50;                             // Set animation frame rate.
  static boolean bypass = true;               // If between minutes this bol signifies no animation needed. 
  byte e, r, shade;                           // Loop countes and pixel shade.
  static byte iteration = 0;                  // This functions call iteration.
  static byte leadingEdge = 0;                // Current position of the wiping/pasting edge of the swooper.
  byte gradient = 0;                          // To calculate the fading of pixel shades trailing the leading edge.

  if ( (currentMinutes != oldMinutes || intro == true) && bypass == true)    
  {                                           // Once a minute or intro.
    bypass = false;                           // Deactivate bypass.
    intro = false;                            // Deactivat intro.
    compileNumbers();                         // compileNumbers compiles the time numbers into matrixBuffer 0.
    oldMinutes = currentMinutes;              // Update oldMinutes ready for next time the minutes change.
  }
  if(bypass != true)                          // Animation running.
  {   
    for(e = 0; e < 16; e++)                   // Loop to generate graphic of swooper bar.
    {
      if( iteration < 23 || ( iteration > 41 && iteration < 61 ) )
      {                                       // If going right then draw bar with gradient to the left
        gradient = leadingEdge - (e + 5) ;    // by subtracting current row, + 5 for dead zone, from leading edge position. 
      }
      else if ( ( iteration > 22 && iteration < 42 ) || iteration > 60 )
      {                                       // If going left then draw bar with gradient to the right
        gradient = (e + 5) - leadingEdge;     // by subtracting leading edge position from current row, + 5 for dead zone.
      }

      switch (gradient)                       // Get the right pixel shade depening on the current gradient calculated.
      {
      case 0:
        shade = 5;                            // A shade of 5 will wipe or paste pixels in buffer layer 0 when mergeLayers is called.
        break;
      case 1:
        shade = 4;
        break;     
      case 2:
        shade = 4;
        break;    
      case 3:
        shade = 3;
        break;   
      case 4:
        shade = 2;
        break;     
      case 5:
        shade = 1;
        break;              
      default: 
        shade = 0;
      }    
      for(r = 0; r < 8; r++)           // Loop to make vertical line of current shade.
      {
        matrixBuffer [1] [r] [e] = shade;     // Put in matrix buffer layer 1.
      }   
    }
    if ( iteration < 42 )              // During first half of animation the swooper will wipes the display
    {
      mergeLayers(0);      // mergeLayer mode 0, wipes graphic in matrixBuffer layer 2 with layer 1
    }
    if ( iteration > 41 )              // During second half of animation the swooper will paste the time on to the display.
    {
      mergeLayers(1);      // mergeLayer mode 1, pastes graphic in matrixBuffer layer 0 with layer 1
    }

    if (iteration > 84)                // It takes 84 calls to this function to complete the animated transition.
    {
      iteration = 0;                   // Reset iteration if upper limit reached,
      leadingEdge = 0;                 // Reset leading edge position,
      bypass = true;                   // and activate bypass.
    }   
    else                               // If still animating,
    {
      iteration++;                     // increment iterations.
      if(iteration <  23 || ( iteration > 41 && iteration < 61 ))      
      {                                // Foward swoop direction during wipe and then paste phases.
        leadingEdge++;                 // Increment leading edge position.
      }
      else if(( iteration > 22 && iteration < 42 )|| iteration > 60 )
      {                                // Reverse swoop direction during wipe and then paste phases
        leadingEdge--;                 // Decrement leading edge position.
      }
    }
  }   
  blinker(bypass);                     // Run blinker if in bypass mode (true), and not if in animation mode (false).
  return;                              // Return to main program loop.
}


void RandomDotTime()                   // RandomDotTime is a clock animation. It transistions from one minute to the next with
{                                      // a series of random pixel paterns that start out dim but build to wipe out the current
                                       // time display and then builds the new time display and then subsides leaving the time.
  byte finishedDots = 0;               // Keeps track of dots wiped or pasted                                  
  static byte totalDots = 0;           // Numer of dots that need wiping or pasteing
  static boolean bypass = false;       // If between minutes this bol signifies no animation needed. 
  static boolean wipePaste = 0;        // wipe = 0, paste = 1
  static byte level = 8;               // Counter for build and subsiding of effect
  static byte shadeLimit = 2;          // Limits maximum pixel shade relative to level                    

  FrameRate = 20;                      // Set frame rate.
  if ( currentMinutes != oldMinutes || intro == true)
  {                                    // once a minute or intro.
    blankLayer(1);                     // Blank buffer layer 1.
    level = 8;                         // Reset effect level.
    bypass = false;                    // Turn off bypass so animation runs.
    intro = false;                     // Trun off intro as animation is now active.
    totalDots = compileNumbers();      // compileNumbers compiles the time numbers into 
                                       // matrixBuffer layer 0 and returns the total number of dots.
    oldMinutes = currentMinutes;       // Update old minutes for next time minutes change.      
    FrameRate = 20;                    // Set frame rate.
  } 
  if(bypass != true )
  {
    randomPattern(1,shadeLimit);      // Puts a new random pattern in display buffer 1 (matrixBuffer [1])
                                      // If a pixel shade is less than 4 then it will not 
                                      // wipe or paste anthing from display buffer 0.
    finishedDots = mergeLayers(wipePaste);   
                                      // mergeBuffs combines matrixBuffer 1 & 2, returns the number of dots at 5
    if ( wipePaste == 0)              // In wipe phase the level will keep increasing till a maximum is reached.
    {                                 
      if ( level < 24 )
      {
        level++;                      // Increment level.
        shadeLimit = level/4;         // Get relative shade limit.
      }
      if(finishedDots == 0 && level >= 24)
      {                               // When all dots have been wiped and maximium effect level reached,
        wipePaste = 1;                // advance to paste phase.
      }
    }

    if ( wipePaste == 1)              // In paste phase the level will 
    {                                 // keep increasing till a maximum is reached.
      if ( level < 48  && finishedDots < totalDots)
      {
        level++;                      // Increment level.
      }              
      if (finishedDots >= totalDots)  // When all dots have been pasted and maximium effect level reached,
      {                               // start decreasing the effect level.
        if ( level > 8 )
        {
          level--;                    // Decrement level.
          shadeLimit = level/8;       // Get relative shade limit.
        }            
        if(level <= 8 )               // When level reaches lowest then end animation.
        {            
          bypass = true;              // Reactivate bypass for normal time display.
          mergeLayers(2);             // Copy just the new time to the display.
          wipePaste = 0;              // Put in wipe mode ready for next minute transition.
        }
      }
    }      
  }
  blinker(bypass);                    // Run blinker if in bypass mode (true), and not if in animation mode (false).
  return;                             // Return to main program loop.
}




void DayProgressBar()                // Displays the time of day as a progress bar.
{
  byte x=0;                          // x counter.
  byte cx, cy;                       // X&Y counters.
  byte temp[8] [16] = {              // Temporary array for rendering progress bar.
    0  };
  //  {
  //    { 2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,},
  //    { 2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,},
  //    { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 },
  //    { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 },
  //    { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 },
  //    { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 },
  //    { 2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,},
  //    { 2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,}
  // };
  long totalTimeSeconds = 0;         // Result for total seconds passed in the day.
  byte barShade, bar, barPosition;   // BarShade 1 to 4 for bar end, bar place on 55 long bar, 
                                     // Barposition place on actual 14 pixel long bar.  
  totalTimeSeconds = (((long)hour() * 3600) + ((long)minute() * 60) + (long)(second())); //Calculate seconds
  bar = map(totalTimeSeconds, 0, 86400, 1, 55); // Map current second passed to 55 long bar.
  barPosition = (bar/4)+1;           // Calculate 14 pixel bar position (add 1 to compensate for zero)

  for( x = 1; x < barPosition; x++)   // Draw all full shade pixles of bar (to the right)
  {
    temp [3] [x] = 4;
    temp [4] [x] = 4;
  }
  barShade = (bar % 4);                // Mod bar to get bar end shade
  temp [3] [barPosition] = barShade;   // Apply shade to end of bar.
  temp [4] [barPosition] = barShade;
  for( cx = 0; cx < 16; cx++)          // Render progress bar to display.
  {
    for( cy = 0; cy < 8; cy++)
    {
      myMatrix.matrix [cy] [cx] = temp [cy] [cx];
    }
  }
  return;                              // Return to main program loop.
}



void displayTime(boolean format)              // Simple time display in 12 or 24 hour format
{
  byte digit1,digit2,digit3,digit4;           // Values for each of the 4 time digits.
  blinker(true);                              // Run blinker, seconds flasher.

  if( format == HOUR12)                       // Display time in 12 hour format.
  {
    digit2 = (hourFormat12() % 10);           // Calculate mod of hours ( just minutes' units), calculated first as needed in next calculation.
    digit1 = ((hourFormat12() - digit2)/10);  // Calculate hours' tens as unit.
    if(digit1 == 0)                           // For 12 hour format if tens of hour is zero a blank should be displayed.
    {                                        
      digit1 = 10;                            // A digit value of 10 is a blank character.
    }
    displayNumber( digit1, 0, 1);             // Display hours' tens in digit 1.
    displayNumber( digit2, 4-BLINKTYPE, 1);   // Display hours' units in digit 2, if the BLINKTYPE is using matrix pixels instead of LEDs
  }                                           // then subtracting the BLINKTYPE will move the hour units one pixle to the left to create better kerning.
  else if ( format == HOUR24 )                // Display time in 24 hour format.
  {
    digit2 = (hour() % 10);                   // Calculate mod of hours ( just minutes' units).
    digit1 = ((hour() - digit2)/10);          // Calculate hours' tens as unit.
    displayNumber( digit1, 0, 1);             // Display hours' tens in digit 1.
    displayNumber( digit2, 4-BLINKTYPE, 1);   // Display hours' units in digit 2.
  }

  digit4 = minute() % 10;                     // Calculate mod of minutes ( just minutes' units).
  digit3 = ((minute() - digit4)/10);          // Calculate minutes' tens as unit.

  displayNumber( digit3, 9, 1);               // Display minutes' tens in digit 3.
  displayNumber( digit4, 13, 1);              // Display minutes' units in digit 4.
  return;                                     // Return to main program loop. 
}  



void displaySTime()                           // Simple time display in hours minutes and seconds using small characters
{
  byte digit1,digit2,digit3,digit4,digit5,digit6;  // Values for each of the 6 time digits.
  blinker(false);                             // Run blinker, seconds flasher. In false mode blinker will not flash any seconds,
                                              // but will generate the halfSeconds needed by this functions own seconds flasher.
  digit2 = (hourFormat12() % 10);             // Calculate mod of hours ( just minutes' units), calculated first as needed in next calculation.
  digit1 = ((hourFormat12() - digit2)/10);    // Calculate hours' tens as unit.
  if(digit1 == 0)                             // For 12 hour format if tens of hour is zero a blank should be displayed.
  {
    digit1 = 10;                              // A digit value of 10 is a blank character.
  }
  digit4 = minute() % 10;                     // Calculate mod of minutes ( just minutes' units).
  digit3 = ((minute() - digit4)/10);          // Calculate minutes' tens as unit.
  digit6 = second() % 10;                     // Calculate mod of seconds ( just minutes' units).
  digit5 = ((second() - digit6)/10);          // Calculate seconds' tens as unit.

  displaySmallNumber( digit1, 0, 1);          // Display hours' tens in digit 1.
  displaySmallNumber( digit2, 2, 1);          // Display hours' units in digit 2.
  displaySmallNumber( digit3, 5, 1);          // Display minutes' tens in digit 3.
  displaySmallNumber( digit4, 8, 1);          // Display minutes' units in digit 4.
  displaySmallNumber( digit5, 11, 1);         // Display seconds' tens in digit 5.
  displaySmallNumber( digit6, 14, 1);         // Display seconds' units in digit 6.

  if(halfSeconds == false)                    // Flash seconds on 2 matrix pixels
  {
    myMatrix.matrix[6] [4] = 0;               // Pixels off.
    myMatrix.matrix[6] [10] = 0;
  }
  else
  {
    myMatrix.matrix[6] [4] = 4;               // Pixels on.
    myMatrix.matrix[6] [10] = 4;
  }
  return;                            // Return to main program loop. 
}  



void streamerAnimation()
{
  if ( intro )
  {                                  // Runs once at intro.
    blinker(false);                  // Turn off seconds blinker
    FrameRate = 20;                  // Set frame rate
    intro = false;                   // Trun off intro as animation is now active
    frameCount = 0;                  // Reset frame count.
    initialiseStreamerTraces();      // Initialises traces
  }  
  frameCount++;                      // Increment frame count.
  blankLayer(2);                     // clears the buffer ready to generate the next frame 
  drawTraces( 2, field, 150, 0);     // Draw current frame of traces to a buffer layer based on 
                                     // speed, then terminate finished traces and regenerate new traces.  
    field++;                         // Increment field counter,
    if ( field >= 7)                 // when field counter reaches 7,
    {
      field = 1;                     // reset to 1.
    }
  copyLayer(2);                      // Copies Buffer layer 2 to display
  return;                            // Return to main program loop.
}



// Graphics utilitys------------------------------------------------------------------------------------------




boolean ScrollMessage(char *messageString, byte stringSize) // scrolls a message across the display
{
  char *pr = messageString;                // Pointer to the message string
  byte c;                                  // A for loop counter for the message length
  byte displayX = 4;                       // Pixel 'X' position for character.
  byte virtualDisplayX = 4;                // Virtual pixel 'X' position for character ( with dead zones).
  byte DisplayY = 7;                       // Pixel 'Y' position for character.
  char character = 1;                      // Current Message character ( ASCII..ish ).
  byte messageWidth = (stringSize-1)*4;    // Width of the message in pixels, (-1 for string end)
                                           // ( *4, 3 pixles for character + 1 pixel for kerning).

  if(scrollPosition == 255)                // ScrollPosition, pixel by pixel position in a virtual position
  {                                        // that includes dead zones at either end for nice scrolling on and off the screen
    scrollPosition = 20 + messageWidth;    // 255 indicates a finished scroll, so set it to a virtual position off the display.
  }
  myMatrix.clear();                        // Clears display

  virtualDisplayX = scrollPosition;                  

  for(c = 0; c < stringSize ; c++)          // Characterwise loop.
  {  
    character = *pr;                        // Get next ASCII character value.     
    if(character == '\0')                   // End loop if end of string encountered.
    {
      break;                                 
    }  
    if( virtualDisplayX < (20 + messageWidth) && virtualDisplayX > messageWidth )   // Only try to render characters
    {                                                                               // that fall within the display area
      displayX = virtualDisplayX - messageWidth;        // Calculate real position of current character in X.
      displayLetter(character, displayX, DisplayY);     // Render character to display (will automaticly clip pixels).
    }
    virtualDisplayX = virtualDisplayX + 4;              // Increment virtualDisplayX to next letters position
    pr++;                                               // Increment character pointer.
  }
  if (scrollPosition == 0)                              // When scroll Position hits zero the entire message will
  {                                                     // have scrolled on and entirely off the display
    scrollPosition = 255;                               // Rest scroll position to default
    intro = true;                                       // Set intro for animation integration.
    return false;                                       // Return false to indicate to calling function that the 
  }                                                     // message has finished being scrolled.
  else
  {
    scrollPosition--;                                   // If still srolling decrement scrollPosition to move
                                                        // message one pixel to the left.
    return true;                                        // Return true to indicate to calling function that the 
  }                                                     // message is still being scrolled.
}


byte mergeLayers(byte mode )  // mergeBuffs combines matrixBuffer 0 & 1
// mode 0 = wipe clear : mode 1 = paste on : mode 2 = just put time to display
// time should be in layer 0 , effect should be in layer 1 , current display should be in layer 2
{
  byte cx, cy;                        // X&Y loop counters.
  byte dotCount = 0;                  // Keeps track of the number of dots that have be merged so calling
                                      // functions can calculate when an animation effect should finish.
  for( cx = 0; cx < 16; cx++)
  {                                   // These two loops scans round 8 x 16 to evaluate and 
    for( cy = 0; cy < 8; cy++)        // modify the buffer and display.
    {
      if ( mode == 0)                 // In wipe mode the calling animation effect wipes the display.
      { 
        if (matrixBuffer [1] [cy] [cx] == 5 )      // A value of 5 indicates that an effect should affect the display.
        {
          matrixBuffer [2] [cy] [cx] = 0;          // In wipe mode the layer to be displayed next will be blank
          myMatrix.matrix [cy] [cx] = 4;           // but the display pixel will be 4 so as to show the animation effect.
        }       
        if (matrixBuffer [2] [cy] [cx] > 0)        // If there is something in this pixel currently then
        {                                          // transfer it to the display
          myMatrix.matrix [cy] [cx] = matrixBuffer [2] [cy] [cx]; 
          dotCount++;                              // and add one to the dot count so the calling function can compare it.
        } 
        else
        {                                          // If there is nothing in the displays buffer then just displat the effect.
          myMatrix.matrix [cy] [cx] = matrixBuffer [1] [cy] [cx];
        }
      }

      if ( mode == 1)                  // In paste mode the calling animation effect will paste what ever is in layer 0 to
      {                                // layer 2 with the effect rendering in layer 1.
        if (matrixBuffer [1] [cy] [cx] == 5 && matrixBuffer [0] [cy] [cx] == 4)
        {                              // A value of 5 indicates that an effect should allow layer 0 to be copyied to layer 2 and displayed.
          myMatrix.matrix [cy] [cx] = matrixBuffer [2] [cy] [cx] = matrixBuffer [0] [cy] [cx];
        }
        if (matrixBuffer [2] [cy] [cx] >= 4 ) // If somthing is already in layer 2 then copy to display regardless.
        { 
          myMatrix.matrix [cy] [cx] = matrixBuffer [2] [cy] [cx];
        }
        else                            // If nothing is pasted or already exsists in layer 2 then just display effect in layer 1.
        {
          myMatrix.matrix [cy] [cx] = matrixBuffer [1] [cy] [cx];
        }
      }


      if ( mode == 2)                  // Mode 2 just displays what ever is in layer 2 and calculates dot count to be returned.
      {
        myMatrix.matrix [cy] [cx] = matrixBuffer [0] [cy] [cx];
      }                                 // Copy pixel from buffer layer 0 to matrix display.
      if ( matrixBuffer [2] [cy] [cx] == 4)
      {                                 // If the resulting pixel in layer 2 is shade 4,
        dotCount++;                     // increment dot count.
      }
    }
  }
  return dotCount;                     // Return total number of dots completed.
}



void copyLayer(byte layer)      // Copies Buffer layer to display
{
  byte e, r;                    // Loop counters
  myMatrix.clear();
  for(e = 0; e < 16; e++)       // 'X' loop, columns.
  {
    for(r = 0; r < 8; r++)      // 'Y' loop, pixels in current column. 
    {
      myMatrix.matrix [r] [e] = matrixBuffer [layer] [r] [e];
    }                           // Copy pixel from buffer layer to matrix display.
  }
  return;
} 



void clipDraw(byte buf, byte cdy, byte cdx, byte shade)
{              // constrains array dimentions to with in limits, if outside limits it dosn't draw them to buffer.
  if ( (cdy >= 0 && cdy <=7 ) && ( cdx >= 0 && cdx <= 15))
  {
    matrixBuffer [buf] [cdy] [cdx] = shade;
  }
  return;
}



void randomPattern(byte layer, byte level)     // Fills a buffer layer with a random pattern
{                                              // effect is resource hungry so is split in to 4 fields
  static boolean field1 = 0;        // to spread the processing load.
  static boolean field2 = 0;
  byte e, r, randomShade ;          // Loop counters and temporary random value holder

  for(e = 0; e < 7 ; e += 2)        // 'Y' loop, increments by 2, this is to skip every other pixel row.      
  {    
    for(r = 0; r < 15; r += 2)      // 'X' loop, increments by 2, this is to skip every other pixel.
    {
      randomShade = random(0,level);           // Generate random pixel shade.
      matrixBuffer [layer] [e + field1] [r + field2] = randomShade;
    }                               // Apply shade to every other pixel in every other row,
  }                                 // Depending on which field function is currently in.
  if (field1)                       // Every other call of this function.
  {
    field2 = !field2;               // Only toggled ever other call creating a total of 4 fields.
  }
  field1 = !field1;                 // Toggled every call.
  return;                           // Return to calling function.
}



void blankLayer( byte layer )      // Blanks a layer in the display buffer
{                                  // layers 0 to 2 (3 layers)
  int x, y;                        // Loop counters
  for(x = 0; x < 16; x++)          // 'X' loop, columns.
  {
    for(y = 0; y < 8; y++)         // 'Y' loop, pixels in current column.   
    {
      matrixBuffer [layer] [y] [x] = 0;    
    }                              // Set all dots to 0.
  }
 return;                           // Return to calling function. 
} 



void matrixGrab( byte layer )      // Transfers current display to a buffer layer
{
  byte x, y;                               // Loop counters
  for(x = 0; x < 16; x++)                  // 'X' loop, columns.
  {
    for(y = 0; y < 8; y++)                 // 'Y' loop, pixels in current column. 
    {
//      matrixBuffer [layer] [y] [x] = myMatrix.matrix [y] [x]; // Copy pixel from matrix display to buffer layer.
    }
  }
  return; 
}



byte layerDotCount( byte layer )      // Scans buffer layer, returns number of dots wth a value higher than 0
{
  byte x, y;                               // Loop counters
  byte dotCount = 0;                       // Keeps count on the number of dots.

  for(x = 0; x < 16; x++)                  // 'X' loop, columns.
  {
    for(y = 0; y < 8; y++)                 // 'Y' loop, pixels in current column.
    {
      if (matrixBuffer [layer] [y] [x] >= 1)
      {                                    // Checks to see if pixel is not zero.
        dotCount++;                        // if so increment dot count.
      }
    }
  }
  return dotCount;                         // Return to calling function with total number of dots in layer.
}  


byte compileNumbers()    // Compiles the current time to the display buffer layer 0(matrixBuffer [0]) 
{                        // using renderNumber then retuns the number of dots used.
  byte digit1, digit2, digit3, digit4;        // Values for each of the 4 time digits.
  byte dotSum = 0;                            // Sum of all dots counted in renderNumber calls.

  if( format)                                 // Display time in 12 hour format.
  {
    digit2 = (hourFormat12() % 10);           // Calculate mod of hours ( just minutes' units), calculated first as needed in next calculation.
    digit1 = ((hourFormat12() - digit2)/10);  // Calculate hours' tens as unit.
    if(digit1 == 0)                           // For 12 hour format if tens of hour is zero a blank should be displayed.
    {
      digit1 = 10;                            // A digit value of 10 is a blank character.
    }
  }

  else if( !format)                           // Display time in 24 hour format.
  {   
    digit2 = (hour() % 10);                   // Calculate mod of hours ( just minutes' units).
    digit1 = ((hour() - digit2)/10);          // Calculate hours' tens as unit.
  }

  digit4 = minute() % 10;                     // Calculate mod of minutes ( just minutes' units).
  digit3 = ((minute() - digit4)/10);          // Calculate minutes' tens as unit.

  dotSum = dotSum + (renderNumber( digit1, 0, 1));              // Render hours' tens to buffer layer 0 in digit 1.
  dotSum = dotSum + (renderNumber( digit2, 4-BLINKTYPE, 1));    // Render hours' units to buffer layer 0 in digit 2,
                                                                // shift left one pixel if BLINKTYPE is in pixel mode.
  dotSum = dotSum + (renderNumber( digit3, 9, 1));              // Render minutes' tens to buffer layer 0 in digit 3.
  dotSum = dotSum + (renderNumber( digit4, 13, 1));             // Render minutes' units to buffer layer 0 in digit 4,
  return dotSum;                              // Return to calling function with total pixels rendered.
}





byte renderNumber( byte num, byte posx, byte posy)     
{           // Takes a byte and puts a graphic of that decimal number in the displays buffer ( layer 0 ),
            // and returns the number of dots used to make that graphic in buffer layer 0
  byte xLoop, yLoop, xCoord, yCoord;            // Loop counters and pixel coordinates. 
  byte dotCount;               // Number of pixels used to create numbers graphic.             
  for(xLoop = 0; xLoop < 3; xLoop++)       // Graphics' 'X' loop, columns.
  {
    for(yLoop = 0; yLoop < 6; yLoop++)     // Graphics' 'Y' loop, pixels in current column.
    {
      yCoord = (yLoop + posy);         // Add 'Y' offset to graphics' current pixel 'Y'coordinate. 
      xCoord = (xLoop + posx);         // Add 'X' offset to graphics' current pixel 'X'coordinate. 
      if (yCoord < 8 && xCoord < 16)   // Fillter out (deadzone) pixels that are outside display boundaries.
      {
        matrixBuffer [0] [yCoord] [xCoord] = pgm_read_byte_near(&(numberLarge [num] [yLoop] [xLoop]));
      }                        // Render character graphic to buffer layer 0.
    }
  }
  switch (num)                 // Look up table for current numbers dot count.
  {
  case 0:
    dotCount = 14;
    break;
  case 1:
    dotCount = 6;
    break;
  case 2:
    dotCount = 12;
    break;
  case 3:
    dotCount = 11;
    break;
  case 4:
    dotCount = 11;
    break;
  case 5:
    dotCount = 12;
    break;
  case 6:
    dotCount = 14;
    break;
  case 7:
    dotCount = 8;
    break;
  case 8:
    dotCount = 15;
    break;
  case 9:
    dotCount = 11;
    break;
  case 10:                    // Note: 10 is a blank.
    dotCount = 0;
    break;
  }
  return dotCount;            // Returns the number of pixels rendered
}


void displayNumber( byte dnNumber, byte dnXpos, byte dnYpos)          // Renders and display large numbes direct to display
{ 
  byte xLoop, yLoop, xCoord, yCoord;            // Loop counters and pixel coordinates.
  for(xLoop = 0; xLoop < 3; xLoop++)        // Graphics' 'X' loop, columns.
  {
    for(yLoop = 0; yLoop < 6; yLoop++)      // Graphics' 'Y' loop, pixels in current column.
    {       // Modulo 128 in the two following assignments dramaticaly reduces sketch size, very very strange???
      yCoord = (yLoop + dnYpos) % 128;          // Add 'Y' offset to graphics' current pixel 'Y'coordinate.
      xCoord = (xLoop + dnXpos) % 128;          // Add 'X' offset to graphics' current pixel 'X'coordinate.
      if (yCoord < 8 && xCoord < 16)    // Fillter out (deadzone) pixles that are outside display boundaries.
      {
//        myMatrix.matrix [yCoord] [xCoord] = pgm_read_byte_near(&(numberLarge [dnNumber] [yLoop] [xLoop]));
        myMatrix.drawPixel(xCoord,yCoord,pgm_read_byte_near(&(numberLarge [dnNumber] [yLoop] [xLoop])));
      }                         // Render character graphic to display.
    }   
  }
 return;
}



void displaySmallNumber( byte snNumber, byte snXpos, byte snypos)            // Renders and display small numbes direct to display
{

  byte xLoop, yLoop, xCoord, yCoord;            // Loop counters and pixel coordinates.
  for(xLoop = 0; xLoop < 2; xLoop++)       // Graphics' 'X' loop, columns.
  {
    for(yLoop = 0; yLoop < 5; yLoop++)     // Graphics' 'Y' loop, pixels in current column.
    {           // Modulo 200 in the two following assignments dramaticaly reduces sketch size, very very strange???
      yCoord = (yLoop + snypos) % 200;         // Add 'Y' offset to graphics' current pixel 'Y'coordinate, mod 8 to keep on display.
      xCoord = (xLoop + snXpos) % 200;         // Add 'X' offset to graphics' current pixel 'X'coordinate, mod 16 to keep on display.
      if (yCoord < 8 && xCoord < 16)   // Fillter out (deadzone) pixles that are outside display boundaries.
      {
 //       myMatrix.matrix [yCoord] [xCoord] = pgm_read_byte_near(&(numberSmall [snNumber] [yLoop] [xLoop]));
        myMatrix.drawPixel([xCoord],[yCoord], pgm_read_byte_near(&(numberSmall [snNumber] [yLoop] [xLoop])));
      }                        // Render character graphic to display.
    }
  }
 return;
}





void displayLetter( char asciiLetter, byte dlposx, byte dlposy)    // Creates a letter directly on the display
{                                    // these character graphics are defined in Characters.h and stored
                                     // in the program memory and are ASCII compatible.
                                     // A dead zone is also intergrated to enable scrolling on and of the display.
  byte xLoop, yLoop, xCoord, yCoord;                 // Loop counters and pixle coordinates.        
  char letterIndex;                  // Lookup table index value.
  letterIndex = ( asciiLetter - 65); // Convert ASCII value to lookup index value.
  for(xLoop = 0; xLoop < 3; xLoop++)             // Loop for character 'X'.
  {
    for(yLoop = 0; yLoop < 6; yLoop++)           // Loop for character 'Y'.
    {
      yCoord = ((yLoop + dlposy)-6);           // Subtract dead zone pixels
      xCoord = ((xLoop + dlposx)-3);           // Subtract dead zone pixels
      if (yCoord < 8 && xCoord < 16)         // Fillter out (deadzone) pixles that are outside display boundaries.
      {
//        myMatrix.matrix [yCoord] [xCoord] = pgm_read_byte_near(&(letter [letterIndex] [yLoop] [xLoop]));
myMatrix.drawPixel([xCoord],[yCoord],pgm_read_byte_near(&(letter [letterIndex] [yLoop] [xLoop])));
      }                              // Render character graphic to display.
    }
  }
  return;
}


void initialiseStreamerTraces()                       // Initalises the traces (struct) for streamers.
{
  byte is;                                            // Loop counter
  for (is = 0; is < 16; is++ )    
  {
    streamerTrace[is].active = false;                 // Set trace as inactive.
    streamerTrace[is].regen = random(10, 150);        // Generate a random frame number for trace start.
  }
  return;
}


void drawTraces( byte dtLayer, byte dtField, unsigned int dtSwell, boolean dtRunoff)
{                                      // Renders frame of traces to buffer, terimnates and generates random 
                                       // parameters for new traces.
    byte g;                            // Loop counter.
    byte  dtXpos, dtYpos;              // Trace 'X' and 'Y' coordinates temporary value holders.
    
     for ( g = 0 ; g < 16 ; g++ )      // Note: this loop is for 16 of trace generations Not 16 of 'x' coorinates.
    {                                  // Draw current traces to buffer layer 1. 
      if( streamerTrace[g].active == true)
      {                                // If curret trace is active, 
        dtYpos = streamerTrace[g].yPos;    // Copy 'Y' position,
        dtXpos = streamerTrace[g].xPos;    // and 'X' position to temporary holders for manipulation.
        switch (streamerTrace[g].length)   // Switch case based on current traces length.                 
        {
        case 3:                            //clipDraw constrains array dimentions to with in limits, 
                                           //if outside limits it dosn't draw them to buffer.
                                           //clipDraw(Layer, y pos, x pos, shade)
          clipDraw(dtLayer, dtYpos, dtXpos, 5);    // Head, this pixle actualy affects the wiping/pasting 
          dtYpos--;                                // in mergeLayers hense a value of 5.
          clipDraw(dtLayer, dtYpos, dtXpos, 2);    // Body
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 1);    // Tail
          break;

        case 4:
          clipDraw(dtLayer, dtYpos, dtXpos, 5);    // Head, this pixle actualy affects the wiping/pasting
          dtYpos--;                                // in mergeLayers hense a value of 5.
          clipDraw(dtLayer, dtYpos, dtXpos, 4);    // Head
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 2);    // Body
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 1);    // Tail
          break;

        case 5:
          clipDraw(dtLayer, dtYpos, dtXpos, 5);    // Head, this pixle actualy affects the wiping/pasting
          dtYpos--;                                // in mergeLayers hense a value of 5.
          clipDraw(dtLayer, dtYpos, dtXpos, 4);    // Head
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 2);    // Body
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 2);    // Body
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 1);    // Tail
          break;

        case 6:
          clipDraw(dtLayer, dtYpos, dtXpos, 5);    // Head, this pixle actualy affects the wiping/pasting
          dtYpos--;                                // in mergeLayers hense a value of 5.
          clipDraw(dtLayer, dtYpos, dtXpos, 4);    // Head
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 2);    // Body
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 2);    // Body
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 1);    // Tail
          dtYpos--;
          clipDraw(dtLayer, dtYpos, dtXpos, 1);    // Tail
          break;
        }
        
        
      // Move traces. ! Field and speed ready for next frame.
       
      switch(streamerTrace[g].speed)       // Evaluates the speed
      {
       case 1:                             // Speed 1 (slowest) increments y in ever 3 fields
        if ( dtField == 3 || dtField == 6 )
        {
          streamerTrace[g].yPos++;         // Increment position in 'y'.
        }
        break;

      case 2:                              // Speed 2 (middle) increments y in every other field    
        if ( dtField == 2 || dtField == 4 || dtField == 6 )
        {
          streamerTrace[g].yPos++;         // Increment position in 'y'.
        }
        break;
   
      case 3:                              // Speed 3 (fastest) increments y in every field
        streamerTrace[g].yPos++;                    // Increment position in 'y'.
        break;
      }
    }
           
      if( streamerTrace[g].active == false && dtRunoff == 0 ) // New Trace regeneration but not during runoff
      {
        if( streamerTrace[g].regen <= frameCount)        // Check thet it is the right frame or later
        {                                                // for regeneration.
          streamerTrace[g].active = true;                // Activate trace.
          streamerTrace[g].xPos = g;                     // give trace same x as trace number ( this helps to get an evenspread).
          streamerTrace[g].yPos = 0;                     // Reset 'Y' position. 
          streamerTrace[g].speed = random(1, 4);         // Assign a random speed
          streamerTrace[g].length = random(3, 6);        // Assign a random length
          streamerTrace[g].terminate = ( 8 + streamerTrace[g].length );  // Calculate termination point.rTerminate
        }        
      }
      
      if ( streamerTrace[g].yPos == streamerTrace[g].terminate )  // When the trace reaches its termination point.
      {     
        if (dtRunoff == 0)                                 // If not yet in runoff stage,
        {                                                // create a new random regeneration frame
          streamerTrace[g].regen = random(frameCount, (frameCount + dtSwell) ); 
        }                                                // calculate based on current frame and swell size.
        streamerTrace[g].active = false;                 // Deactivate trace.

        streamerTrace[g].yPos = 0;                       // Reset starting point.
      }
    }
  return;
}

// utilities--------------------------------------------------------------------------------------------------




byte byte_wrap(byte x, int lower, int upper, int inc)  
{
                                              // This function takes a byte and increment it with in two limits. 
                                              // If it goes beyond those limits it will wrap around and return the new value.
  byte result;                                // New byte value
  int value = (int)x + inc;                   // Convert x into an int to allow for values 
                                              // beyond a byte and apply the increment. 
  int range = (upper - lower) + 1;            // Find the range and add 1 to make it inclusive.

  if( value <= upper || value >= lower) 
  {                                           // If the result is still between the limits
    result = (byte)value;                     // then just convert it to a byte for the return.
  }
  if( value > upper)                          // If the result is higher than the upper limit
  {                                           // mod the value with the range to get the remainder
    result = (byte)(lower + (value % range) - 1);
  }                                           // add the lower limit and sub 1 to account for the wrap
                                              // arround, then just convert it to a byte for the return.
  if( value < lower)                 
  {                                           // If the value is below the lower limit
    if( value < 0 )                           // and bellow zero first converted to a positive(absolute value)
    {                                         // then mod with the range to get the remainder
      result = (byte)((upper + 1 ) - (abs(value) % range) );
    }                                         // the upper limit needs to be increased by 1 to compensate  
                                              // for passing zero the value is subtracted and converted to a byte.
    else                                      // If the value is not lower than zero the prosses is the same
    {                                         // but the upper limit dose not need to compensate for crossing zero
      result = (byte)(upper - (value % range) );
    }                                         // and the value is already positive.
  }
  else
  {                                           // some situations when zero is crossed making value such that
    result = (byte)(value % range);           // it is not identified by the if previuos conditions, this else 
  }                                           // catches thease conditions and resolves them with a mod.
  return result;                              // Return with the final result.
}



void saveAlarmTime()                                  // Saves alarm settings to EEPORM so they can be reinstated 
{                                                     // at next power up after a power down/reset.               
  int address = 0;                                    // EEPROM address counter.
  EEPROM.write(address, alarmTime.hour);              // Write alarm hour to EEPROM address 0.
  address++;                                          // Increment EEPORM address.
  delay(4);                                           // Delay required to allow EEPROM writing to complete.
  EEPROM.write(address, alarmTime.minute);            // Write alarm minute to EEPROM address 1.
  address++;                                          // Increment EEPORM address.
  delay(4);
  EEPROM.write(address, alarmTime.snoozeHour);        // Write alarm snooze hour to EEPROM address 2.
  address++;                                          // Increment EEPORM address.
  delay(4);
  EEPROM.write(address, alarmTime.snoozeMinute);      // Write alarm snooze minute to EEPROM address 3.
  address++;                                          // Increment EEPORM address.
  delay(4);
  EEPROM.write(address, alarmTime.melody);            // Write alarm tone to EEPROM address 4.
  delay(4);
  return;
}
void restoreAlarmTime()
{
  int address = 0;
  alarmTime.hour = EEPROM.read(address);              // Read alarm hour from EEPROM address 0.
  address++;                                          // Increment EEPORM address.
  alarmTime.minute = EEPROM.read(address);            // Read alarm minute from EEPROM address 1.            
  address++;                                          // Increment EEPORM address.
  alarmTime.snoozeHour = EEPROM.read(address);        // Read alarm snooze hour from EEPROM address 2.
  address++;                                          // Increment EEPORM address.
  alarmTime.snoozeMinute = EEPROM.read(address);      // Read alarm snooze minute from EEPROM address 3.
  address++;                                          // Increment EEPORM address.
  alarmTime.melody = alarmTone = EEPROM.read(address);// Read alarm tone from EEPROM address 4.
  return;
}


void sleepNow()         // here we put the arduino to sleep
{
  digitalWrite(EXBLINK, LOW);
  /* Now is the time to set the sleep mode. In the Atmega8 datasheet
   * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
   * there is a list of sleep modes which explains which clocks and 
   * wake up sources are available in which sleep mode.
   *
   * In the avr/sleep.h file, the call names of these sleep modes are to be found:
   *
   * The 5 different modes are:
   *     SLEEP_MODE_IDLE         -the least power savings 
   *     SLEEP_MODE_ADC
   *     SLEEP_MODE_PWR_SAVE
   *     SLEEP_MODE_STANDBY
   *     SLEEP_MODE_PWR_DOWN     -the most power savings
   *
   * For now, we want as much power savings as possible, so we 
   * choose the according 
   * sleep mode: SLEEP_MODE_PWR_DOWN
   * 
   */
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   // sleep mode is set here

  sleep_enable();          // enables the sleep bit in the mcucr register
  // so sleep is possible. just a safety pin 


  /* Now it is time to enable an interrupt. We do it here so an 
   * accidentally pushed interrupt button doesn't interrupt 
   * our running program. if you want to be able to run 
   * interrupt code besides the sleep function, place it in 
   * setup() for example.
   * 
   * In the function call attachInterrupt(A, B, C)
   * A   can be either 0 or 1 for interrupts on pin 2 or 3.   
   * 
   * B   Name of a function you want to execute at interrupt for A.
   *
   * C   Trigger mode of the interrupt pin. can be:
   *             LOW        a low level triggers
   *             CHANGE     a change in level triggers
   *             RISING     a rising edge of a level triggers
   *             FALLING    a falling edge of a level triggers
   *
   * In all but the IDLE sleep modes only LOW can be used.
   */

  attachInterrupt(0,wakeUpNow, RISING); // use interrupt 2 (pin 0) and run function
  // wakeUpNow when pin 0 gets LOW 

  sleep_mode();            // here the device is actually put to sleep!!
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

  sleep_disable();         // first thing after waking from sleep:
  // disable sleep...
  detachInterrupt(0);      // disables interrupt 2 on pin 0 so the 
  // wakeUpNow code will not be executed 
  // during normal running time.

}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
  intro = true;
}

// control functions------------------------------------------------------------------------------------------


void ButtonScan()                  // This runs as often as possible. It polls all the buttons and 
{                                  // recordes each of their status. ( Down, Latched and Pressed)
                                   // This alows the prossesor to get on with other things during
                                   // debouncing and hold down timers.

  byte i;                          // Loop counter

  if( debounce > currentMillis )
  {                                // Debounce timer, return immediately if still running.
    return;
  } 
//  buttonDown [0] = !digitalRead(S0);  // Poll each of the buttons
//  buttonDown [1] = !digitalRead(S1);
//  buttonDown [2] = !digitalRead(S2);
//  buttonDown [3] = !digitalRead(S3);

  for ( i = 0; i < 5; i++)  
  {                                // Scan button data arrays
    if (buttonDown [i] == true && buttonLatch [i] == false)
    {                              // If a button is currently being held down but not yet latched,
      buttonLatch [i] = true;      // latch it and set a debounce timer of 20mS.
      debounce = currentMillis + 20;
    }

    if (buttonDown [i] == false && buttonLatch [i] == true)
    {                              // If the button is no longer being held down and was latched                  
      buttonPressed [i] = true;    // mark the button as having been pressed, this should be
      // cleared by what ever fuction next makes use of it.
      buttonLatch [i] = false;     // Clear the latch.  
    }
  }      
  debounce = currentMillis + 20;  // set a debounce timer of 20mS.
  return;
}         




void MultiButtonHandler()          // Usualy called right after the buttonScan, this function determins what action is
{                                  // to be taken depending on which buttons have been pressed and how.
  if ( alarmTrigger)               // If alarm is sounding buttons can have a different function.
  {
    if ( buttonPressed [1]  == true  && clockMode < 250)        // If in a normal clock mode ( not setting or alarm toggle),
    {                                                           // button S1 is pushed to set a snooze timer
      alarmTime.snoozeMinute = byte_wrap((minute()),0,59,SNOOZETIME);    // Incement snooze miniutes by amount specified by SNOOZETIME
      if ( alarmTime.snoozeMinute < minute() )                  // Increases snooze hour if snooze period crosses an hour.
      {                                                         // byte_wrap will wrap the minuets around and they will therfore be less
        alarmTime.snoozeHour = byte_wrap(hour(),0,23,1);        // than current minutes. 
      }

      alarmTrigger = false;                                    // Deactivate alarm trigger.
      buttonPressed [1] = false;                               // Reset button S1.
    }
    else if ( buttonPressed [0] == true || buttonPressed [2] == true || buttonPressed [3] == true)
    {                                               // If any other button is pushed this will deactivate the alarm completely
      buttonPressed [0] = false;                    // Reset button S0.
      buttonPressed [2] = false;                    // Reset button S2.
      buttonPressed [3] = true;                     // Force set button S3, this causes program to go toggle alarm off after button handler.
      alarmTrigger = false;                         // Deactivate alarm trigger.
    }
  }


  if ( buttonPressed [0]  == true  && clockMode < 250 )          // If, in a normal clock mode ( not setting or alarm toggle),
  {                                                  // button S0 is pushed to cycle through clocks and animations
    clockMode++;                                     // Increment clock mode.
    matrixGrab(2);                                   // Put a copy of the current display in buffer layer 2 ( for intergrating to some animation)
    intro = true;                                    // To indicate to the next animation that it is being entered from another clock mode.
    blinker(false);                                  // Turn off blinker.
    myMatrix.clear();                                // Clear display
    buttonPressed [0] = false;                       // Reset button S0.
    buttonPressed [2] = false;                       // Reset button S2.
    buttonPressed [3] = false;                       // Reset button S3.
  }

  if ( buttonDown [1] == true && holdTimer < currentMillis && clockMode < 250)      // If true then the button S1 has been held down for more than 3 seconds
  {                                    // Program will now enter setting procedure.
    saveClockMode = clockMode;         // Save current clock mode to return to after settings are done.
    clockMode = 255;                   // Mode for settings procedure
    alarmTrigger = false;              // Deactivate alarm trigger
    buttonPressed [1] = false;         // Reset button S1.
  }
  else if (buttonDown [1] == false)    // If true then button S1 has not been held down for 3 seconds,
  {
    holdTimer = currentMillis + 3000;  // Reset holdTimer
  }

  if ( buttonPressed [3]  == true  && clockMode < 250 )          // If, in a normal clock mode ( not setting or alarm toggle),
  {                                    // button S3 is pushed to toggle alarm on or off.
    myMatrix.clear();                  // Clear display.
    saveClockMode = clockMode;         // Save current clock mode to return to after toggling the alarm on or off.
    clockMode = 254;                   // Put in alarmToggle mode.
  }


  return;
}

void alarmToggle(void)                  // Enables/disables the alarm
{
  FrameRate = 60;
  blinker(false);
    byte cx,cy;                       // For loop counters
    byte bell[8] [5] = {              // Array for bell symbol
    { 0,0,0,0,0,},
    { 0,0,4,0,0,},
    { 0,4,2,4,0,},
    { 0,4,2,4,0,},
    { 0,4,2,4,0,},
    { 0,4,2,4,0,},
    { 4,4,4,4,4,},
    { 0,0,4,0,0,}
 };

  
  if(buttonPressed [3] == true)          // Button 3 toggles alarm on and off
  {   
    myMatrix.clear();                    // Clear display.
    toggleTimer = currentMillis + 2000;  // Timer reset every time the alarm on/off button is pushed.
    alarmActive = !alarmActive;          // Toggles alarm on or off.
    buttonPressed [3] = false;
    alarmTime.snoozeHour = alarmTime.hour;        // Reset snooze time
    alarmTime.snoozeMinute = alarmTime.minute;    
  }
  
 
  
  if (toggleTimer > currentMillis)
  {
    
    for( cx = 0; cx < 5; cx++)            // For loop displays bell symbol
    {
      for( cy = 0; cy < 8; cy++)
      {
//        myMatrix.matrix [cy] [cx+1] = bell [cy] [cx];
        myMatrix.drawPixel(cx+1,cy,bell [cy] [cx]);
      } 
    } 
   
    if (alarmActive == true)               // If alarm active display 'on'
    { 
      displayLetter('o', 10, 7);
      displayLetter('n', 14, 7);
    }
    else if (alarmActive == false)          // If alarm not active display 'off'
    {
      displayLetter('o', 10, 7);
      displayLetter('f', 13, 7);
      displayLetter('f', 16, 7);
    }
    return;
  }
  else
  {
    blankLayer(2);                       // Blank layer ready for matrixGrab on exit of this function.
    matrixGrab(2);                       // Copy alarm on/off screen to buffer for integration into animation
    intro = 1;                           // Set to run animation when going back into clock mode.
    clockMode = saveClockMode;           // Restore previous clockMode
  }
  return;
}


void alarm(void)                              // Checks alarm status, updates snooze timings
{                                             // and sounds alarm when alarm is triggered.
  if( alarmActive == true )                   // Check if active.
  {
    if ( alarmTrigger == true )               // Check trigger.
    {
      tune(true);                             // plays alarm melody/tone if active and triggered.
      return;
    }
    // Check and update alarm and snooze status if alarm not triggered
    if( hour() == alarmTime.hour || hour() == alarmTime.snoozeHour )
    {                                         // Compare current hour agains alarm and snooze hour
      if((minute() == alarmTime.minute && minute() == alarmTime.snoozeMinute) || minute() == alarmTime.snoozeMinute)
      {
        tune(false);                          // Reset tune position.
        alarmTone = alarmTime.melody;         
        alarmTrigger = true;                  // If either alarm or snooze time matches current time the trigger will       
      }                                       // be set and the alarm will sound next time this function is called.
    }
  }
  return;
}



void tune( boolean reset)               //Play tunes in Tunes.h if parameter is TRUE.
{  
  static unsigned long tuneTime = 0;    // Keeps track of next note time in millis.
  if(reset == false)                    // Resets tunes position and returns.
  {
    tunePosition = 0;
    return;
  }
  int interval;                          // Total time between notes in millis
  if(tuneTime <= currentMillis)
  {
    note currentNote;                    // Create struct of type note, this includes a pitch and duration
    switch(alarmTone)                    // alarmTone is set in settings to select witch tune is to be played
    {
    case 0:                              // Tunes and pitches are stored in the program memory.
      currentNote.pitch = pgm_read_word_near(&(melody0 [tunePosition].pitch));        //Get pitch from prog mem.
      currentNote.duration = pgm_read_byte_near(&(melody0 [tunePosition].duration));  //Get duration from prog mem.
      break;
    case 1:
      currentNote.pitch = pgm_read_word_near(&(melody1 [tunePosition].pitch));
      currentNote.duration = pgm_read_byte_near(&(melody1 [tunePosition].duration));
      break;
    case 2:
      currentNote.pitch = pgm_read_word_near(&(melody2 [tunePosition].pitch));
      currentNote.duration = pgm_read_byte_near(&(melody2 [tunePosition].duration));
      break;
    case 3:
      currentNote.pitch = pgm_read_word_near(&(melody3 [tunePosition].pitch));
      currentNote.duration = pgm_read_byte_near(&(melody3 [tunePosition].duration));
      break;  
    default:
      alarmTone = 0;                          // Set alarmTone to zero if theres and error.
      tuneTime = 0;                           // Reset tuneTime        
    }

    interval = 55 * (currentNote.duration);   // Multiply note duration by 55 to give total note duration in millis.
    tuneTime = currentMillis + interval;      // add interval to currentMillis to get next note time.

    if( currentNote.pitch != REST && currentNote.pitch != EOT)
    {                                        // Note played unless on a REST or if it is the End Of the Tune
//      tone(BUZZERPIN, currentNote.pitch, (interval-20));
    }
    tunePosition++;                          // Index tunes position to next note.

    if( currentNote.pitch == EOT )
    {
      tunePosition = 0;                       // Rest the tune position if End Of Tune encountered,
    }                                         // tune will now loop back to the begining and play again.
  }
  return;  
}





//-----------------------------------------------------------------------------------

void blinker(boolean active)                   // Causes either two centre LEDs to Flash in seconds
{                                              // or two of the matrix dots to flash (see BLINKTYPE).
  static byte inhibit = 2;                     // Countdown for a 2 flash delay on restarting the blinker,
                                               // this prevents any semi flashes after an animation.
  static unsigned long oldMillis = 0;          // Stores previous milliseconds.
  static byte oldSeconds= 0;                   // Stores previous seconds.
  if ( inhibit > 0 && active == true )         // This should test true if blinker have just been reactivated.
  {
    inhibit--;                                 // Countdown to first blink
    oldMillis = millis();                      // Save current milliseconds ready for next blinker call.
    oldSeconds = second();                     // Save current seconds ready for next blinker call.
    halfSeconds = LOW;                         // Ensure half seconds and therefor blinker LEDs are low. 
    digitalWrite(EXBLINK, halfSeconds);
    return;                                    // Return without doing anythingelse
  }
                                              
  if(oldSeconds != second() || (millis() - oldMillis) >= 500)
  {                                            // If either the second or halfsecond point has been crossed then..
    oldMillis = millis();                      // update old milliseconds,
    oldSeconds = second();                     // update old milliseconds,
    halfSeconds = !halfSeconds;                // toggle halfSeconds.

    if (active)                                // If blinker is active then depending on the BLINKTYPE option,
    {
      #if( !BLINKTYPE )
      digitalWrite(EXBLINK, halfSeconds);      // the LEDs will bilnk on and then off every second
      #endif
      
      #if( BLINKTYPE )
      if(halfSeconds == true)                  // or pixels 7,2 and 7,5 will flash on and then off every second
      {
//        myMatrix.matrix[2] [7] = 4;
//        myMatrix.matrix[5] [7] = 4;
        myMatrix.drawPixel(2,7,1);
        myMatrix.drawPixel(5,7,1);
      }
      else //if ( format == HOUR12 )
      {
//        myMatrix.matrix[2] [7] = 0;
//        myMatrix.matrix[5] [7] = 0;
        myMatrix.drawPixel(2,7,0);
        myMatrix.drawPixel(5,7,0);

      
      }
      #endif
    }
  }
  if (!active)                                  // If blinker is not active then the LED pin will remain low
  {                                             // but the halfSeconds will continue to toggle every
    digitalWrite(EXBLINK, LOW);                 // half second for other functions to use it.
    inhibit = 2;                                // Reset the inhibit counter for the next 
  }                                             // time blinker is reactivated.
  return;  
}


void Settings()                      // This function runs a through a setting procedure for the
{                                    // user to set the time, date, alarm etc.
  FrameRate = 60;                    // This will affect the speed of the message scrolling
  blinker(false);                    // Turn off blinker

  static byte stage = 0;             // Stettings stage, ie. set time, set date...etc
  time_t settingTime = now();        // Get a temporay copy of the current time for setting to manipulate
  static byte setHour;               // temporary storage of settings values.
  static byte setMinute;
  static byte setSecond;
  static byte setDay;
  static byte setWeekDay;
  static byte setMonth;
  static unsigned int setYear;





  if (buttonPressed [0] == HIGH)      // If mode button is pushed at any stage, exit settings
  {                                   // without making any changes
    stage = 0;                        // Reset settings stage
    clockMode = 0;                    // Reset clock mode to first one.
    buttonPressed [0] = LOW;          // Reset button
    scrollPosition = 255;             // Reset message sroll.
    myMatrix.clear();                 // Clear display
    intro = true;                     // Reset intro for animation intigration.
    return;
  }

  byte digit1,digit2,digit3,digit4;   // Values for each of the 4 time digits.


  switch(stage)                       // Switch case for settings stages.    
  {  
  case 0:                             // Stage 0, scroll message "Settings" across the display.

    if (!ScrollMessage(msgSet, sizeof(msgSet)))  // Scrolls message untill ScrollMessage() returns false,
    {                                            // this signifies the end of the message scrolling.
      myMatrix.clear();                          // Clear display.
      scrollPosition = 255;                      // Reset message scroll.
      if ( buttonPressed [1] == HIGH)            // If button S1 is pushed at any stage then settings() will advance to the next stage.
      {                                          // If no buttons are pushed the message will scroll again.
        buttonPressed [1] = LOW;                 // Reset button S1

        settingTime= now();                      // Update current time.
        setHour = hour();                        // Get current hour.
        setMinute = minute();                    // Get current minute.
        setSecond = second();                    // Get current second. 
        setDay = day();                          // Get current day
        setWeekDay = weekday();                  // Get current day of the week
        setMonth = month();                      // Get current month
        setYear = year();                        // Get current year
        scrollPosition = 255;                    // Reset message scroll.         
        stage = 1;                               // Advance settings procedure to next stage.
      }           
    }
    break;

  case 1:                               // Stage 1, scroll message "Set Time" across the display.

    if (!ScrollMessage(msgTim, sizeof(msgTim))|| buttonPressed [1] == HIGH)
    {                                    // Scrolls message untill ScrollMessage() returns false,
      buttonPressed [1] = LOW;           // Reset button S1, if no buttons are pushed the message will scroll again. 
      myMatrix.clear();                  // Clear display.
      scrollPosition = 255;              // Reset message scroll.
      stage = 2;                         // Advance settings procedure to next stage.
    }
    break;


                    

  case 2:                               // Stage 2, set the hour, this will display the current time with the hour flashing.
    if(halfSeconds == false && buttonPressed [3] == LOW && buttonPressed [4] == LOW && buttonDown [3] == LOW && buttonDown [4] == LOW)
    {                                   // If no buttons have been pushed and it is in a half second, blank the hour digits.
      displayNumber( 10, 0, 1);         // Blank in digit 1.
      displayNumber( 10, 4, 1);         // Blank in digit 2.

    }
    else                                // If in a full second dispaly tempoary hours
    {
      digit2 = (setHour % 10);          // Calculate mod of hour ( just hour units), calculated first as needed in next calculation.
      digit1 = ((setHour - digit2)/10); // Calculate tens as unit.
      
      displayNumber( digit1, 0, 1);     // Display hours' tens in digit 1.
      displayNumber( digit2, 4, 1);     // Display hours' units in digit 2.
    }

    digit4 = minute() % 10;              // Calculate mod of minute ( just minutes' units) in digit2
    digit3 = ((minute() - digit4)/10);   // Calculate minutes' tens as unit.

    displayNumber( digit3, 9, 1);        // Display minutes' tens in digit 3.
    displayNumber( digit4, 13, 1);       // Display minutes' units in digit 4.



    if (buttonPressed [1] == HIGH)       // If button S1 is pushed then advance to the next stage.
    {
      buttonPressed [1] = LOW;           // Reset button S1
      stage = 3;
    } 

    if (buttonPressed [3] == HIGH)       // If button S3 is pushed then increase hour by 1.
    {
      buttonPressed [3] = LOW;           // Reset button S3
      setHour = byte_wrap(setHour,0,23,1);  // Increment hour, byte_wrap() will increment or decrement a value
                                            // but wrap around to the begining/end if limit is reached.
                                            // byte_wrap ( byte variable , bottom limit, top limit, increment/decrement)  
      setTime(setHour,setMinute,0,setDay,setMonth,setYear);      // Update time for display.
    }

    if (buttonPressed [2] == HIGH)        // If button S2 is pushed then decrease hour by 1.
    {
      buttonPressed [2] = LOW;            // Reset button S2
      setHour = byte_wrap(setHour,0,23,-1);     // Decrement hour
      setTime(setHour,setMinute,0,setDay,setMonth,setYear);       // Update time for display.
    }  
    break;



  case 3:                           // Stage 3, set the minutes, this will display the current time with the minutes flashing.
    if(halfSeconds == false && buttonPressed [2] == LOW && buttonPressed [3] == LOW && buttonDown [2] == LOW && buttonDown [3] == LOW)
    {                                    // If no buttons have been pushed and it is in a half second, blank the minutes digits.
      displayNumber( 10, 9, 1);         // Blank in digit 3.
      displayNumber( 10, 13, 1);        // Blank in digit 4.

    }
    else                                // If in a full second dispaly temporary minutes
    {
      digit4 = minute() % 10;           // Calculate mod of minutes ( just minutes' units), calculated first as needed in next calculation.
      digit3 = ((minute() - digit4)/10); // Calculate tens as unit.

      displayNumber( digit3, 9, 1);      // Display minutes' tens in digit 3.
      displayNumber( digit4, 13, 1);     // Display minutes' units in digit 4.
    }
    digit2 = (hour() % 10);              // Calculate mod of minute ( just minutes' units).
    digit1 = ((hour() - digit2)/10);     // Calculate minutes' tens as unit.


    displayNumber( digit1, 0, 1);        // Display hours' tens in digit 1.
    displayNumber( digit2, 4, 1);        // Display hours' units in digit 2.



    if (buttonPressed [1] == HIGH)       // If button S1 is pushed then advance to the next stage.
    {
      buttonPressed [1] = LOW;           // Reset button S1.
      stage = 4;                         // Advance settings procedure to next stage.
    } 

    if (buttonPressed [3] == HIGH)      // If button S3 is pushed then increase minutes by 1.
    {
      buttonPressed [3] = LOW;                                   // Reset button S3.
      setMinute = byte_wrap(setMinute,0,59,1);                   // Increment minutes.
      setTime(setHour,setMinute,0,setDay,setMonth,setYear);      // Update time for display.
    }

    if (buttonPressed [2] == HIGH)       // If button S2 is pushed then decrease minutes by 1.
    {
      buttonPressed [2] = LOW;                                   // Reset button S2.
      setMinute = byte_wrap(setMinute,0,59,-1);                  // Decrement minutes.
      setTime(setHour,setMinute,0,setDay,setMonth,setYear);      // Update time for display.
    }
    break;
    // Set Format

  case 4:                                 // Stage 4, scroll message "Set Format" across the display.
    if (!ScrollMessage(msgFrm, sizeof(msgFrm))|| buttonPressed [1] == HIGH)
    {
      buttonPressed [1] = LOW;              // Reset button S1.
      myMatrix.clear();                     // Clear display.
      scrollPosition = 255;                 // Reset message scroll.
      stage = 5;                            // Advance settings procedure to next stage.
    }
    break;

  case 5:                                 // Stage 5, set the time format to 12 or 24 hour.
    if(format == true)                    // format boolean gets toggles between true for 12 hour format
    {                                     // and false for 24 hour format.
      displayNumber( 1, 0, 1);            // Display a '1'.
      displayNumber( 2, 4, 1);            // Display a '2'.

    }
    else if (format == false)
    {
      displayNumber( 2, 0, 1);            // Display a '2'.
      displayNumber( 4, 4, 1);            // Display a '4'.
    }

    displayLetter('H', 12, 7);            // Display a 'H'.
    displayLetter('r', 16, 7);            // Display a 'r' as an abbreviation for "Hour".



    if (buttonPressed [1] == HIGH)        // If button S1 is pushed then advance to the next stage.          
    {
      buttonPressed [1] = LOW;            // Reset button S1.
      stage = 6;                          // Advance settings procedure to next stage.
    } 

    if (buttonPressed [3] == HIGH || buttonPressed [2] == HIGH)  //Either button S2 or S3 will toggle the format value.
    {
      buttonPressed [3] = buttonPressed [2] = LOW;      // Reset both button S1 and S2.
      format = !format;                   // Toggle format.    
    }
    break;  

    // Set Date

  case 6:                                   // Stage 6, scroll message "Set Date" across the display.
    if (!ScrollMessage(msgDat, sizeof(msgDat))|| buttonPressed [1] == HIGH)
    {
      buttonPressed [1] = LOW;              // Reset button S1
      myMatrix.clear();                     // Clear display.
      scrollPosition = 255;                 // Reset message scroll.
      stage = 7;                            // Advance settings procedure to next stage.
    }
    break; 


  case 7:                                   // Stage 7, set the day of the month.
    digit2 = (day() % 10);                  // Calculate mod of day ( just day units).
    digit1 = ((day() - digit2)/10);         // Calculate tens of days as unit.


    displayNumber( digit1, 0, 1);           // Display days' tens in digit 1.     
    displayNumber( digit2, 4, 1);           // Display days' units in digit 2.

    displayLetter('D', 12, 7);              // Display a 'D'.
    displayLetter('y', 16, 7);              // Display a 'y' as an abbreviation for "Day".



    if (buttonPressed [1] == HIGH)          // If button S1 is pushed then advance to the next stage.
    {
      buttonPressed [1] = LOW;              // Reset button S1.
      stage = 8;                            // Advance settings procedure to next stage.
    } 

    if (buttonPressed [3] == HIGH)          // If button S3 is pushed then increase day by 1.
    {
      buttonPressed [3] = LOW;              // Reset button S3.
      settingTime = settingTime + 86400;    // Add 86400 seconds (24 hours worth) to settingTime.
      setTime(settingTime);                 // Update time for display.
    }

    if (buttonPressed [2] == HIGH)          // If button S2 is pushed then decrease day by 1.
    {
      buttonPressed [2] = LOW;              // Reset button S2.
      settingTime= settingTime - 86400;     // Subtract 86400 seconds (24 hours worth) to settingTime.
      setTime(settingTime);                 // Update time for display.
    }  
    break;
  
    // Set Month

  case 8:                                   // Stage 7, set the month of the year.                                 
    digit2 = (month() % 10);                // Calculate mod of month ( just month units).                
    digit1 = ((month() - digit2)/10);       // Calculate tens of months' as units.


    displayNumber( digit1, 0, 1);           // Display months' tens in digit 1.
    displayNumber( digit2, 4, 1);           // Display months' units in digit 2.

    displayLetter('M', 12, 7);              // Display a 'M'.
    displayLetter('n', 16, 7);              // Display a 'n' as an abbreviation for "Month".



    if (buttonPressed [1] == HIGH)          // If button S1 is pushed then advance to the next stage.
    {
      buttonPressed [1] = LOW;              // Reset button S1.
      stage = 9;                            // Advance settings procedure to next stage.
    } 

    if (buttonPressed [3] == HIGH)          // If button S3 is pushed then increase months by 1.
    {
      buttonPressed [3] = LOW;                                           // Reset button S3.
      setMonth = byte_wrap(setMonth,1,12,1);                             // Increment months.
      setTime(setHour,setMinute,setSecond,setDay,setMonth,setYear);      // Update date for display.
    }

    if (buttonPressed [2] == HIGH)         // If button S2 is pushed then decrease months by 1
    {
      buttonPressed [2] = LOW;                                           // Reset button S2.
      setMonth = byte_wrap(setMonth,1,12,-1);                            // Decrement months.
      setTime(setHour,setMinute,setSecond,setDay,setMonth,setYear);      // Update date for display.  
    }  
    break; 

    // Set Year

  case 9:                                   // Stage 9, scroll message "Set Year" across the display.                                      
    if (!ScrollMessage(msgYea, sizeof(msgYea))|| buttonPressed [1] == HIGH)
    {
      buttonPressed [1] = LOW;              // Reset button S1
      myMatrix.clear();                     // Clear display.
      scrollPosition = 255;                 // Reset message scroll.
      stage = 10;                           // Advance settings procedure to next stage.
    }
    break;

  case 10:
    digit4 = (setYear % 10);                // Calculate mod of year ( just year units), calculated first as needed in next calculation.
    digit3 = (((setYear - digit4)/10)%10);                                  // Calculate tens of years as units.   
    digit2 = (((setYear - ((digit3*10)+digit4))/100)%10);                   // Calculate hundereds of years as units.
    digit1 = (((setYear - ((digit2*100)+(digit3*10)+digit4))/1000));        // Calculate thousands of years as units.

    displayNumber( digit1, 0, 1);           // Display years' thousands in digit 1.
    displayNumber( digit2, 4, 1);           // Display years' hundereds in digit 2.
    displayNumber( digit3, 9, 1);           // Display years' tens in digit 3.
    displayNumber( digit4, 13, 1);          // Display years' units in digit 4.

    if (buttonPressed [1] == HIGH)           // If button S1 is pushed then advance to the next stage.
    {
      buttonPressed [1] = LOW;               // Reset button S1
      stage = 11;                            // Advance settings procedure to next stage.
    } 

    if (buttonPressed [3] == HIGH)           // If button S3 is pushed then increase year by 1.
    {
      buttonPressed [3] = LOW;                                           // Reset button S3
      setYear++;                                                         // Increment year.
      setTime(setHour,setMinute,setSecond,setDay,setMonth,setYear);      // Update time for display.
    }

    if (buttonPressed [2] == HIGH)
    {
      buttonPressed [2] = LOW;                                           // Reset button S2.
      setYear--;                                                         // Decrement year.
      setTime(setHour,setMinute,setSecond,setDay,setMonth,setYear);      // Update time for display.
    }  
    break; 

    // Set Alarm 
  
  case 11:                               // Stage 9, scroll message "Set Alarm" across the display.
    if (!ScrollMessage(msgAlm, sizeof(msgAlm))|| buttonPressed [1] == HIGH)
    {
      buttonPressed [1] = LOW;           // Reset button S1
      myMatrix.clear();                  // Clear display.
      scrollPosition = 255;              // Reset message scroll.
      stage = 12;                        // Advance settings procedure to next stage.
    }
    break; 

    // Set Alarm Hour
  case 12:                              // Stage 12, set the alarm hours, this will display the current alarm time with the hours flashing.
    if(halfSeconds == false && buttonPressed [2] == LOW && buttonPressed [3] == LOW && buttonDown [2] == LOW && buttonDown [3] == LOW)
    {                                   // If no buttons have been pushed and it is in a half second, blank the hours digits
      displayNumber( 10, 0, 1);         // Blank in digit 1.   
      displayNumber( 10, 4, 1);         // Blank in digit 2.
    }
    else                                // If in a full second dispaly temporary minutes.
    {
      digit2 = (alarmTime.hour % 10);              // Calculate mod of hour( just hours' units)
      digit1 = ((alarmTime.hour - digit2)/10);     // Calculate tens or hours as unit.
      displayNumber( digit1, 0, 1);                // Display hours' tens in digit 1.
      displayNumber( digit2, 4, 1);                // Display hours' units in digit 2.
    }

    digit4 = alarmTime.minute % 10;                // Calculate mod of minute ( just minutes' units).  
    digit3 = ((alarmTime.minute - digit4)/10);     // Calculate minutes' tens as unit.

    displayNumber( digit3, 9, 1);                  // Display minutes' tens in digit 3
    displayNumber( digit4, 13, 1);                 // Display minutes' units in digit 4.

    if (buttonPressed [1] == HIGH)           // If button S1 is pushed then advance to the next stage.
    {
      buttonPressed [1] = LOW;               // Reset button S1
      stage = 13;                            // Advance settings procedure to next stage.
    } 

    if (buttonPressed [3] == HIGH)          // If button S3 is pushed then increase hour by 1.          
    {
      buttonPressed [3] = LOW;                                      // Reset button S3
      alarmTime.hour = byte_wrap(alarmTime.hour,0,23,1);            // Increment hours.
    }

    if (buttonPressed [2] == HIGH)          // If button S2 is pushed then decrease hour by 1.
    {
      buttonPressed [2] = LOW;                                      // Reset button S2.
      alarmTime.hour = byte_wrap(alarmTime.hour,0,23,-1);           // Decrement hours.
    }
      
    break;


    // Set Alarm Minute



  case 13:                              // Stage 13, set the alarm minutes, this will display the current alarm time with the minutes flashing.
    if(halfSeconds == false && buttonPressed [2] == LOW && buttonPressed [3] == LOW && buttonDown [2] == LOW && buttonDown [3] == LOW)
    {                                   // If no buttons have been pushed and it is in a half second, blank the minutes digits
      displayNumber( 10, 9, 1);         // Blank in digit 3.
      displayNumber( 10, 13, 1);        // Blank in digit 4.

    }
    else                                // If in a full second dispaly temporary minutes.
    {
      digit4 = alarmTime.minute % 10;                // Calculate mod of minute ( just minutes' units).
      digit3 = ((alarmTime.minute - digit4)/10);     // Calculate minutes' tens as unit.

      displayNumber( digit3, 9, 1);                  // Display minutes' tens in digit 3
      displayNumber( digit4, 13, 1);                 // Display minutes' units in digit 4.
    }
    digit2 = (alarmTime.hour % 10);                  // Calculate mod of hour( just hours' units)
    digit1 = ((alarmTime.hour - digit2)/10);         // Calculate tens or hours as unit.


    displayNumber( digit1, 0, 1);                    // Display hours' tens in digit 1.
    displayNumber( digit2, 4, 1);                    // Display hours' units in digit 2.



    if (buttonPressed [1] == HIGH)           // If button S1 is pushed then advance to the next stage.
    {
      buttonPressed [1] = LOW;               // Reset button S1
      myMatrix.clear();
      stage = 14;                            // Advance settings procedure to next stage.
    } 

    if (buttonPressed [3] == HIGH)           // If button S3 is pushed then increase minutes by 1.
    {
      buttonPressed [3] = LOW;                                          // Reset button S3
      alarmTime.minute = byte_wrap(alarmTime.minute,0,59,1);            // Increment minutes.
    }

    if (buttonPressed [2] == HIGH)          // If button S2 is pushed then decrease minutes by 1.
    {
      buttonPressed [2] = LOW;                                          // Reset button S2.
      alarmTime.minute = byte_wrap(alarmTime.minute,0,59,-1);           // Decrement minutes.
    }  
    break;

    // Set Alert Type
  case 14:                                  // Stage 14, set the alarm tone, play and display the current tone.
    tune(true);                             // Play current tone tune.
    displayLetter('T', 3, 7);               // Display a 'T'.               
    displayLetter('n', 7, 7);               // Display a 'n' as an abbreviation for "Tune".
    displayNumber( alarmTone, 13, 1);       // Display tone number in digit 4

    if (buttonPressed [1] == HIGH)           // If button S1 is pushed then advance to the final stage.
    {
      buttonPressed [1] = LOW;               // Reset button S1
      alarmTrigger = false;                  // Ensure Alarm trigger is off.
      tune(false);                           // Reset tune position counter and stop playing tune.
      
      stage = 15;                            // Advance settings procedure to final stage.
    } 

    if (buttonPressed [3] == HIGH)          // If button S3 is pushed then increase tone by 1.
    {
      buttonPressed [3] = LOW;                                              // Reset button S3
      tune(false);                                                          // Reset tune position counter
      alarmTime.melody = alarmTone = byte_wrap(alarmTone,0,3,1);            // Decrement alarmtone.
    }

    if (buttonPressed [2] == HIGH)           // If button S2 is pushed then decrease tone by 1.
    {
      buttonPressed [2] = LOW;                                               // Reset button S2.
      tune(false);                                                           // Reset tune position counter
      alarmTime.melody = alarmTone = byte_wrap(alarmTone,0,3,-1);            // Increment alarmtone.
    }  
    break;

  case 15:                                    // Stage 15, finish settings procedure and return to normal clock mode.
    
    alarmTime.snoozeHour = alarmTime.hour;    // Update snooze alarm hour with alarm hours.
    alarmTime.snoozeMinute = alarmTime.minute;// Update alarm minutes.
    saveAlarmTime();                          // Save alarm settings to EEPROM
    settingTime = now();                      // Update time
//    RTC.set(settingTime);                     // Set DS1307 to new time and date settings
    stage = 0;                                // Reset stage for next time settings are called.
    clockMode = 254;
    myMatrix.clear();                         // Clear display.
    intro = true;                             // Set to run animation for intergrating back into clock mode.
    break;
  }
  return;
}
