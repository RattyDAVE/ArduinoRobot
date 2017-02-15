// Includes for Webserver
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
const char *ssid = "FishNet";
const char *password = "starwarsivanewhope";
ESP8266WebServer server ( 80 );

// Includes for Matrix
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
int pinCS = D4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);


String tape = "";
//char *tape = "Arduino";
int wait = 40; // In milliseconds
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels
//------//

  //used for spin()
  int spin_wait = 50;
  int spin_inc = -2;

#include <Dave_Button.h>
//Button buttonD1 = Button(D1,PULLUP);
//Button buttonD2 = Button(D2,PULLUP);
Button buttonD3 = Button(D3,PULLUP);
Button buttonD6 = Button(D6,PULLUP);
//Button buttonD8 = Button(D8,PULLUP);
//Button buttonA0 = Button(A0,PULLUP);
Button buttonGPIO9 = Button(9,PULLUP);
Button buttonGPIO10 = Button(10,PULLUP);

int dostuff = 0;

void handleRoot() {
	char temp[800];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 800,

"<html>\
  <head>\
    <title>Alanna</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
<form action=\"\" method=\"get\">\
  Message: <input type=\"text\" name=\"message\"><br>\
  <input type=\"submit\" value=\"Submit\">\
</form>\
<form action=\"\" method=\"get\">\
<input type=\"radio\" name=\"spin\" value=\"on\">Spin on<br>\
<input type=\"radio\" name=\"spin\" value=\"off\" checked>Spin Off<br>\
<input type=\"submit\" value=\"Submit\">\
</form>\
  </body>\
</html>",

		hr, min % 60, sec % 60
	);
	server.send ( 200, "text/html", temp );
}

void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
		message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
}

void setup ( void ) {
	Serial.begin ( 115200 );
	WiFi.begin ( ssid, password );
	Serial.println ( "" );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}

	Serial.println ( "" );
	Serial.print ( "Connected to " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	if ( MDNS.begin ( "esp8266" ) ) {
		Serial.println ( "MDNS responder started" );
	}

	server.on ( "/", handleRoot );
	server.on ( "/inline", []() {
		server.send ( 200, "text/plain", "this works as well" );
	} );
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );

  matrix.setIntensity(7); // Use a value between 0 and 15 for brightness
  pinMode(BUILTIN_LED,OUTPUT);

}

void loop ( void ) {

//  if(buttonD1.isPressed()){ tape = "ButtonD1"; digitalWrite(BUILTIN_LED,HIGH);}
//  if(buttonD2.isPressed()){ tape = "ButtonD2"; digitalWrite(BUILTIN_LED,HIGH);}
  if(buttonD3.isPressed()){ tape = "ButtonD3"; digitalWrite(BUILTIN_LED,HIGH);}
  else if(buttonD6.isPressed()){ tape = "ButtonD6"; digitalWrite(BUILTIN_LED,HIGH);}
//  else if(buttonD8.isPressed()){ tape = "ButtonD8"; digitalWrite(BUILTIN_LED,HIGH);}
//  else if(buttonA0.isPressed()){ tape = "ButtonA0"; digitalWrite(BUILTIN_LED,HIGH);}
  else if(buttonGPIO9.isPressed()){ tape = "ButtonGPIO9"; digitalWrite(BUILTIN_LED,HIGH);}
  else if(buttonGPIO10.isPressed()){ tape = "ButtonGPIO10"; digitalWrite(BUILTIN_LED,HIGH);}
  else digitalWrite(BUILTIN_LED,LOW);
    
	server.handleClient();

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if( server.argName ( i ) == "spin" && server.arg ( i ) == "off") dostuff = 0;
    if( server.argName ( i ) == "spin" && server.arg ( i ) == "on") dostuff = 2;
    
    if( server.argName ( i ) == "message" ) {dostuff = 1;tape = urldecode(server.arg ( i ));} 
  }

  if (dostuff == 1) displaymessage();
  if (dostuff == 2) spin();
  else matrix.shutdown(1);
  
}

void displaymessage() {
  matrix.shutdown(0);
  for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display

    delay(wait);
  }
}

void spin() {
  matrix.shutdown(0);

  for ( int x = 0; x < matrix.width() - 1; x++ ) {
    matrix.fillScreen(LOW);
    matrix.drawLine(x, 0, matrix.width() - 1 - x, matrix.height() - 1, HIGH);
    matrix.write(); // Send bitmap to display
    delay(spin_wait);
  }

  for ( int y = 0; y < matrix.height() - 1; y++ ) {
    matrix.fillScreen(LOW);
    matrix.drawLine(matrix.width() - 1, y, 0, matrix.height() - 1 - y, HIGH);
    matrix.write(); // Send bitmap to display
    delay(spin_wait);
  }

  spin_wait = spin_wait + spin_inc;
  if ( spin_wait == 0 ) spin_inc = 2;
  if ( spin_wait == 50 ) spin_inc = -2;
}

String urldecode(String input) {
  char c;
  String ret = "";
 
  for(byte t=0;t<input.length();t++) {
    c = input[t];
    if (c == '+') c = ' ';
    if (c == '%') {
      t++;
      c = input[t];
      t++;
      c = (h2int(c) << 4) | h2int(input[t]);
    }
  
    ret.concat(c);
  }
  return ret;
}

unsigned char h2int(char c) {
  if (c >= '0' && c <='9'){
    return((unsigned char)c - '0');
  }
  if (c >= 'a' && c <='f') {
    return((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <='F') {
    return((unsigned char)c - 'A' + 10);
  }
  return 0;
}

