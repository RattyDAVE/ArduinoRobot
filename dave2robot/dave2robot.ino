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
D9        |        |          |Not used - Reserved - MotorShield Servo S2
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
 */

#define DEF_IR_PIN 10


// MOTOR
#include <Wire.h>
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);

#define DEF_SENSITIVITY 10000

#define DEF_KP 1
#define DEF_KD 5
#define DEF_M1_DEFAULT_SPEED 100
#define DEF_M2_DEFAULT_SPEED 100
#define DEF_M1_MAX_SPEED 255
#define DEF_M2_MAX_SPEED 255

// REMOTE
#include <IRremote.h>
IRrecv My_Receiver(DEF_IR_PIN);
//IRdecode My_Decoder;
decode_results My_Decoder;
//decode_results results;

// Car MP3 Remote : NEC Codes.
#define DEF_REMOTE_PROTOCOL NEC
#define DEF_REMOTE_CH_DOWN  0xFFA25D
#define DEF_REMOTE_CH       0xFF629D
#define DEF_REMOTE_CH_UP    0xFFE21D
#define DEF_REMOTE_PRE      0xFF22DD
#define DEF_REMOTE_NEXT     0xFF02FD
#define DEF_REMOTE_PLAY     0xFFC23D
#define DEF_REMOTE_VOL_DOWN 0xFFE01F
#define DEF_REMOTE_VOL_UP   0xFFA857
#define DEF_REMOTE_EQ       0xFF906F
#define DEF_REMOTE_0        0xFF6897
#define DEF_REMOTE_100      0xFF9867
#define DEF_REMOTE_200      0xFFB04F
#define DEF_REMOTE_1        0xFF30CF
#define DEF_REMOTE_2        0xFF18E7
#define DEF_REMOTE_3        0xFF7A85
#define DEF_REMOTE_4        0xFF10EF
#define DEF_REMOTE_5        0xFF38C7
#define DEF_REMOTE_6        0xFF5AA5
#define DEF_REMOTE_7        0xFF42BD
#define DEF_REMOTE_8        0xFF4AB5
#define DEF_REMOTE_9        0xFF52AD
#define DEF_REMOTE_REPEAT   0xFFFFFF
int Previous; //Used to store previous IR coded. Used for repeat.



// Line Follower
#include <SunFounderLineFollower.h>
SunFounderLineFollower sflf;
int lastError = 0;
int olddir = 0;

void setup() {
    //Serial.begin(230400);
    AFMS.begin();
    My_Receiver.enableIRIn(); // Start the receiver
    set_motors(0, 0);
}

void loop() {

    //  if (My_Receiver.GetResults(&My_Decoder)) {
    if (My_Receiver.decode(&My_Decoder)) {
        //    My_Decoder.decode();
        if (My_Decoder.decode_type == DEF_REMOTE_PROTOCOL) {
            if (My_Decoder.value == DEF_REMOTE_REPEAT) My_Decoder.value = Previous;
            switch (My_Decoder.value) {
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
                    // Do stuff
                    break;
                case DEF_REMOTE_NEXT:
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
            int Previous = My_Decoder.value;
        }
        My_Receiver.resume();
    }

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

