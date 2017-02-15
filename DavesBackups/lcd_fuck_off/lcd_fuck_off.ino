#include <LiquidCrystal.h>
#include <Wire.h>
#define   CONTRAST_PIN   9
#define   BACKLIGHT_PIN  6
#define   CONTRAST       130
#define   LIGHT_PIN      A0

LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2, BACKLIGHT_PIN, POSITIVE );

const byte NUMBER_OF_NAMES = 18;
char* names[NUMBER_OF_NAMES] = { "You just made myday.",
                                 "Youre an        excellent cook.",
                                 "Youre a hoot.",
                                 "I like the way  you are.",
                                 "Your kids are sonice thoughtful.",
                                 "You have a greatsense of style.",
                                 "Thanks for beingthere for me.",
                                 "Have you lost   weight?",
                                 "Your home feels so inviting.",
                                 "You deserve a   promotion.",
                                 "You look so     young.",
                                 "Im so jealous ofyour garden.",
                                 "I like your aura",
                                 "Kabo lelou             Wolf dick",
                                 "Kalala                   Lesbian",
                                 "Lasarnat               Wet dream",
                                 "Liki ou manman      Our mums ass",
                                 "Trou fes sale         Salted ass"
                               };

int lightreading = 0;
int newlightreading = 0;

char text;

void setup() {
  // set up the LCD's number of columns and rows:
  pinMode(CONTRAST_PIN, OUTPUT);
  analogWrite ( CONTRAST_PIN, CONTRAST );
  lcd.begin(16, 2);
  randomSeed(analogRead(0));
    analogWrite ( BACKLIGHT_PIN, 0 );
}

void loop() {
  //showmessage();

  //lcd.setCursor(0, 1);
  //lcd.print(lightreading);

  //analogWrite ( BACKLIGHT_PIN, 255 );

  newlightreading = analogRead ( LIGHT_PIN );

  if ( (lightreading - 5)  > newlightreading) {
    analogWrite ( BACKLIGHT_PIN, 255 );

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("      FUCK");
    lcd.setCursor(0, 1);
    lcd.print("      OFF");
    lcd.setCursor(0, 1);


    delay(2000);
    analogWrite ( BACKLIGHT_PIN, 0 );
  }

  lightreading = newlightreading;

  delay(1000);

  //analogWrite ( BACKLIGHT_PIN, 0 );

  //delay(10000);
}

void showmessage() {
  String fullstring;
  String line1;
  String line2;

  //text = string(names[random(0,NUMBER_OF_NAMES)]);
  //lcd.autoscroll();
  //lcd.println(names[random(0,NUMBER_OF_NAMES)]);

  fullstring = names[random(0, sizeof(names) / 2)];

  //line1 = fullstring.substring(0, 16);
  //line2 = fullstring.substring(16, 32);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  lcd.setCursor(0, 1);

  lcd.print(analogRead(LIGHT_PIN));

  analogWrite ( BACKLIGHT_PIN, 255 );

  delay(1);

  //analogWrite ( BACKLIGHT_PIN, 0 );

  //delay(10000);
}
