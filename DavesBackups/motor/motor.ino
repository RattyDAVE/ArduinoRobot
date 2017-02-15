/*

Pin    Used  Function
1
2 INT 
3 INT  X     Motor2 
4
5      X     Motor3
6      X     Motor4
7
8
9      X    Servo1
10     .    Servo2
11     X    Motor1
12
13

14 A0  X    RightEncoder
15 A1  X    LeftEncoder
16 A2  X    PingOut
17 A3  X    PingIn
18 A4  X    SDA
19 A5  X    SDL

*/

#define DEBUG 1  //Debug Code  1=on 0=off.

//HMC5883L electronic compass
// Reference the I2C Library
#include <Wire.h>
#define I2CSDA 4 //Use Analog Pin 4 as SDA 
#define I2CSDL 5 //Use Analog Pin 5 as SDL
// Reference the HMC5883L Compass Library
#include <HMC5883L.h>
// Store our compass as a variable.
HMC5883L compass;
int heading = 0;

//Servo Section
#include <Servo.h> 
Servo myservo1;  // create servo object to control a servo 
Servo myservo2;  // create servo object to control a servo 
#define Servo1Pin 9
#define Servo2Pin 10

int Servo1Pos=90,Servo2Pos=90,Servo1Angle=0,Servo2Angle=0;
#define servoIncrementValue 30


//Motor Section
/* 
 // motor pins
 
 Digital pin 11: DC Motor #1 / Stepper #1 (activation/speed control)
 Digital pin 3: DC Motor #2 / Stepper #1 (activation/speed control)
 Digital pin 5: DC Motor #3 / Stepper #2 (activation/speed control)
 Digital pin 6: DC Motor #4 / Stepper #2 (activation/speed control)
 
 Digital pin 4, 7, 8 and 12 are used to drive DC motors and Stepper motors via the 74HC595 serial-to-parallel latch
 All these pins are in use if any DC motors or steppers are used
 
 Digital pin 9: Servo #1 control
 Digital pin 10: Servo #2 control
 */

#include <AFMotor.h>
AF_DCMotor motor1(1,MOTOR12_1KHZ);
AF_DCMotor motor2(2,MOTOR12_1KHZ);
AF_DCMotor motor3(3,MOTOR34_1KHZ);
AF_DCMotor motor4(4,MOTOR34_1KHZ);

//AF_DCMotor motor1(1,MOTOR12_64KHZ);
//AF_DCMotor motor2(2,MOTOR12_64KHZ);
//AF_DCMotor motor3(3,MOTOR34_64KHZ);
//AF_DCMotor motor4(4,MOTOR34_64KHZ);

//Sound.
//#define BuzzerPin 14

// ultrasonic 1 
#define PingSensOut 15  // pin for triggering pulse    INPUT
#define PingSensIn 16   // pin for recieving echo      OUPUT
int PingDist;           // how long it takes for the sound to rebound

// map
byte x,y;
const int points=20;
//Set up the world. It is only 40 units by 40 units.
byte mapArray[20][20];
//byte mapArray[15][15];

byte flags; //this a flag varable. Bit 1 is for run the rerouting.
//                     Bit 2 is used in sensorSweep() what way to sweep.

//Initial Starting point for Robot. is 39,39
int currentX=10,currentY=10;

//Pretend multi tasking
#include <TimedAction.h>
TimedAction sweepAction = TimedAction(10,sensorSweep);

#if (DEBUG == 1)
TimedAction printmapAction = TimedAction(10000,printMap);
#endif

//TimedAction turnAction = TimedAction(2000,TurnRight);
//TimedAction stopAction = TimedAction(10000,AllStop);

//Added to test!
int wanted_direction = 0; 
;  //Direction wants to go.
int turndir = 1; // 0=Left 1=Right
int motorspeed = 255;



// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:

#if (DEBUG == 1)
  Serial.begin(9600); //Open Serial Port (Used as a Display for Debuging)
  //  Serial.begin(115200); //Open Serial Port (Used as a Display for Debuging)
  Serial.println("Starting the I2C interface.");

#endif


  Wire.begin(); // Start the I2C interface.

