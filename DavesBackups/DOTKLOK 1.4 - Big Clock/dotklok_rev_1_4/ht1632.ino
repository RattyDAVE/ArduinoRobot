 /*** HT1632 FUNCTIONS from demo16x24.c ***/

    /***********************************************************************
     * demo16x24.c - Arduino demo program for Holtek HT1632 LED driver chip,
     *            As implemented on the Sure Electronics DE-DP016 display board
     *            (16*24 dot matrix LED module.)
     * Nov, 2008 by Bill Westfield ("WestfW")
     *   Copyrighted and distributed under the terms of the Berkely license
     *   (copy freely, but include this notice of original author.)
     ***********************************************************************/
     
     // Modified by Andrew O'Malley //
     

#define SCREENS 1       // The number of LED screens used
#define SURE_NEW 1      // used to indicate type of Sure screen (0=older, chip on LED side; 1=new, chip on non-LED side)
#define RED 0
#define GREEN 1

byte screen_color=GREEN; // only need to set RED or GREEN if SURE_NEW set to 0,
  
/*
 * Set these constants to the values of the pins connected to the SureElectronics Module
 * NOTE THAT ht1632_writebits MUST BE UPDATED ACCORDINGLY FOR DIRECT PORT MANIPULATION
 */
 
#define REV_1 1
//#define BETA3 1

#ifdef BETA1
  static const byte ht1632_data = 11;    // Data pin
  static const byte ht1632_wrclk = 10;   // Write clock pin 
  static const byte ht1632_cs1 = 9;       // Chip Select 1 
  static const byte ht1632_cs2 = 8;       // Chip Select 2
  static const byte ht1632_cs3 = 7;      // Chip Select 3  (not actually connected)
  static const byte ht1632_cs4 = 6;       // Chip Select 4 (not actually connected)
  #define CLK_LOW  B11111011
  #define CLK_HI   B00000100
  #define DATA_LOW B11110111
  #define DATA_HI  B00001000
#endif

// define for DOTKLOK BETA 2 (green)
#ifdef BETA2
  static const byte ht1632_data = 9;    // Data pin
  static const byte ht1632_wrclk = 10;   // Write clock pin 
  static const byte ht1632_cs1 = 11;       // Chip Select 1 
  static const byte ht1632_cs2 = 8;       // Chip Select 2
  static const byte ht1632_cs3 = 7;      // Chip Select 3  (not actually connected)
  static const byte ht1632_cs4 = 6;       // Chip Select 4 (not actually connected)
  #define CLK_LOW  B11111011
  #define CLK_HI   B00000100
  #define DATA_LOW B11111101
  #define DATA_HI  B00000010
#endif

// define for DOTKLOK BETA 3 (red)
#ifdef BETA3
  static const byte ht1632_data = 10;    // Data pin
  static const byte ht1632_wrclk = 11;   // Write clock pin 
  static const byte ht1632_cs1 = 9;       // Chip Select 1 
  static const byte ht1632_cs2 = 8;       // Chip Select 2
  static const byte ht1632_cs3 = 7;      // Chip Select 3  (not actually connected)
  static const byte ht1632_cs4 = 6;       // Chip Select 4 (not actually connected)
  #define CLK_LOW  B11110111
  #define CLK_HI   B00001000
  #define DATA_LOW B11111011
  #define DATA_HI  B00000100
#endif

// define for PCB directly connected to 2416 screen
#ifdef CIRCUIT_PCB
  static const byte ht1632_data = 12;    // Data pin
  static const byte ht1632_wrclk = 11;   // Write clock pin 
  static const byte ht1632_cs1 = 9;       // Chip Select 1 
  static const byte ht1632_cs2 = 8;       // Chip Select 2
  static const byte ht1632_cs3 = 10;      // Chip Select 3 
  static const byte ht1632_cs4 = 7;       // Chip Select 4 
  #define CLK_LOW  B11110111
  #define CLK_HI   B00001000
  #define DATA_LOW B11101111
  #define DATA_HI  B00010000
#endif

