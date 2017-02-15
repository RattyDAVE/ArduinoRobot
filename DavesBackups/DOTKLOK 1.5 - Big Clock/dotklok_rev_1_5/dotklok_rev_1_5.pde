 /*********************************************************************************
 * 
 * DOTKLOK rev. 1.5
 * Andrew O'Malley, 15 December 2011
 * aomalley_AT_gmail_DOT_com
 *
 * AVAILABLE AS A FULL KIT FROM WWW.AOMALLEY.ORG/DOTKLOK
 * Check www.aomalley.org/dotklok for latest code updates
 * 
 * Animated clock with SURE 24x16 display, based on Electric Window 3
 * 
 * Clock animations by Andrew O'Malley    
 *
 * Display interface from demo16x24.c by Bill Westfield ("WestfW")
 * further modified by Andrew O'Malley
 *
 * Uses RTClib from Adafruit.com
 * Uses Button.h from arduino.cc (by Alexander Brevig)
 * Uses Wire.h from arduino.cc (included w/ Arduino IDE)
 *
 * This code comes with no implied warranty
 * Released under Creative Commons Attribution-ShareAlike (CC BY-SA 3.0) license
 * see http://creativecommons.org/licenses/by-sa/3.0/
 *
/********************************************************************************/


/*** DEFINES AND GLOBAL VARIABLES ***/

boolean DEBUG=false ; // can be forced true by pressing B4 during setup
boolean PAUSE=false; // if true, use B5 to pause the clock for taking photographs

int animation=1;
#define ani_max 12

char REV[]="1.5";

#include <avr/pgmspace.h>
#include <Wire.h>
#include <RTClib.h>
#include <Button.h>
#include "ht1632.h"
#include "fonts.h"
#include "font.h"

#define X_MAX 23          // horizontal display size in pixels (number of screens*24 -1) 23 or 47 or 71
#define Y_MAX 15          // vertical display size in pixels
#define DELAY_MIN 5       // min delay in millis for frame rate in display functions
#define DELAY_MAX 2000    // max delay in millis for frame rate in display functions
#define ASSERT(condition) // ???

#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define X 0
#define Y 1

#define BUTTON 50 // delay used to "debounce" buttons

// global clock variables
RTC_DS1307 RTC;      
DateTime time_now;
DateTime time_prev;

// Button mapping
Button b1 = Button(6,PULLUP);
Button b2 = Button(5,PULLUP);
Button b3 = Button(8,PULLUP);
Button b4 = Button(7,PULLUP);
Button b5 = Button(4,PULLUP);
Button buttons[] = { b1, b2, b3, b4, b5 };

unsigned long int b1_time;
unsigned long int b2_time;
unsigned long int b3_time;
unsigned long int b4_time;

byte display[X_MAX+1][Y_MAX+1];  // AO's display matrix

boolean twelve_hour=false;

/*** UTILITY & DISPLAY FUNCTIONS ***/

// debugging tool only:
// used to estimate available mem
// obtainted from arduino.cc forum
// not sure how accurate this is
int availableMemory()
{
  int size = 8192;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
} 

// tests all the pixels on the screen
// used during kit packing to test screens before shipping
void screenTest()
{
  for(int y=0; y<=Y_MAX; y++){
    for(int x=0; x<=X_MAX; x++){
      plot(x, y, 1);
    }
  }
  delay(1000);
  for(int x=0; x<=X_MAX; x++){
    for(int y=0; y<=Y_MAX; y++){
      plot(x, y, 0);
    }
  }
  delay(500);
  ht1632_clear();
}

// shift the display frame left, clears the rightmost column
void shift_display_left() {
  for(int x=0; x<X_MAX; x++){
    for(int y=0; y<=Y_MAX; y++){
      display[x][y]=display[x+1][y];
    }
  }
  for(int y=0; y<=Y_MAX; y++){
    display[X_MAX][y]=0;
  }
}

// shift the display frame right, clears the leftmost column
void shift_display_right() {
  for(int x=X_MAX; x>0; x--){
    for(int y=0; y<=Y_MAX; y++){
      display[x][y]=display[x-1][y];
    }
  }
  for(int y=0; y<=Y_MAX; y++){
    display[0][y]=0;
  }
}

// shift the display frame down, clears the top row
void shift_display_down() {
  for(int x=0; x<=X_MAX; x++){
    for(int y=Y_MAX; y>0; y--){
      display[x][y]=display[x][y-1];
    }
  }
  for(int x=0; x<=X_MAX; x++){
    display[x][0]=0;
  }
}