#if (DEBUG == 1)
  Serial.println("Constructing new HMC5883L");
#endif

  compass = HMC5883L(); // Construct a new HMC5883 compass.

#if (DEBUG == 1)
  Serial.println("Setting scale to +/- 1.3 Ga");
#endif

  compass.SetScale(1.3); // Set the scale of the compass.

#if (DEBUG == 1)
  Serial.println("Setting measurement mode to continous.");
#endif

  compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous

    heading = getlocation();

  //pinMode(BuzzerPin, OUTPUT); 
  //tone(BuzzerPin,550,250);  

  //Init the Servos.

  myservo1.attach(Servo1Pin);
  myservo2.attach(Servo2Pin);

  myservo1.write(Servo1Pos);
  myservo2.write(Servo2Pos);

  mapArray[currentX][currentY] = 11;   //Mark current position on map with a 9 for debuging.
  mapArray[1][1] = 240; //This is the GOAL.

  // ultrasonic 1
  pinMode(PingSensOut, OUTPUT);        // initialize the pulse pin as output:
  pinMode(PingSensIn, INPUT);         // initialize the echo_pin pin as an input:

  /* Not Active as dont have the hardware 
   // motors
   motorR.attach(MotorRPin);
   motorL.attach(MotorLPin);
   */

  bitSet(flags, 1); // Set the Flag on Bit 1. This runs the Routing.

#if (DEBUG == 1)
  Serial.println(freeMemory()); //I only have 2000 bytes!!! of SRAM. So have to watch EVERY BYTE!
  Serial.flush();
#endif

}

void loop() {
  // put your main code here, to run repeatedly: 

  sweepAction.check(); //Check if sensorSweep needs to run. (Defined at Start)
#if (DEBUG == 1)
  printmapAction.check(); //Check if sensorSweep needs to run. (Defined at Start)
#endif
  //printMap();

  //  turnAction.check(); //Testing motors.
  //  stopAction.check(); //Testing motors.


  //Check the Flags!
  if (bitRead(flags, 1))  waveFill();

  //testturn();
}

void testturn(){
  //TEST
  //getlocation();
  int heading = getlocation();
  int diff = heading - wanted_direction;   // how many degrees are we off

#if (DEBUG == 1)
  Serial.print("Heading : ");
  Serial.print(heading);
  Serial.print("\tWanted Direction :");
  Serial.print(wanted_direction);
  Serial.print("\tDifference :");
  Serial.print(diff);
#endif


  // modify degress 
  if(diff > 180)
    diff = -360+diff;
  else if(diff < -180)
    diff = 360+diff;


    if(diff > 20) {
    // Turn Left
    turndir = 0;
  } 
  else if(diff < -20) {
    // Turn Right
    turndir = 1;
  }
  else{
    turndir = 2;
  }


#if (DEBUG == 1)
  Serial.print("\tModified Difference :");
  Serial.print(diff);
#endif

#if (DEBUG == 1)
  Serial.print("\tTurn Diection :");
  Serial.print(turndir);
#endif

  diff = map(diff, -180, 180, 0, 255);   // Make the robot turn to its proper orientation

#if (DEBUG == 1)
  Serial.print("\tPower :");
  Serial.print(diff);
#endif

if (diff > 90 || diff < -90)  motorspeed = 255;
else motorspeed = 100;
 
  //motorspeed = map(diff, 0, 45, 0, 255);

#if (DEBUG == 1)
  Serial.print("\tMoving :");
#endif


  if(turndir == 0) {
#if (DEBUG == 1)
    Serial.println("Left");
#endif
    TurnLeft();
  } else if(turndir == 1) {
#if (DEBUG == 1)
    Serial.println("Right");
#endif
    TurnRight();
  } else {
#if (DEBUG == 1)
    Serial.println("Stop");
#endif
    AllStop();
  }


}

