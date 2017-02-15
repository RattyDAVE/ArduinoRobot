#include "main.h"
#include <avr/sleep.h> //Needed for sleep_mode
#include <avr/power.h> //Needed for powering down perihperals such as the ADC/TWI and Timers
int TwelveHourMode = false;//Set the 12hourMode or 24hoursMode

int show_time_length = 2000;
int show_the_time = false;

//You can set always_on to true and the display will stay on all the time
//This will drain the battery in about 15 hours 

int always_on = false;

long seconds = 00;
int minutes = 00;
int hours =12;

#define RED  1
#define GREEN 2
#define BLUE  3
#define YELLOW  4
int systemColor = BLUE;
int display_brightness = 15000; //A larger number makes the display more dim. This is set correctly below.

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Pin definitions
int digit1 = 9; //Display pin 1
int digit2 = 10; //Display pin 2
int digit3 = A0; //Display pin 6
int digit4 = A1; //Display pin 8

int segA = 6; //Display pin 14
int segB = 8; //Display pin 16
int segC = 5; //Display pin 13
int segD = 11; //Display pin 3
int segE = 13; //Display pin 5
int segF = 4; //Display pin 11
int segG = 7; //Display pin 15

int colons = 12; //Display pin 4
int ampm = 3; //Display pin 10

int theButton = 2;

//The 32.686kHz interrupt handler
SIGNAL(TIMER2_OVF_vect){
  seconds += 8;
  minutes += seconds / 60;
  seconds %= 60;
  hours += minutes / 60; 
  minutes %= 60; 

  //Do we display 12 hour or 24 hour time?
  if(TwelveHourMode == true) {
    
    while(hours > 12) hours -= 12;
  }
  else {
   
    while(hours > 23) hours -= 24;
  }
}

SIGNAL(INT0_vect){
 
  show_the_time = true;
}

void setup() {                
  for(int x = 1 ; x < 18 ; x++){
    pinMode(x, INPUT);
    digitalWrite(x, LOW);
  }

  pinMode(theButton, INPUT); //This is the main button, tied to INT0
  digitalWrite(theButton, HIGH);

  //These pins are used to control the display
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  pinMode(colons, OUTPUT);
  pinMode(ampm, OUTPUT);


  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable();

  ADCSRA &= ~(1<<ADEN); 
  ACSR = (1<<ACD);
  DIDR0 = 0x3F; 
  DIDR1 = (1<<AIN1D)|(1<<AIN0D);

  power_twi_disable();
  power_spi_disable();
  power_timer1_disable();


  //Setup TIMER2
  TCCR2A = 0x00;
  //TCCR2B = (1<<CS22)|(1<<CS20); 
  TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20); 
  ASSR = (1<<AS2); //Enable asynchronous operation
  TIMSK2 = (1<<TOIE2); //Enable the timer 2 interrupt

  //Setup external INT0 interrupt
  EICRA = (1<<ISC01); //Interrupt on falling edge
  EIMSK = (1<<INT0); //Enable INT0 interrupt

  Serial.begin(9600);  
  Serial.println("SainTime:");

  if(systemColor == RED) {
    display_brightness = 1500; //The higher the number, the lower the brightness
    showColor("red ");
  }
  else if(systemColor == GREEN) {
    display_brightness = 1;
    showColor("gren");
  }
  else if(systemColor == BLUE) {
    display_brightness = 1; 
    showColor("bLue");
  }
  else if(systemColor == YELLOW) {
    //display_brightness = 1500;
    display_brightness = 1;
    showColor("yeLo");
  }
  sei(); //Enable global interrupts
}

void loop() {
  if(always_on == false)
    sleep_mode(); 

  if(show_the_time == true || always_on == true) {
    

    while(digitalRead(theButton) == LOW) ; 
    delay(100);
    while(digitalRead(theButton) == LOW) ; 

    showTime(); 

    
    if(digitalRead(theButton) == LOW) setTime();

    show_the_time = false; //Reset the button variable
  }
}

