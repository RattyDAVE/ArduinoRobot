#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int currentColumn=0;
int currentRow=0;

int shape[8][8] = {
  {1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1}
};


void setup() {
  matrix.setIntensity(15); // Set brightness between 0 and 15

  // Adjust to your own needs
  //  matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
  //  matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
  //  matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
  //  matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
  //  ...
  //  matrix.setRotation(0, 2);    // The first display is position upside down
  //  matrix.setRotation(3, 2);    // The same hold for the last display

}


void loop() {
  draw();

  matrix.write(); // Send bitmap to display
}

void draw(){
    for(int currentRow = 0; currentRow < 8; currentRow++){
       for(int currentColumn = 0; currentColumn < 8; currentColumn++){
         if (shape[currentColumn][currentColumn] == 1) {
             matrix.drawPixel(currentColumn, currentColumn, HIGH);
         }
           else {
             matrix.drawPixel(currentColumn, currentColumn, LOW); 
           } 
        }
      }
}
