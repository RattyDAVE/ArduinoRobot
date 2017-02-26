//Pins

/*
  | Digital | Analog | Function | Notes |
  | ------- | ------ | -------- | ----- |
  D0        |        |RX        |Serial
  D1        |        |TX        |Serial
  D2        ||INT0   |Not used - Reserved - Wheel Encoder (L)
  D3        ||INT1   |Not used - Reserved - Wheel Encoder (R)
  D4        |
  D5        |
  D6        |
  D7        |
  D8        |
  D9        |        |          |MotorShield Servo S2
  D10       |        |SS        |IR Receiver - (MotorShield Servo S1 NOT USED)
  D11       |        |MOSI
  D12       |        |MISO
  D13       |        |SCK
  D14       |A0
  D15       |A1
  D16       |A2
  D17       |A3
  D18       |A4      |SDA       |Shared by MotorShield and Line Sensor (i2c)
  D19       |A5      |SCK       |Shared by MotorShield and Line Sensor (i2c)

  ## I2C Bus Address
  | Address | Module |
  |---      |---     |
  0x60      |Motorshield
  0x9       |Line Sensor
  0x3c      |Display
*/

#define DEF_IR_PIN 10
#define DEF_SERVO_PIN 9

// MOTOR
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
#define DEF_KP 1
#define DEF_KD 5
#define DEF_M1_DEFAULT_SPEED 100
#define DEF_M2_DEFAULT_SPEED 100
#define DEF_M1_MAX_SPEED 255
#define DEF_M2_MAX_SPEED 255

// REMOTE
#include <IRremote.h>
IRrecv IR_Receiver(DEF_IR_PIN);
decode_results IR_Decoder;

// Car MP3 Remote : NEC Codes.
#define DEF_REMOTE_PROTOCOL NEC
#define DEF_REMOTE_CH_DOWN  0xffa25d
#define DEF_REMOTE_CH       0xff629d
#define DEF_REMOTE_CH_UP    0xffe21d
#define DEF_REMOTE_PRE      0xff22dd
#define DEF_REMOTE_NEXT     0xff02fd
#define DEF_REMOTE_PLAY     0xffc23d
#define DEF_REMOTE_VOL_DOWN 0xffe01f
#define DEF_REMOTE_VOL_UP   0xffa857
#define DEF_REMOTE_EQ       0xff906f
#define DEF_REMOTE_0        0xff6897
#define DEF_REMOTE_100      0xff9867
#define DEF_REMOTE_200      0xffb04f
#define DEF_REMOTE_1        0xff30cf
#define DEF_REMOTE_2        0xff18e7
#define DEF_REMOTE_3        0xff7a85
#define DEF_REMOTE_4        0xff10ef
#define DEF_REMOTE_5        0xff38c7
#define DEF_REMOTE_6        0xff5aa5
#define DEF_REMOTE_7        0xff42bd
#define DEF_REMOTE_8        0xff4ab5
#define DEF_REMOTE_9        0xff52ad
#define DEF_REMOTE_REPEAT   0xffffffff
int IR_previous; //Used to store previous IR coded. Used for repeat.

//Servo
#include <Servo.h>
Servo My_Servo;  // create servo object to control a servo
int servo_pos = 90;
int servo_speed = 45;

// Line Follower
#include <SunFounderLineFollower.h>
#define DEF_SENSITIVITY 10000
SunFounderLineFollower sflf;
int lastError = 0;
int olddir = 0;

//Display
#define DISPLAY_I2C_ADDRESS 0x3C
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
SSD1306AsciiWire oled;
int i;