void showTime() {
  int combinedTime = (hours * 100) + minutes; //Combine the hours and minutes

  boolean buttonPreviouslyHit = false;

  long startTime = millis();
  while( (millis() - startTime) < show_time_length) {
    displayNumber(combinedTime, true); 

    if(digitalRead(theButton) == LOW) {
      buttonPreviouslyHit = true;
    }
    else if( (buttonPreviouslyHit == true) && (digitalRead(theButton) == HIGH) ) {

#ifdef ENABLE_TVBGONE
 
      TCCR2A = 0x00;
      TCCR2B = 0;
      ASSR = 0;
      TIMSK2 = 0; 


      sendAllCodes();

      //Setup TIMER2
      TCCR2A = 0x00;
      //TCCR2B = (1<<CS22)|(1<<CS20); //Set CLK/128 or overflow interrupt every 1s
      TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20); //Set CLK/1024 or overflow interrupt every 8s
      ASSR = (1<<AS2); //Enable asynchronous operation
      TIMSK2 = (1<<TOIE2); //Enable the timer 2 interrupt

      quickflashLEDx(2); 
#endif

      return;
    }      
  }

}

void showColor(char *colorName) {
  long startTime = millis();
  while( (millis() - startTime) < show_time_length) {
    displayLetters(colorName); 
    delayMicroseconds(display_brightness);
  }
}

void setTime(void) {

  int idleMiliseconds = 0;
  int buttonHold = 0; 

  while(idleMiliseconds < 2000) {

    cli(); 

  
    minutes += seconds / 60; 
    seconds %= 60;
    hours += minutes / 60;
    minutes %= 60;

    //Do we display 12 hour or 24 hour time?
    if(TwelveHourMode == true) {
      //In 12 hour mode, hours go from 12 to 1 to 12.
      while(hours > 12) hours -= 12;
    }
    else {
      //In 24 hour mode, hours go from 0 to 23 to 0.
      while(hours > 23) hours -= 24;
    }

    sei(); //Resume interrupts

    int combinedTime = (hours * 100) + minutes; 

      for(int x = 0 ; x < 10 ; x++) {
      displayNumber(combinedTime, true);
      delayMicroseconds(display_brightness); 
    }
    for(int x = 0 ; x < 10 ; x++) {
      displayNumber(combinedTime, false); 
      delayMicroseconds(display_brightness); 
    }

    
    if(digitalRead(theButton) == LOW) {
      idleMiliseconds = 0;

      buttonHold++;
      if(buttonHold < 10)
        minutes++;
      else {
      
        minutes /= 10; 
        minutes *= 10; 
        minutes += 10;  
      }
    }
    else
      buttonHold = 0;

    idleMiliseconds += 200;
  }
}
void displayNumber(int toDisplay, boolean displayColon) {

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      break;
    case 2:
      digitalWrite(digit2, DIGIT_ON);
      if(displayColon == true) digitalWrite(colons, DIGIT_ON);
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      break;
    }

    //Now display this digit
    if(digit > 1)
      lightNumber(toDisplay % 10); 
    else if(digit == 1) {
      if( (toDisplay % 10) != 0) 
        lightNumber(toDisplay % 10); 
    }

    toDisplay /= 10;

    delayMicroseconds(2000);
     lightNumber(10);

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
    digitalWrite(colons, DIGIT_OFF);
    digitalWrite(ampm, DIGIT_OFF);
  }

}
void displayLetters(char *colorName) {
#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

  digitalWrite(digit4, DIGIT_OFF);
  digitalWrite(colons, DIGIT_OFF);
  digitalWrite(ampm, DIGIT_OFF);

  for(int digit = 0 ; digit < 4 ; digit++) {
    switch(digit) {
    case 0:
      digitalWrite(digit1, DIGIT_ON);
      break;
    case 1:
      digitalWrite(digit2, DIGIT_ON);
      break;
    case 2:
      digitalWrite(digit3, DIGIT_ON);
      break;
    case 3:
      digitalWrite(digit4, DIGIT_ON);
      break;
    }
    lightNumber(colorName[digit]);
     delayMicroseconds(2000); 
   
    //Turn off all segments
    lightNumber(10);

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
  }
}
void lightNumber(int numberToDisplay) {

#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH

  /*
Segments
   -  A
   F / / B
   -  G
   E / / C
   -  D
   */

  switch (numberToDisplay){

  case 0:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    break;

  case 1:
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    break;

  case 2:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 3:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 4:
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 5:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 6:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 7:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    break;

  case 8:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 9:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 10:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

    /*
Segments
     -  A
     F / / B
     -    G
     E / / C
     - D
     */

    //Letters
  case 'b': //cdefg
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
  case 'L': //def
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    break;
  case 'u': //cde
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    break;

  case 'g': //abcdfg
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
  case 'r': //eg
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
  case 'n': //ceg
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

    //case r
  case 'e': //adefg
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
  case 'd': //bcdeg
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
  case ' ': //None
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 'y': //bcdfg
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;
    //case e 
    //case L
  case 'o': //cdeg
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  }
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=





