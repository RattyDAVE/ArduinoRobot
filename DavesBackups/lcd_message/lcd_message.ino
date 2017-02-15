#include <LiquidCrystal.h>
#include <Wire.h>
#define   CONTRAST_PIN   10
#define   BACKLIGHT_PIN  11
#define   CONTRAST       130
#define   LIGHT_PIN      A0

// LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2, BACKLIGHT_PIN, POSITIVE );

#define LCD_RS 9
#define LCD_RW 8
#define LCD_E 7
#define LCD_DATA4 5
#define LCD_DATA3 4
#define LCD_DATA2 3
#define LCD_DATA1 2

LiquidCrystal lcd(LCD_RS, LCD_RW, LCD_E, LCD_DATA4, LCD_DATA3, LCD_DATA2, LCD_DATA1, BACKLIGHT_PIN, POSITIVE);

const byte NUMBER_OF_NAMES = 69;
char* names[NUMBER_OF_NAMES][4] PROGMEM = {
  {"amma ki chut", "mothers pussy", "Hindi", ""},
  {"Baap ke lavde", "Penis of father", "Hindi", ""},
  {"Bakrichod", "Goat Fucker", "Hindi", ""},
  {"Balatkaar", "Rape", "Hindi", ""},
  {"Bell Bula", "bull's dick", "Hindi", ""},
  {"Betichod", "daughter fucker", "Hindi", ""},
  {"Bhen ke lode", "Cock of sister", "Hindi", ""},
  {"bhosad chod", "dirty cunt fucker", "Hindi", ""},
  {"bhosadi ke", "man with a pussy", "Hindi", ""},
  {"bhosdee kay", "son of a whore", "Hindi", ""},
  {"bhosdi k", "son of a randi ( bitch )", "Hindi", ""},
  {"bhundi", "top of nipple", "Hindi", ""},
  {"Bihari", "Loser", "Hindi", ""},
  {"bolti bandh kar", "shut your mouth", "Hindi", ""},
  {"bulle ke baal", "hair around penis", "Hindi", ""},
  {"bursungha", "one who smell pussy", "Hindi", ""},
  {"camina", "bastard", "Hindi", ""},
  {"Cha Cha Chod", "Uncle Fucker", "Hindi", ""},
  {"chod", "fuck", "Hindi", ""},
  {"chod bhangra", "fuck disco", "Hindi", ""},
  {"choot k bhoot", "ghost of cunt", "Hindi", ""},
  {"choot k pakode", "fryball of cunt", "Hindi", ""},
  {"choot ka paani", "pussy water", "Hindi", ""},
  {"choot ka pissu", "pussy tick", "Hindi", ""},
  {"choot ki jhilli", "clit of cunt", "Hindi", ""},
  {"chootia", "dumbass", "Hindi", ""},
  {"Chopre he randi", "Shut up whore!", "Hindi", ""},
  {"chudpagal", "pussy madman", "Hindi", ""},
  {"Chut ka maindak", "frog in a vagina", "Hindi", ""},
  {"chut ke makkhan", "butter of cunt", "Hindi", ""},
  {"chutiya", "wanker", "Hindi", ""},
  {"Chutiya chootia", "Fucker", "Hindi", ""},
  {"Delhi", "Pussy of father", "Hindi", ""},
  {"Dheeli Choot", "Loose Cunt", "Hindi", ""},
  {"Gaand chaat mera", "lick my ass/ kiss my ass", "Hindi", ""},
  {"Gaand gaand", "Ass", "Hindi", ""},
  {"Gaand ka Khadda", "Ass hole", "Hindi", ""},
  {"Gaand ke Dhakan", "Butt Plug", "Hindi", ""},
  {"Gaand mein Kida", "Ants in your Ass", "Hindi", ""},
  {"Gaandu", "Asshole", "Hindi", ""},
  {"gaandu", "asshole", "Hindi", ""},
  {"gand mein louda", "cock in ur ass", "Hindi", ""},
  {"Gandmasti", "Foolhardy Fun", "Hindi", ""},
  {"goli", "balls", "Hindi", ""},
  {"haram zaadaa", "bastard", "Hindi", ""},
  {"harami", "bastard", "Hindi", ""},
  {"jhaat", "pubic hair", "Hindi", ""},
  {"Jhatoo", "pubes", "Hindi", ""},
  {"kaala lund", "black cock", "Hindi", ""},
  {"kaali kutti", "black bitch", "Hindi", ""},
  {"Kukarchod", "Dog fucker", "Hindi", ""},
  {"Kutha Sala", "Damn Bastard", "Hindi", ""},
  {"kuthri", "bitch", "Hindi", ""},
  {"kutte", "dog", "Hindi", ""},
  {"kutte ki olad", "son of a bitch", "Hindi", ""},
  {"lavde ka baal", "pubic hair", "Hindi", ""},
  {"lowde ka bal", "penis hair", "Hindi", ""},
  {"lund", "cock", "Hindi", ""},
  {"Lund chuse", "cock sucker", "Hindi", ""},
  {"Lund Khajoor", "Dickhead", "Hindi", ""},
  {"Lund luhnd", "Cock", "Hindi", ""},
  {"Madar Chod", "Mother Fucker!!", "Hindi", ""},
  {"Mader chod", "Mother Fucker", "Hindi", ""},
  {"Maggi", "Prostitute", "Hindi", ""},
  {"Mai Chod", "Mother Fucker", "Hindi", ""},
  {"Mera mume le", "Suck my dick", "Hindi", ""},
  {"Randi", "Whore", "Hindi", ""},
  {"Randi Ka Choda", "Son of a whore", "Hindi", ""},
  {"Tharki", "Bitchy", "Hindi", ""}
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
  analogWrite ( BACKLIGHT_PIN, 255 );

}

void loop() {
  //showmessage();

  //  lcd.setCursor(0, 1);
  //  lcd.print(lightreading);

  //analogWrite ( BACKLIGHT_PIN, 255 );

  newlightreading = analogRead ( LIGHT_PIN );

  if ( (lightreading - 5)  > newlightreading) {
    //if (1 == 1) {
    analogWrite ( BACKLIGHT_PIN, 255 );

    //    lcd.clear();
    //    lcd.setCursor(0, 0);
    //    lcd.print("      FUCK");
    //    lcd.setCursor(0, 1);
    //    lcd.print("      OFF");
    //    lcd.setCursor(0, 1);

    showmessage();
  }
  lightreading = newlightreading;
  delay(5000);
  analogWrite ( BACKLIGHT_PIN, 0 );
  lcd.clear();

  //delay(1000);

  //analogWrite ( BACKLIGHT_PIN, 0 );

  //delay(10000);
}

void showmessage() {
  char* line1;
  char* line2;
  //char* line3;
  //char* line4;

  int displaynumber;

  displaynumber = random(0, NUMBER_OF_NAMES);

  //for (int displaynumber = 0; displaynumber < NUMBER_OF_NAMES; displaynumber++) {


  line1 = (char*)pgm_read_word(&(names[displaynumber][0]));
  line2 = (char*)pgm_read_word(&(names[displaynumber][1]));
  //line3 = (char*)pgm_read_word(&(names[displaynumber][2]));
  //line4 = (char*)pgm_read_word(&(names[displaynumber][3]));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);

  //delay(1000);

  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print(line3);
  //lcd.setCursor(0, 1);
  //lcd.print(line4);

  //delay(1000);
  //}
}
