# ArduinoRobot

## Items Ordered.

See https://github.com/RattyDAVE/ArduinoRobot/wiki/Items-Ordered

## Library files

See https://github.com/RattyDAVE/ArduinoRobot/wiki/Library-files

## Notes

Motorshield v2
https://learn.adafruit.com/adafruit-motor-shield-v2-for-arduino/overview

Infrared Remote Control 
https://github.com/z3t0/Arduino-IRremote/wiki

Sonar
http://playground.arduino.cc/Code/NewPing

Line Follower
https://www.sunfounder.com/wiki/index.php?title=Line_Follower_Module
https://learn.sparkfun.com/tutorials/sparkfun-line-follower-array-hookup-guide


Basics of what you need! (won't work directly!)
http://samvrit.tk/tutorials/pid-control-arduino-line-follower-robot/?i=1

Maybe can customise this https://learn.sparkfun.com/tutorials/sparkfun-line-follower-array-hookup-guide
http://www.techbitar.com/fast-line-following-robot.html
https://diyhacking.com/projects/DIY_LineFollower.ino
http://junkplusarduino.blogspot.co.uk/p/robot-2.html


## Milestones

- Using the Motor shield and Arduino only. Move the robot in a SQUARE.  Forward for 5 seconds. Turn Right 90 deg. Forward for 5 seconds. 	Turn Right 90 deg. Forward for 5 seconds. Turn Right 90 deg.	Forward for 5 seconds. Turn Right 90 deg. So roughly at the start point. Closest to start point wins.
- Same as 1 but LEFT 90 deg. Closest to start point wins.
- 1) and 2) combined in a random selection. Dice will be thrown for number of LEFT turns another dice for number of right turns. These will then go in to a “hat” and pulled out so in a random order. EG. 1st Dice (for Left) is 3, 2nd Dice (For right) is 5. 3 bits of paper marked with a L are put in the bag and 5 are written with R. Then they are mixed up and pulled out. So you may get the pattern RLRLRLRR. Closest to start point wins.
- Using the Line sensor follow a straight line and stop when the line ends within the stop zone that is 15cm. QUICKEST wins.
- Same as 4) but with corners. (Radius of no more than 15cm). QUICKEST wins (within 15cm stop zone).

## Pins Used.


| Digital | Analog | Function | Notes |
| ------- | ------ | -------- | ----- |
D0||RX|Serial 
D1||TX|Serial
D2||INT0|Not used - Reserved - Wheel Encoder (L)
D3||INT1|Not used - Reserved - Wheel Encoder (R)
D4|
D5|
D6|
D7|
D8|
D9|||Not used - Reserved - MotorShield Servo S2
D10||SS|Not used - Reserved - MotorShield Servo S1
D11||MOSI
D12||MISO
D13||SCK
D14|A0
D15|A1
D16|A2
D17|A3
D18|A4|SDA|Shared by MotorShield and Line Sensor (i2c) 
D19|A5|SCK|Shared by MotorShield and Line Sensor (i2c)

## I2C Bus Address
| Address | Module |
|---|---|
0x60|Motorshield
0x9|Line Sensor