// shift the display frame up, clears the bottom row
void shift_display_up() {
  for(int x=0; x<=X_MAX; x++){
    for(int y=0; y<Y_MAX; y++){
      display[x][y]=display[x][y+1];
    }
  }
  for(int x=0; x<=X_MAX; x++){
    display[x][Y_MAX]=0;
  }
}

// clear column "col"
// only updates the display buffer, not the actual display
void clear_column(byte col)  
{
  if(col<0 || col>X_MAX) return;

  for(int y=0; y<=Y_MAX; y++){
    display[col][y]=0;
  }
}

// clear row "row"
// only updates the display buffer, not the actual display
void clear_row(byte row)  
{
  if(row<0 || row>Y_MAX) return;

  for(int x=0; x<=X_MAX; x++){
    display[x][row]=0;
  }
}

// update the display based on the (global) display[][] array
void update_display() {
  for(int x = 0; x <= X_MAX; x++) {
    for (int y = 0; y <= Y_MAX; y++) {
      plot(x, y, display[x][y]);
    }
  }
}

void clear_shift_right()
{
  //vw_rx_stop();
  for(int x=X_MAX; x>=0; x--) {
    for(int y=0; y<Y_MAX+1; y++) { 
      display[x][y]=0;
      //update_display();
      //delay(10);
    }
    update_display();
    delay(random(1,250));
  }
  //vw_rx_start();
}

void clear_shift_left()
{
  //vw_rx_stop();
  for(int x=0; x<X_MAX+1; x++) {
    for(int y=0; y<Y_MAX+1; y++) { 
      display[x][y]=0;
      //update_display();
      //delay(10);
    }
    update_display();
    delay(random(1,250));
  }
  //vw_rx_start();
}

void clear_display()
{
  for(int x=0; x<=X_MAX; x++) {
    for(int y=0; y<=Y_MAX; y++) {
      display[x][y]=0;
    }
  }
  //update_display();
  ht1632_clear();
}

void clear_display_buffer()
{
  for(int x=0; x<X_MAX+1; x++) {
    for(int y=0; y<Y_MAX+1; y++) {
      display[x][y]=0;
    }
  }
}

// ANIMATION FUNCTIONS 

// change_animation
// used to check the animation changing buttons
// updates the animation number
boolean change_animation(){
  
    //if(DEBUG) { Serial.println("change_animation: begin"); }

    if(!b1.isPressed() && !b2.isPressed() && b3.uniquePress()){
      animation--;
      if(animation<1)  animation=ani_max; // roll-over
      //if(DEBUG) { Serial.print("change_animation / animation: "); Serial.println(animation); }
      delay(BUTTON); // helps debounce buttons
      return true;
    }
    if(!b1.isPressed() && !b2.isPressed() && b4.uniquePress()){
      animation++;
      if(animation>ani_max)  animation=1; // roll-over
      //if(DEBUG) { Serial.print("change_animation / animation: "); Serial.println(animation); }      
      delay(BUTTON); // helps debounce buttons
      return true;
    }
    
    return false;
}  

// midnight_random()
// used to check if midnight
// and change animation randomly if the B5 (slide switch on back of klok) is activated
boolean midnight_random(){
  
  int prev_anim=animation;
 
  if( b5.isPressed() ){
    if( time_now.hour()==0 && time_prev.hour()==23 ){ 
      if(DEBUG) Serial.print("MIDNIGHT RANDOM ");
      while(prev_anim == animation)  animation = random(1,ani_max+1);
      if(DEBUG && prev_anim==1)  animation=11;
      if(DEBUG) Serial.println(animation);
      return true;
    }
  }
 
 return false;
} 

  
// puttime_3x5
// place the time in 3x5 font
void puttime_3x5(int x_hour, int y_hour, int x_min, int y_min, byte hour, byte minute){

  if(twelve_hour){
    if(hour>12)  hour-=12;
    if(hour==0)  hour=12;
  }
  
  // hours
  if(hour/10==1){
    putchar_3x5(x_hour,y_hour,' '); // erase the prev 0
    putchar_3x5(x_hour+1,y_hour,hour/10+48); // slide the 1 over
  }
  else{
    if(twelve_hour)  putchar_3x5(x_hour,y_hour,' ');
    else             putchar_3x5(x_hour,y_hour,hour/10+48);
  }
  
  if(hour%10==1){
    putchar_3x5(x_hour+4,y_hour,' '); // erase the prev 0
    putchar_3x5(x_hour-1+4,y_hour,hour%10+48); // slide the 1 back
  }
  else  putchar_3x5(x_hour+4,y_hour,hour%10+48);

  // minutes
  if(minute/10==1){
    putchar_3x5(x_min,y_min,' '); // erase the prev 0
    putchar_3x5(x_min+1,y_min,minute/10+48); // slide the 1 over
  }
  else  putchar_3x5(x_min,y_min,minute/10+48);

  if(minute%10==1){ 
    putchar_3x5(x_min+4,y_min,' '); // erase the prev 0
    putchar_3x5(x_min-1+4,y_min,minute%10+48); // slide thet 1 back
  }
  else putchar_3x5(x_min+4,y_min,minute%10+48);

} // end function