// define for REV 1.0 PCB
#ifdef REV_1
  static const byte ht1632_data = 9;    // Data pin
  static const byte ht1632_wrclk = 10;   // Write clock pin 
  static const byte ht1632_cs1 = 11;       // Chip Select 1 
  static const byte ht1632_cs2 = 0;       // Chip Select 2 (not connected on PCB)
  static const byte ht1632_cs3 = 0;      // Chip Select 3  (not connected on PCB)
  static const byte ht1632_cs4 = 0;       // Chip Select 4 (not connected on PCB)
  #define CLK_LOW  B11111011
  #define CLK_HI   B00000100
  #define DATA_LOW B11111101
  #define DATA_HI  B00000010
#endif

// define for PCB with RIBBON connecting to 2416 screen
#ifdef CIRCUIT_PCB_RIBBON
  static const byte ht1632_data = 9;    // Data pin
  static const byte ht1632_wrclk = 10;   // Write clock pin 
  static const byte ht1632_cs1 = 12;       // Chip Select 1 
  static const byte ht1632_cs2 = 7;       // Chip Select 2
  static const byte ht1632_cs3 = 11;      // Chip Select 3 
  static const byte ht1632_cs4 = 8;       // Chip Select 4
  #define CLK_LOW  B11111011
  #define CLK_HI   B00000100
  #define DATA_LOW B11111101
  #define DATA_HI  B00000010
#endif

// pwm brightness setting commands
// note: there are 16 brightness settings (0-15)
// these numbers are added to the base brightness command
// they were chosen experimentally as a good range of 5 values w/ good steps between them
unsigned int bright_commands[5] = { 0, 2, 5, 10, 15};

// index for the 5 brightness "settings" (0-4) adjusted in seconds_time()
int brightness=4;

// array of chip selects for cascaded displays
byte chip[4] = { ht1632_cs1, ht1632_cs2, ht1632_cs3, ht1632_cs4 }; 

// There should also be a common GND.
// The module with all LEDs like draws about 200mA,
// which makes it PROBABLY powerable via Arduino +5V

/***********************************************************************
 * ht1632_chipselect / ht1632_chipfree
 * Select or de-select a particular ht1632 chip.
 * De-selecting a chip ends the commands being sent to a chip.
 * CD pins are active-low; writing 0 to the pin selects the chip.
 ***********************************************************************/

void ht1632_chipselect(byte chipno)
{
  DEBUGPRINT("\nHT1632(%d) ", chipno);
  digitalWrite(chipno, 0);
}

void ht1632_chipfree(byte chipno)
{
  DEBUGPRINT(" [done %d]", chipno);
  digitalWrite(chipno, 1);
}

/*
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk
 * Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent
 * being (bits & firstbit), shifted till firsbit is zero.
 * MODIFIED FOR DIRECT PORT ACCESS, see above for #defines of PORTB pins
 */
void ht1632_writebits (byte bits, byte firstbit)
{
  DEBUGPRINT(" ");
  while (firstbit) {
    DEBUGPRINT((bits&firstbit ? "1" : "0"));
    //digitalWrite(ht1632_wrclk, LOW);
    //PORTB = PORTB & B11110111; 
    PORTB = PORTB & CLK_LOW; 
    if (bits & firstbit) {
      //digitalWrite(ht1632_data, HIGH);
      //PORTB = PORTB | B00010000;
      PORTB = PORTB | DATA_HI;
    } 
    else {
      //digitalWrite(ht1632_data, LOW);
      //PORTB = PORTB & B11101111;
      PORTB = PORTB & DATA_LOW;
    }
    //digitalWrite(ht1632_wrclk, HIGH);
    //PORTB = PORTB | B00001000;
    PORTB = PORTB | CLK_HI;
    firstbit >>= 1;
  }
}

/*
 * ht1632_sendcmd
 * Send a command to the ht1632 chip.
 * A command consists of a 3-bit "CMD" ID, an 8bit command, and
 * one "don't care bit".
 *   Select 1 0 0 c7 c6 c5 c4 c3 c2 c1 c0 xx Free
 */