void setup() {
  
  oled.begin(&Adafruit128x64, DISPLAY_I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.println("Setup");
  oled.setScroll(true);

  oled.println("Start MotorShield");
  AFMS.begin();
  set_motors(0, 0);

  oled.println("Start IR");
  IR_Receiver.enableIRIn(); // Start the receiver

  oled.println("Start Servo");
  My_Servo.attach(DEF_SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  My_Servo.write(servo_pos); // Set initial position

  oled.println("Setup End");
  
}

void loop() {
  oled.println(i++);

   
  if (IR_Receiver.decode(&IR_Decoder)) IR_Do_Stuff();
  My_Servo.write(servo_pos);

  int error = readLine(DEF_SENSITIVITY);
  //int motorSpeed = DEF_KP * error + DEF_KD * (error - lastError);

  int motorSpeed = (error * DEF_KP);

  lastError = error;

  int leftMotorSpeed = DEF_M1_DEFAULT_SPEED + motorSpeed;
  int rightMotorSpeed = DEF_M2_DEFAULT_SPEED - motorSpeed;
  // set motor speeds using the two motor speed variables above
  set_motors(leftMotorSpeed, rightMotorSpeed);
}

void set_motors(int motor1speed, int motor2speed) {
  if (motor1speed > DEF_M1_MAX_SPEED) motor1speed = DEF_M1_MAX_SPEED; // limit top speed
  if (motor2speed > DEF_M2_MAX_SPEED) motor2speed = DEF_M2_MAX_SPEED; // limit top speed
  if (motor1speed < 0) motor1speed = 0; // keep motor above 0
  if (motor2speed < 0) motor2speed = 0; // keep motor speed above 0
  motor1->setSpeed(motor1speed); // set motor speed
  motor2->setSpeed(motor2speed); // set motor speed
  motor1->run(FORWARD);
  motor2->run(FORWARD);
}

int readLine(int sensitivity) {
  int retdir = 0;
  int byteprocessed = sflf.byteprocessed(sensitivity);

  //NO LINE! USE OLD VALUE!
  if (byteprocessed == B00000000) retdir = olddir;
  else if (byteprocessed == B10000000) retdir = -64;
  else if (byteprocessed == B11000000) retdir = -32;
  else if (byteprocessed == B01000000) retdir = -16;
  else if (byteprocessed == B01100000) retdir = -8;
  else if (byteprocessed == B00100000) retdir = -4;
  else if (byteprocessed == B00110000) retdir = -2;
  else if (byteprocessed == B00010000) retdir = -1;
  //dead center
  else if (byteprocessed == B00011000) retdir = 0;
  else if (byteprocessed == B00001000) retdir = 1;
  else if (byteprocessed == B00001100) retdir = 2;
  else if (byteprocessed == B00000100) retdir = 4;
  else if (byteprocessed == B00000110) retdir = 8;
  else if (byteprocessed == B00000010) retdir = 16;
  else if (byteprocessed == B00000011) retdir = 32;
  else if (byteprocessed == B00000001) retdir = 64;

  //Got some other result! Calibration wrong.
  //For now we continue as before and hope the next read sorts itself out!
  else retdir = olddir;

  olddir = retdir;

  return retdir;
}

void IR_Do_Stuff() {
  int remote_code;
  
  remote_code = IR_Decoder.value;

  if (remote_code == DEF_REMOTE_REPEAT) remote_code = IR_previous;

  switch (remote_code) {
    case DEF_REMOTE_CH_DOWN:
      // Do stuff
      break;
    case DEF_REMOTE_CH:
      // Do stuff
      break;
    case DEF_REMOTE_CH_UP:
      // Do stuff
      break;
    case DEF_REMOTE_PRE:
      servo_pos = min(180, servo_pos + servo_speed);
      // Do stuff
      break;
    case DEF_REMOTE_NEXT:
      servo_pos = min(180, servo_pos - servo_speed);
      // Do stuff
      break;
    case DEF_REMOTE_PLAY:
      // Do stuff
      break;
    case DEF_REMOTE_VOL_DOWN:
      // Do stuff
      break;
    case DEF_REMOTE_VOL_UP:
      // Do stuff
      break;
    case DEF_REMOTE_EQ:
      // Do stuff
      break;
    case DEF_REMOTE_0:
      // Do stuff
      break;
    case DEF_REMOTE_100:
      // Do stuff
      break;
    case DEF_REMOTE_200:
      // Do stuff
      break;
    case DEF_REMOTE_1:
      // Do stuff
      break;
    case DEF_REMOTE_2:
      // Do stuff
      break;
    case DEF_REMOTE_3:
      // Do stuff
      break;
    case DEF_REMOTE_4:
      // Do stuff
      break;
    case DEF_REMOTE_5:
      // Do stuff
      break;
    case DEF_REMOTE_6:
      // Do stuff
      break;
    case DEF_REMOTE_7:
      // Do stuff
      break;
    case DEF_REMOTE_8:
      // Do stuff
      break;
    case DEF_REMOTE_9:
      // Do stuff
      break;
  }
  IR_previous = remote_code;
  IR_Receiver.resume();
}
