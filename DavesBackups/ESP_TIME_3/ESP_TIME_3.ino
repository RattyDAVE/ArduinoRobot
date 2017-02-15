///MAX72xx CODE
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
int pinCS = D4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
//END MAX 72xx Code

const prog_uint8_t sprite_01[8] =
{
  0x18,    // ___XX___ 
  0x3C,    // __XXXX__
  0x7E,    // _XXXXXX_
  0xDB,    // X_XXXX_X
  0xFF,    // XXXXXXXX
  0x5A,    // _X_XX_X_
  0x81,    // X______X
  0x42     // _X____X_
};

const prog_uint8_t sprite_02[8] =
{
  0b00100100,
  0b00100100,
  0x7E,    
  0xDB,    
  0xFF,    
  0b01111110,
  0x81,    
  0x42     
};

const prog_uint8_t sprite_03[8] =
{
  0b01000010,
  0b00100100,
  0x7E,    
  0xDB,    
  0xFF,    
  0b00111100,
  0x24,    
  0x18     
};



void setup()
{
  matrix.setIntensity(1); // Use a value between 0 and 15 for brightness
  matrix.setCursor(0, 0);
  matrix.fillScreen(LOW);
}

void loop() {
// void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
     matrix.fillScreen(LOW);
   matrix.drawBitmap(0, 0, sprite_01, 8, 8, 1);
   matrix.write();
   delay(500);
   
     matrix.fillScreen(LOW);
matrix.drawBitmap(0, 0, sprite_02, 8, 8, 1);
   matrix.write();
   delay(500);
   
matrix.fillScreen(LOW);
matrix.drawBitmap(0, 0, sprite_03, 8, 8, 1);
   matrix.write();
   delay(500);

        matrix.fillScreen(LOW);
matrix.drawBitmap(0, 0, sprite_02, 8, 8, 1);
   matrix.write();
   delay(500);

}