static void ht1632_sendcmd (byte chipno, byte command)
{
  ht1632_chipselect(chipno);                // Select chip
  ht1632_writebits(HT1632_ID_CMD, 1<<2);   // send 3 bits of id: COMMMAND
  ht1632_writebits(command, 1<<7);         // send the actual command
  ht1632_writebits(0, 1); 	           /* one extra dont-care bit in commands. */
  ht1632_chipfree(chipno);              //done
}

/*
 * ht1632_senddata
 * send a nibble (4 bits) of data to a particular memory location of the
 * ht1632.  The command has 3 bit ID, 7 bits of address, and 4 bits of data.
 *    Select 1 0 1 A6 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3 Free
 * Note that the address is sent MSB first, while the data is sent LSB first!
 * This means that somewhere a bit reversal will have to be done to get
 * zero-based addressing of words and dots within words.
 */
static void ht1632_senddata (byte chipno, byte address, byte data)
{
  ht1632_chipselect(chipno);          // Select chip
  ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
  ht1632_writebits(address, 1<<6);       // Send address
  ht1632_writebits(data, 1<<3);          // send 4 bits of data
  ht1632_chipfree(chipno);            // done
}

void ht1632_setup()
{
  for(int c=0; c<SCREENS; c++) {
    
    pinMode(chip[c], OUTPUT);
    digitalWrite(chip[c], HIGH); 	/* unselect (active low) */
    pinMode(ht1632_wrclk, OUTPUT);
    pinMode(ht1632_data, OUTPUT);
    
    if(SURE_NEW){
      ht1632_sendcmd(chip[c], HT1632_CMD_SYSDIS);    // Disable system
      ht1632_sendcmd(chip[c], HT1632_CMD_COMS01);    // 16*32, NMOS drivers
      ht1632_sendcmd(chip[c], HT1632_CMD_RCCLK);     // HT1632 master mode &18
      ht1632_sendcmd(chip[c], HT1632_CMD_SYSON);     /* System on */
      ht1632_sendcmd(chip[c], HT1632_CMD_LEDON);     /* LEDs on */
      ht1632_sendcmd(chip[c], HT1632_CMD_PWM+15);    // LEDs at full brightness
    }
    else{  
      ht1632_sendcmd(chip[c], HT1632_CMD_SYSDIS);    // Disable system
      ht1632_sendcmd(chip[c], HT1632_CMD_COMS11);    // 16*32, PMOS drivers
      ht1632_sendcmd(chip[c], HT1632_CMD_MSTMD);     /* Master Mode HT1632 */
      ht1632_sendcmd(chip[c], HT1632_CMD_SYSON);     /* System on */
      ht1632_sendcmd(chip[c], HT1632_CMD_LEDON);     /* LEDs on */
      ht1632_sendcmd(chip[c], HT1632_CMD_PWM+15);    // LEDs at full brightness      
    }
    
    for (byte i=0; i<96; i++)
      ht1632_senddata(chip[c], i, 0);  // clear the display!
    //delay(LONGDELAY);
  }

  if(DEBUG && !SURE_NEW){
    if(screen_color==RED)    Serial.println("RED");
    if(screen_color==GREEN)  Serial.println("GREEN");
  }
  
}

/*
 * we keep a copy of the display controller contents so that we can
 * know which bits are on without having to (slowly) read the device.
 * Note that we only use the low four bits of the shadow ram, since
 * we're shadowing 4-bit memory.  This makes things faster, and we
 * use the other half for a "snapshot" when we want to plot new data
 * based on older data...
 */
byte ht1632_shadowram[SCREENS][96];  // our copy of the display's RAM

/*
 * plot a point on the display, with the upper left hand corner
 * being (0,0), and the lower right hand corner being (23, 15).
 * Note that Y increases going "downward" in contrast with most
 * mathematical coordiate systems, but in common with many displays
 * No error checking; bad things may happen if arguments are out of
 * bounds!  (The ASSERTS compile to nothing by default)
 * EDIT: error checking added for x and y out of bounds
 */