// puttime_4x7
// place the time in 4x7 font
void puttime_4x7(int x_hour, int y_hour, int x_min, int y_min, byte hour, byte minute){

  // hours
  if(twelve_hour){
    if(hour>12)       hour-=12;
    else if(hour==0)  hour=12;
    
    if(hour>9)  putchar_4x7(x_hour,y_hour,hour/10+48);
    else        putchar_4x7(x_hour,y_hour,' ');
    
    putchar_4x7(x_hour+5,y_hour,hour%10+48);
  }
  else{
    putchar_4x7(x_hour,y_hour,hour/10+48);
    putchar_4x7(x_hour+5,y_hour,hour%10+48);
  }

  // minutes
  putchar_4x7(x_min,y_min,minute/10+48);
  putchar_4x7(x_min+5,y_min,minute%10+48);

} // end function

/*
 * Copy a character glyph from the font_4x7 data structure to
 * the display[][] matirx, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void bufchar_4x7(byte x, byte y, char c)
{
  byte dots;
  if (c >= 'A' && c <= 'Z' ||
    (c >= 'a' && c <= 'z') ) {
    c &= 0x1F;   // A-Z maps to 1-26
  } 
  else if (c >= '0' && c <= '9') {
    //c = (c - '0') + 27;
    c = (c - '0');
  } 
  else if (c == ' ') {
    c = 10; // space
  }
  for (char col=0; col< 4; col++) {
    dots = pgm_read_byte_near(&font_4x7[c][col]);
    for (char row=0; row < 7; row++) {
      if (dots & (64>>row))   	     // only 7 rows.
        //plot(x+col, y+row, 1);
        display[x+col][y+row]=1;
      else 
        //plot(x+col, y+row, 0);
        display[x+col][y+row]=0;
    }
  }
}

// buftime_4x7
// place the time in 4x7 font into the display[][] matrix
void buftime_4x7(int x_hour, int y_hour, int x_min, int y_min, byte hour, byte minute){

  // hours
  if(twelve_hour){
    if(hour>12)       hour-=12;
    else if(hour==0)  hour=12;
    
    if(hour>9)  bufchar_4x7(x_hour,y_hour,hour/10+48);
    else        bufchar_4x7(x_hour,y_hour,' ');
    
    bufchar_4x7(x_hour+5,y_hour,hour%10+48);
  }
  else{
    bufchar_4x7(x_hour,y_hour,hour/10+48);
    bufchar_4x7(x_hour+5,y_hour,hour%10+48);
  }

  // minutes
  bufchar_4x7(x_min,y_min,minute/10+48);
  bufchar_4x7(x_min+5,y_min,minute%10+48);

} // end function

// puttime_5x12
// place the time in 5x12 font
void puttime_5x12(int x_hour, int y_hour, int x_min, int y_min, byte hour, byte minute){

  // hours
  if(twelve_hour){
    if(hour>12)       hour-=12;
    else if(hour==0)  hour=12;
    
    if(hour>9){
      putchar_5x12(x_hour+3,y_hour,' '); // clears any center characters from single digit 12-hour mode
      putchar_5x12(x_hour,y_hour,hour/10+48);
      putchar_5x12(x_hour+6,y_hour,hour%10+48);
    }
    else{
      putchar_5x12(x_hour,y_hour,' '); // clears double digits before placing centered single digit
      putchar_5x12(x_hour+6,y_hour,' ');
      putchar_5x12(x_hour+3,y_hour,hour%10+48);
    }
    
  }
  else{
    putchar_5x12(x_hour+3,y_hour,' '); // clears any center characters from single digit 12-hour mode
    putchar_5x12(x_hour,y_hour,hour/10+48);
    putchar_5x12(x_hour+6,y_hour,hour%10+48);
  }

  // minutes
  putchar_5x12(x_min,y_min,minute/10+48);
  putchar_5x12(x_min+6,y_min,minute%10+48);

} // end function

void put_sprite(int x, int y, const uint8_t sprite[8])
{
  byte dots;

  for (char col=0; col<= 7; col++) {
    dots = pgm_read_byte_near(&sprite[col]);
    for (char row=0; row <= 7; row++) {
      if (dots & (128>>row)){   
        //if(x+row>=0 && x+row<=X_MAX && y+col>=0 && y+col<=Y_MAX) plot(x+row, y+col, dots & (128>>row));
        if(x+row>=0 && x+row<=X_MAX && y+col>=0 && y+col<=Y_MAX) plot(x+row, y+col, 1);
      }
      //else 
      //if(x>=0 && x<=X_MAX && y>=0 && y<=Y_MAX) plot(x+row, y+col, 0);
      //plot(x+col, y+row, dots);
    }
  }
}

void del_sprite(int x, int y, const uint8_t sprite[8])
{
  byte dots;

  for (char col=0; col<= 7; col++) {
    dots = pgm_read_byte_near(&sprite[col]);
    for (char row=0; row <= 7; row++) {
      if (dots & (128>>row)){
        if(x+row>=0 && x+row<=X_MAX && y+col>=0 && y+col<=Y_MAX) plot(x+row, y+col, 0);
      }
    }
  }
}

// invader()
// a space invader randomly wanders in and out of the screen
// used in game_time() animation
void invader()
{

  int steps=0;
  int x;
  int y;
  int x_inc;
  int y_inc;

  int this_sprite=random(0,2);  
  int start=random(0,4);

  switch(start) {

  case 0:  // start in the upper left corner
    x=-8;
    x_inc=1;
    y=-8;
    y_inc=1;
    break;
  case 1: // start in the upper right corner
    x=X_MAX+1;
    x_inc=-1;
    y=-8;
    y_inc=1;
    break;
  case 2: // start in the lower left corner
    x=-8;
    x_inc=1;
    y=Y_MAX+1;
    y_inc=-1;
    break;
  case 3: // start in the lower right corner
    x=X_MAX+1;
    x_inc=-1;
    y=Y_MAX+1;
    y_inc=-1;
    break;
  } // end switch(start)          

  //while(x>-9 && x<X_MAX+8 && y>-9 && y<Y_MAX+8) {
  while(x>-8 && x<X_MAX+8 && y>-8 && y<Y_MAX+8) {
    if(this_sprite) {
      put_sprite(x,y, sprite_00);
      while( PAUSE && b5.isPressed( )); // pause mode for photos
      delay(100);
      del_sprite(x,y, sprite_00);
      put_sprite(x,y, sprite_01);
      while( PAUSE && b5.isPressed( )); // pause mode for photos
      delay(100);
      del_sprite(x,y, sprite_01);
    }
    else {
      put_sprite(x,y, sprite_02);
      while( PAUSE && b5.isPressed( )); // pause mode for photos
      delay(100);
      del_sprite(x,y, sprite_02);
      put_sprite(x,y, sprite_03);
      while( PAUSE && b5.isPressed( )); // pause mode for photos
      delay(100);
      del_sprite(x,y, sprite_03);
    }

    //if(this_sprite)  del_sprite(x,y, sprite_01);
    //else             del_sprite(x,y, sprite_03);

    if(random(0,2))      x+=x_inc;
    else { 
      if (steps>32) x-=x_inc; 
    }
    if(random(0,2))      y+=y_inc;
    else { 
      if (steps>16) y-=y_inc; 
    }

    steps++;
    //ht1632_clear();
    
  }
} // end function invader()

/*
 * Copy a character glyph from the tetris_8x8 data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void put_tetris(byte x, byte y, char c)
{
  byte dots;

  for (char col=0; col< 8; col++) {
    dots = pgm_read_byte_near(&tetris_8x8[c][col]);
    for (char row=0; row < 8; row++) {
      if (dots & (128>>row))   	     // 8 rows.
        plot(x+col, y+row, 1);
    }
  }
}

void del_tetris(byte x, byte y, char c)
{
  byte dots;

  for (char col=0; col< 8; col++) {
    dots = pgm_read_byte_near(&tetris_8x8[c][col]);
    for (char row=0; row < 8; row++) {
      if (dots & (128>>row))   	     // 8 rows.
        plot(x+col, y+row, 0);
    }
  }
}

void tetris_fall(int del){
  
  int x=random(-2,X_MAX-6);
  int y=-8;
  
  int shape=random(0,8);
  
  for(int i=0; i<24; i++){
    put_tetris(x,y,shape);
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    delay(del);
    del_tetris(x,y,shape);
    y++;
  }
}

/*
 * Copy a character glyph from the pacman data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void put_pacman(byte x, byte y, char c)
{
  prog_uint16_t dots;

  for (char col=0; col< 12; col++) {
    dots = pgm_read_word_near(&pacman_12x12[c][col]);
    for (prog_uint16_t row=0; row < 11; row++) {
      if (dots & (1024>>row))   	     // 11 rows.
        plot(x+col, y+row, 1);
      //else 
        //plot(x+col, y+row, 0);
    }
  }
}

void del_pacman(byte x, byte y, char c)
{
  prog_uint16_t dots;

  for (char col=0; col< 12; col++) {
    dots = pgm_read_word_near(&pacman_12x12[c][col]);
    for (prog_uint16_t row=0; row < 11; row++) {
      if (dots & (1024>>row))   	     // 11 rows.
        plot(x+col, y+row, 0);
      //else 
        //plot(x+col, y+row, 0);
    }
  }
}

// pacman(del)
// pacman eats across the screen from right to left
// used in game_time()
void pacman(int del){
  
  int x=X_MAX+1;
  int y=2;
  
  for(int i=0; i<18; i++){
    put_pacman(x,y,0);
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    delay(del);
    del_pacman(x,y,0);
    put_pacman(x,y,1);
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    delay(del);
    del_pacman(x,y,1);
    x--;
    put_pacman(x,y,2);
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    delay(del);
    del_pacman(x,y,2);
    put_pacman(x,y,1);
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    delay(del);
    del_pacman(x,y,1);
    x--;
  }
}
 

/***********************************************************************
 * traditional Arduino sketch functions: 
 and loop.
 ***********************************************************************/

