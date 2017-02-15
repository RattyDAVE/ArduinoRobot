#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

///MAX72xx CODE
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
int pinCS = D4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
//END MAX 72xx Code


char ssid[] = "FishNet";  //  your network SSID (name)
char pass[] = "starwarsivanewhope";       // your network password

#include <Time.h>
#include <Timezone.h>

TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        //British Summer Time -  "UTC + 1" or GMT + 1
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         //Grenwich Mean Time -  "UTC + 0" or GMT 
Timezone LocalTime(BST, GMT);
TimeChangeRule *tcr; 



time_t prevDisplay = 0; // when the digital clock was displayed

unsigned int localPort = 2390;      // local port to listen for UDP packets

//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void setup()
{
  //Serial.begin(115200);
  // We start by connecting to a WiFi network
  matrix.setIntensity(1); // Use a value between 0 and 15 for brightness
  matrix.setCursor(0, 0);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    matrix.print(".");
    matrix.write();
  }
  udp.begin(localPort);
  setSyncProvider(getNtpTime);
}

void loop() {
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      //Serial.println(maketimestring());
      displaymessage(maketimestring());
    }
  }
}

time_t getNtpTime(){
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);  
  int cb = udp.parsePacket();
  if (!cb) {
    return 0;
  }
  else {
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;


/*
    // print Unix time:
    Serial.println(epoch);


    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
*/
    //return (epoch);
    return (LocalTime.toLocal(epoch, &tcr));
  }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address){
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}


String maketimestring() {
  String timestring;
  timestring = "";
  if (hour() < 10) timestring += '0';
  timestring += hour();

  if (second() % 2) timestring += ':';
  else timestring += ' ';

  if (minute() < 10) timestring += '0';
  timestring += minute();
  
  return timestring;
}

void displaymessage(String tape) {
  matrix.fillScreen(LOW);
  matrix.setCursor(1, 0);
  matrix.print(tape);
  matrix.write();
}