void updatemap(int angle, int distance){
  int newX,newY,countdown;

  //Take distance in cm and convert to Feet
  distance = distance / 30;

  //Convert angle from degreese to radians
  newX = currentX + (cos(angle * DEG_TO_RAD) * distance);
  newY = currentY + (sin(angle * DEG_TO_RAD) * distance);

  //Mark on map where the obstacle was.
  if ((newX < points) && (newY < points) && (newX > 0) && (newY > 0)){

    if ((mapArray[newX][newY] >= 22)) {
      //Oh Crap! I have a new object in the Path data.
      mapArray[newX][newY] = 1;
      bitSet(flags, 1); // Set the Flag on Bit 1. This runs the Routing.
    }
    else if (mapArray[newX][newY] < 10) {
      ++mapArray[newX][newY]; 
    }

    //Mark area clear between me and obstacle.
    for (countdown = 0; countdown < distance - 1; countdown = countdown + 1) {
      newX = currentX + (cos(angle * DEG_TO_RAD) * countdown);
      newY = currentY + (sin(angle * DEG_TO_RAD) * countdown);
      if (mapArray[newX][newY] <= 10)
        mapArray[newX][newY] = 0;
    }
  }

  mapArray[currentX][currentY] = 11;
}

void sensorSweep(){  

  bitRead(flags, 2) ? Servo1Pos+=servoIncrementValue : Servo1Pos-=servoIncrementValue; //increment or decrement current position

  if (Servo1Pos>160){
    bitClear(flags, 2);
    Servo1Pos = 160;
  } 
  else if (Servo1Pos < 10){
    bitSet(flags, 2);
    Servo1Pos = 0;
  }

  myservo1.write(Servo1Pos);

  SonarScan();                          //scan for distance measurements

  heading = getlocation();

#if (DEBUG == 1)
  Serial.print(heading);
  Serial.println("degrees\t");
#endif

  //Need to Add on Compass
  Servo1Angle = -(Servo1Pos - 90);
  heading = getlocation();

#if (DEBUG == 1)
  Serial.print(Servo1Pos);
  Serial.print(" Servo1Pos\t");
  Serial.print(Servo1Angle);
  Serial.print(" Servo1Angle\t");
#endif

  Servo1Angle = -(Servo1Pos - 90) + heading;
  if (Servo1Angle > 360) Servo1Angle -= 360;

  if (DEBUG == 1) {
    Serial.print(Servo1Angle);
    Serial.println(" Servo1Heading\t");
  }

  updatemap(Servo1Angle,PingDist);
}

int SonarScan(){
  digitalWrite(PingSensOut, LOW);
  delayMicroseconds(2);
  digitalWrite(PingSensOut, HIGH);
  delayMicroseconds(10);
  digitalWrite(PingSensOut, LOW);
  PingDist = pulseIn(PingSensIn,10) / 29.1 / 2;

  //  tone(BuzzerPin, map(PingDist, 167, 1, 120, 1500),map(PingDist, 167, 1, 1000, 300));
  //tone(BuzzerPin, map(PingDist, 167, 1, 120, 1500),50);

#if (DEBUG == 1)
  if (PingDist <= 0) {
    Serial.println("Out of range");
  }
  else {
    Serial.print(PingDist);
    Serial.print("cm");
    Serial.println();
    Serial.flush();
  }
#endif

  return(PingDist);
}

//Below is used for the freeMemory function. I am not going to explain this at all. As far too complicated!
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;
int freeMemory() {
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}

