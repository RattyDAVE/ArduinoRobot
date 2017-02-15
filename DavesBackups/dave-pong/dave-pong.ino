#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#include <IRremote.h>
int ir_PIN = 2;
IRrecv irrecv(ir_PIN);
decode_results results;


unsigned long lastRefreshTime = 0;
int refreshInterval = 1;
unsigned long lastMoveTime = 0;
int moveInterval;
int speakerPin = 4;
unsigned long now = 0;
unsigned long overTime = 100000000;
int currentColumn = 0;
int ballX;
int ballY;
int player1Score, player2Score;
int player1Position, player2Position;
boolean ballMovingUp = true;
boolean ballMovingLeft = true;
boolean isGameOn = true;
int shape[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);


void setup() {
  Serial.begin(9600);
       Serial.println("Im Awake");
   irrecv.enableIRIn(); // Start the receiver
  player1Score = 0;
  player2Score = 0;
  restartGame();
}

void loop() {
  now = millis();
  if (isGameOn) {
    update();
  } else {
    updateScore();
  }
  draw();
}

void gameOver() {
  isGameOn = false;
  overTime = now;
  player1Score %= 7;
  player2Score %= 7;
  int note[] = {700, 600, 500, 400, 300, 200};
  for (int i = 0; i < 6; i++) {
    tone(speakerPin, note[i], 150);
    delay(200);
  }

}

void restartGame() {
  moveInterval = 700;
  ballX = now % 8;
  ballY = 6;
  ballMovingUp = true;
  ballMovingLeft = true;
  isGameOn = true;
}

void updateBall() {
  if (ballMovingLeft)
    ballX--;
  else
    ballX++;
  if (ballX == 0)
    ballMovingLeft = false;
  else if (ballX == 7)
    ballMovingLeft = true;

  if (ballMovingUp)
    ballY--;
  else
    ballY++;
  if (ballY == 0) {
    player1Score++;
    gameOver();
  } else if (ballY == 7) {
    player2Score++;
    gameOver();
  }
  if (ballY == 1 && ballX >= player2Position && ballX < player2Position + 3) {
    ballMovingUp = false;
    moveInterval -= 20;
    buzz();
  }
  else if (ballY == 6 && ballX >= player1Position && ballX < player1Position + 3) {
    ballMovingUp = true;
    moveInterval -= 20;
    buzz();
  }

}

void buzz() {
  tone(speakerPin, 300, 20);
}

void update() {
  //clear table;
  getRemote();
  if (now - lastMoveTime > moveInterval) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        shape[i][j] = 0;
      }
    }

    //update player positions
    //int player1PotansValue = analogRead(A0);
    //int player2PotansValue = analogRead(A1);
    //Serial.println(player2PotansValue);
    //player1Position = player1PotansValue * 6 / 1024;
    //player2Position = player2PotansValue * 6 / 1024;

//    getRemote();



    for (int i = 0; i < 8; i++) {
      if (i >= player1Position && i < player1Position + 3) {
        shape[7][i] = 1;
      } else {
        shape[7][i] = 0;
      }
      if (i >= player2Position && i < player2Position + 3) {
        shape[0][i] = 1;
      } else {
        shape[0][i] = 0;
      }
    }
    //update the ball
    updateBall();
    //draw the ball
    shape[ballY][ballX] = 1;
    lastMoveTime = now;
  }
}


void updateScore() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      shape[i][j] = 0;
    }
  }

  for (int i = 0; i < player2Score; i++) {
    for (int j = 0; j < i + 1; j++) {
      shape[j][6 - i] = 1;
    }
  }

  for (int i = 0; i < player1Score; i++) {
    for (int j = 0; j < i + 1; j++) {
      shape[7 - j][i + 1] = 1;
    }
  }
  if (now - overTime > 3000)
    restartGame();
}


void getRemote() {
  if (irrecv.decode(&results)) {

    switch (results.value) {
      case 4137711015:
      case 2148500481:
      case 1427585931:
      case 2148467713:
        Serial.println("1 Pressed!!!!!!");
        player2Position--;
        
        break;

      case 2148467715:
      case 3466459158:
      case 2148500483:
      case 756334074:
      case 3992928380:
        Serial.println("3 Pressed!!!!!!");
        player2Position++;
        break;

      case 2148467719:
      case 2441719334:
      case 3989900430:
      case 2148500487:
        Serial.println("7 Pressed!!!!!!");
        player1Position--;
        break;


      case 1593295712:
      case 2148467721:
      case 2148500489:
      case 3597221157:
      case 1937225124:
        Serial.println("9 Pressed!!!!!!");
        player1Position++;
        break;

      default :
        Serial.println(results.value, HEX);
        Serial.print("results.value = ");
        Serial.println(results.value);


    }
    if (player1Position < 0) player1Position = 1;
    if (player1Position > 5) player1Position = 5;
    if (player2Position < 0) player2Position = 1;
    if (player2Position > 5) player2Position = 5;
    
    irrecv.resume(); // Receive the next value
  }
}



void draw() {
  for (int currentRow = 0; currentRow < 8; currentRow++) {
    for (int currentColumn = 0; currentColumn < 8; currentColumn++) {
      if (shape[currentRow][currentColumn] == 1) {
        matrix.drawPixel(currentColumn, currentRow, HIGH);
      }
      else {
        matrix.drawPixel(currentColumn, currentRow, LOW);
      }
    }
  }
  matrix.write();
}
