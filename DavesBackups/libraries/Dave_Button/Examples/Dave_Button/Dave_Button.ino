#include <Dave_Button.h>


// To Test press the FLASH button on NodeMCU

Button button = Button(D3,PULLUP);

void setup(){
  pinMode(BUILTIN_LED,OUTPUT);
}

void loop(){
  if(button.isPressed())digitalWrite(BUILTIN_LED,HIGH);
  else digitalWrite(BUILTIN_LED,LOW);
}