void waveFill()
{
  int minVal;
  int reset_minVal = 13;  //Lowest value to go to. 1-10 = Used for Objects, 11 = Robot, 240 = goal
  for (int i = 0; i < 239; i++)   //Scroll through find solution "i" times, IF IT LEAVES WHITE WITHOUT SOLVING INCREASE THIS
  {
    for (x = 0; x < points; x++)   //Scroll down the MAP Array
    {
      for (int y = 0; y < points; y++)   //Scroll across the MAP Array
      {     
        //if Location is a clear or the Goal
        if (mapArray[x][y] == 0 || mapArray[x][y] == 11) //THEN FIND THE HIGHEST VALUE AROUND CURRENT COORDINATE
        {
          minVal = reset_minVal;  //Clear minVal data from last round
          //Right   ***************************************************************
          if (x < points - 1)//not out of boundary
            if (mapArray[x + 1][y] > minVal && mapArray[x + 1][y] < 241) 
            {   //IF TO THE RIGHTS VALUE IS GREATER THAN minVal & IS IN RANGE
              minVal = mapArray[x + 1][y];
            }
          //Left   ****************************************************************
          if (x > 0)
            if (mapArray[x - 1][y] > minVal && mapArray[x - 1][y] < 241) 
            {   //IF TO THE LEFTS VALUE IS GREATER THAN minVal & IS IN RANGE
              minVal = mapArray[x - 1][y];
            }
          //Down  *****************************************************************
          if (y < points - 1)
            if (mapArray[x][y + 1] > minVal && mapArray[x][y + 1] < 241) 
            {   //IF BELOWS VALUE IS GREATER THAN minVal & IS IN RANGE
              minVal = mapArray[x][y + 1];
            }
          //Up  *******************************************************************
          if (y > 0)
            if (mapArray[x][y - 1] > minVal && mapArray[x][y - 1] < 241) 
            {   //IF ABOVES VALUE IS GREATER THAN minVal & IS IN RANGE
              minVal = mapArray[x][y - 1];
            }

          //If Value > reset_minVal && Location is a Robot. SOLVED & EXIT
          if (minVal > reset_minVal && mapArray[x][y] == 11) 
          {
            return;
          }
          else if (minVal != reset_minVal)  //If Value doesn't equal the reset value change current coordinate with the min value found - 1
          {
            mapArray[x][y] = minVal - 1;
          }
        }
      }
    }
  }
  //0 = Clear, 1-10 = Object, 11 = Robot, 240 = Goal, Above 240 Reserved for Upgrades
  bitClear(flags, 1); // Clear the Flag on Bit 1. We have done the routing.
}

void clearMap()
{
  for (x = 0; x < points; x++)   //Scroll down the MAP Array
  {
    for (y = 0; y < points; y++)   //Scroll across the MAP Array
    {
      if (mapArray[x][y] > 11 && mapArray[x][y] < 241)  //If the map Coordinate is between 11 & 240 reset to Zero
      {
        mapArray[x][y] = 0;
      }
    }
  }
}

#if (DEBUG == 1)
void printMap(){
  //Print out the MAP!
  Serial.println();
  for (y = 0; y < points; y = y + 1) {
    Serial.print(y);
    Serial.print("\t");
    for (x = 0; x < points; x = x + 1) {
      if (mapArray[x][y] < 10) { 
        Serial.print(" ");
      }
      if (mapArray[x][y] < 100) { 
        Serial.print(" ");
      }
      Serial.print(mapArray[x][y]);
      Serial.print(" ");
    }
    Serial.println();
    Serial.flush();
  }
}
#endif

// getlocation() is the HMC5883L
int getlocation(){
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  int MilliGauss_OnThe_XAxis = scaled.XAxis;// (or YAxis, or ZAxis)
  float heading = atan2(scaled.YAxis, scaled.XAxis);

  if(heading < 0) heading += 2*PI;
  heading = (heading * 180/M_PI);
  return(heading); 
}

int rotatetoheading(int gotoheading){
  return(1);
}

void TurnRight(){
  motor1.setSpeed(motorspeed);
  motor2.setSpeed(motorspeed);
  motor3.setSpeed(motorspeed);
  motor4.setSpeed(motorspeed);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}

void TurnLeft(){
  motor1.setSpeed(motorspeed);
  motor2.setSpeed(motorspeed);
  motor3.setSpeed(motorspeed);
  motor4.setSpeed(motorspeed);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
}

void Forward(){
  motor1.setSpeed(motorspeed);
  motor2.setSpeed(motorspeed);
  motor3.setSpeed(motorspeed);
  motor4.setSpeed(motorspeed);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void Backward(){
  motor1.setSpeed(motorspeed);
  motor2.setSpeed(motorspeed);
  motor3.setSpeed(motorspeed);
  motor4.setSpeed(motorspeed);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}


void AllStop(){
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  motor3.setSpeed(0);
  motor4.setSpeed(0);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
