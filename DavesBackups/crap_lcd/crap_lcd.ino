#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

const byte NUMBER_OF_NAMES = 15;
char* names[NUMBER_OF_NAMES] = { "You just made my day.",
"You’re an excellent cook, artist, athlete or teacher.",
"You’re a hoot.", 
"I like the way you are.",
"Your kids are so nice, thoughtful or well-behaved.",
"You have a great sense of style.",
"Thanks for being there for me.",
"Your smile brightens the room.",
"Have you lost weight?",
"Your home feels so inviting.",
"You deserve a promotion.",
"You make me want to be a better person.",
"You look so young.",
"I’m so jealous of your garden, shoes or keyboarding skills.",
"I like your aura."};

char text;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

randomSeed(analogRead(0));

}

void loop() {
  lcd.clear();
  //text = string(names[random(0,NUMBER_OF_NAMES)]);
lcd.autoscroll();

  lcd.print(names[random(0,NUMBER_OF_NAMES)]);
  delay(10000);
}

