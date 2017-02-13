// just reading the encoder, nothing else.

int encoderValue_A = 0;
int encoderValue_B = 0; 

void setup() {

  Serial.begin(9600);

  pinMode(2, INPUT);
  pinMode(3, INPUT);

  attachInterrupt(digitalPinToInterrupt(2), countA, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), countB, FALLING);
}

void loop() {

  // if there is data to read, read it
  if (Serial.available() > 0) {

    // read the incoming data from the ezb
    int incomingByte = Serial.read();

    // command 'a' means transmit the current values and reset the counters
    if (incomingByte == 'a') {

      // send the values
      Serial.write(encoderValue_A);
      Serial.write(encoderValue_B);

      // reset the values
      encoderValue_A = 0;
      encoderValue_B = 0;
    }
  }
}

void countA() {
  encoderValue_A++;  
}

void countB() {
  encoderValue_B++;  
}