void setup() 
{
  // init the button inputs
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  digitalWrite(4, HIGH);       
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);

  // debug mode forced by B4 pressed during start up
  if(!digitalRead(7))  DEBUG=true;
  
  if(DEBUG){
    Serial.begin(57600);
    Serial.print("DOTKLOK "); Serial.println(REV);
    Serial.println("DEBUG true, serial port open at 57600");
    //Serial.print("Avail mem = ");
    //Serial.println(availableMemory());
  }
  else{
    Serial.begin(57600);
    Serial.print("DOTKLOK "); Serial.println(REV);
    Serial.println("DEBUG false, serial port now closed");
    Serial.end();
  }

  // display screen setup, software version display, (and test)
  ht1632_setup();
  ht1632_clear();
  putchar_3x5(7, 5, REV[0]);
  putchar_3x5(13, 5, REV[2]);
  plot(11, 9, 1);
  delay(1000);
  if(DEBUG)  screenTest();
  
  // general set up
  //pinMode(13, OUTPUT);
  randomSeed(analogRead(0));

  // RTC setup
  Wire.begin();
  RTC.begin();

  if (!RTC.isrunning()) {
    if(DEBUG) Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  else { 
    if(DEBUG) Serial.println("RTC is running."); 
  }
  
  // test buttons
  /*
  while( b5.isPressed() ){
    for(int i=0; i<4; i++){
      if( buttons[i].isPressed() )  plot(i,0,1);
      else                          plot(i,0,0);
    } 
  } */
  
} // end setup()  

void loop()
{    
  // all time and button checking handled in animation fucntions
  // main loop just handles animation number (init. to 1)
  
  // bound the animation number
  //animation = constrain(animation,1,ani_max);
  //if(DEBUG)  animation=6; // used to force any animation during testing/debugging
  if(DEBUG) { Serial.print("loop / animation: "); Serial.println(animation); }

  // run the clock animation
  switch(animation) {
  case 1:
    big_time();
    break;
  case 2:
    basic_date_time();
    break;
  case 3:
    game_time();
    break;
  case 4:
    pong_time();  
    break;
  case 5:
    float_time(); 
    break;
  case 6:
    seconds_time();
    break;
  case 7:
    percent_time();
    break;
  case 8:
    column_time();
    break;
  case 9:
    random_dot_time();
    break;
  case 10:
    analog_dot_time();
    break;
  case 11:
    binary_year_time();
    break;
  case 12:
    scroll_time();
    break;

  } // end switch(animation)

} // end main loop