//void ht1632_plot (char x, char y, char val)
void plot (char x, char y, char val)
{
  char addr, bitval;
  byte chip_this;
  int c;

  ASSERT(x >= 0);
  ASSERT(x <= X_MAX);
  ASSERT(y >= 0);
  ASSERT(y <= Y_MAX);

  // exit if x or y is out of bounds
  if ((x < 0) || (y < 0) || (x > X_MAX) || (y > Y_MAX)) {
    return;
  }

  // adjust for the proper chip "c" based on x 
  if(x<24) {
    c=0;
  }
  else if(x<48){
    c=1;
    x -= 24;
  }
  else if(x<72){
    c=2;
    x -= 48;
  }
  else{
    c=3;
    x -= 72;
  }

  // reverse screen for upside down, might not be compatible with 2+ screens
  //x = X_MAX-x;
  //y = Y_MAX-y;

  /*
   * The 4 bits in a single memory word go DOWN, with the LSB
   * (first transmitted) bit being on top.  However, writebits()
   * sends the MSB first, so we have to do a sort of bit-reversal
   * somewhere.  Here, this is done by shifting the single bit in
   * the opposite direction from what you might expect.
   */
   
  // compute which bit will need set
  bitval = 8>>(y&3);  // compute which bit will need set
  
  // compute which memory word this is in
  if(SURE_NEW){
    addr = (x<<2) + (y>>2);
  }
  else{
    if(screen_color==RED)  { addr = (x<<2) + (y>>2); }
    else                   { addr = ((x%24)<<2) + (y>>2)^ 28; }
  }
  
  if (val) {  // Modify the shadow memory
    ht1632_shadowram[c][addr] |= bitval;
  } 
  else {
    ht1632_shadowram[c][addr] &= ~bitval;
  }
  // Now copy the new memory value to the display
  //if (x<24) chip_this=chip[1];
  //else chip_this=chip[0];
  ht1632_senddata(chip[c], addr, ht1632_shadowram[c][addr]);
} 

/*
 * ht1632_clear
 * clear the display, and the shadow memory, and the snapshot
 * memory.  This uses the "write multiple words" capability of
 * the chipset by writing all 96 words of memory without raising
 * the chipselect signal.
 */
void ht1632_clear()
{
  char i;

  for(int c=0; c<SCREENS; c++) {
    ht1632_chipselect(chip[c]);  // Select chip
    ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
    ht1632_writebits(0, 1<<6); // Send address
    for (i = 0; i < 96/2; i++) // Clear entire display
      ht1632_writebits(0, 1<<7); // send 8 bits of data
    ht1632_chipfree(chip[c]); // done
    for (i=0; i < 96; i++)
      ht1632_shadowram[c][i] = 0;
  }
}

/*
 * Copy a character glyph from the font_3x5 data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void putchar_3x5(byte x, byte y, char c)
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
  for (char col=0; col< 3; col++) {
    dots = pgm_read_byte_near(&font_3x5[c][col]);
    for (char row=0; row < 5; row++) {
      if (dots & (16>>row))   	     // only 5 rows.
        plot(x+col, y+row, 1);
      else 
        plot(x+col, y+row, 0);
    }
  }
}

/*
 * Copy a character glyph from the font_4x7 data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void putchar_4x7(byte x, byte y, char c)
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
        plot(x+col, y+row, 1);
      else 
        plot(x+col, y+row, 0);
    }
  }
}

/*
 * Copy a character glyph from the font_5x12 data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void putchar_5x12(byte x, byte y, char c)
{
  prog_uint16_t dots;

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
  for (char col=0; col< 5; col++) {
    dots = pgm_read_word_near(&font_5x12[c][col]);
    for (char row=0; row < 12; row++) {
      if (dots & (2048>>row))    // 12 rows.
        plot(x+col, y+row, 1);
      else 
        plot(x+col, y+row, 0);
    }
  }
}

/*
 * Copy a character glyph from the font data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
 /*
 DOESN'T COMPILE YET
void putchar(char &font, byte rows, byte cols, byte x, byte y, char c)
{
  byte dots;
  int shift=pow(2, rows-1);
  
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
  for (char col=0; col<cols; col++) {
    dots = pgm_read_byte_near(&font[c][col]);
    for (char row=0; row<rows; row++) {
      if (dots & (shift>>row))
        plot(x+col, y+row, 1);
      else 
        plot(x+col, y+row, 0);
    }
  }
}

*/

