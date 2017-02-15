
/*
* espNTPdigitalClock.ino
*
* Created: 5/6/2015 2:17:03 PM
* Author: Nigel with help from lots of others.
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
// #include <Wire.h>
// #include <Adafruit_LEDBackpack.h>
// #include <Adafruit_GFX.h>
// #include <Ticker.h>
// extern "C" {
//   #include "user_interface.h"
//}

char ssid[] = "FishNet";         //  your network SSID (name)
char pass[] = "starwarsivanewhope";             // your network password

#define localTimeOffset 0UL      // your localtime offset from UCT

#define UPDATESECS 1800UL      // update every 30 minutes
time_t update;
bool flag = true;
bool colon = false;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP            udp;
// Adafruit_7segment   disp = Adafruit_7segment();
// Ticker            showTime;

unsigned int localPort = 2390;               // local port to listen for UDP packets

const char* timeServer = "uk.pool.ntp.org";         //  my own local time server
const char* timeServer2 = "uk.pool.ntp.org";   // fall back to regional time server

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

void setup()
{
   Serial.begin(115200);
   Wire.begin(4,5);   // sda,sdl
   delay(10);
   disp.begin(0x70);   // default display for the LED backpack
   notime();         // set the display to 8888 with no colon
   
   Serial.println();
   Serial.println();

   // We start by connecting to a WiFi network
   Serial.print("Connecting to ");
   Serial.println(ssid);
   WiFi.begin(ssid, pass);
   WiFi.mode(WIFI_STA);
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   WiFi.disconnect();
   showTime.attach(0.333,doDisplay);
   setNTPtime();
}

void loop()
{
   if(now() > update) setNTPtime();
   if(flag && (now() % 10UL == 0UL)) printTime("Now ",now());
   else flag = true;
   delay(900);
}

void doDisplay()
{
   if (year() > 2014){
      disp.print(hourFormat12() * 100 + minute()); // I like to see it as a 12 hour clock
      disp.drawColon(colon);
      disp.writeDisplay();
   }
}

void setNTPtime()
{
   WiFi.begin(ssid, pass);
   WiFi.mode(WIFI_STA);
   colon = false;
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   Serial.println();
   time_t epoch = 0UL;
   if((epoch =  getFromNTP(timeServer)) != 0 || (epoch =  getFromNTP(timeServer2)) != 0){ // get from time server
      epoch -= 2208988800UL + localTimeOffset;
      setTime(epoch += dst(epoch));
      update = now() +  UPDATESECS; // set next update time if successful
      colon = true;
   }
   else{
      colon = false;
      update = now() + 30; // or try again in 30 seconds
   }
   WiFi.disconnect();
}

unsigned long getFromNTP(const char* server)
{
   udp.begin(localPort);
   sendNTPpacket(server);   // send an NTP packet to a time server
   delay(1000);         // wait to see if a reply is available
   int cb = udp.parsePacket();
   if (!cb) {
      Serial.println("no packet yet");
      return 0UL;
   }
   Serial.print("packet received, length=");
   Serial.println(cb);
   // We've received a packet, read the data from it
   udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

   //the timestamp starts at byte 40 of the received packet and is four bytes,
   // or two words, long. First, extract the two words:

   unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
   unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
   // combine the four bytes (two words) into a long integer
   // this is NTP time (seconds since Jan 1 1900):
   udp.stop();
   return (unsigned long) highWord << 16 | lowWord;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(const char* server)
{
   Serial.print("sending NTP packet to ");
   Serial.println(server);
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
   udp.beginPacket(server, 123); //NTP requests are to port 123
   udp.write(packetBuffer, NTP_PACKET_SIZE);
   udp.endPacket();
}

int dst (time_t t) // calculate if summertime in Europe
{
   tmElements_t te;
   te.Year = year(t)-1970;
   te.Month =3;
   te.Day =1;
   te.Hour = 0;
   te.Minute = 0;
   te.Second = 0;
   time_t dstStart,dstEnd, current;
   dstStart = makeTime(te);
   dstStart = lastSunday(dstStart);
   dstStart += 2*SECS_PER_HOUR;  //2AM
   te.Month=10;
   dstEnd = makeTime(te);
   dstEnd = lastSunday(dstEnd);
   dstEnd += SECS_PER_HOUR;  //1AM
   if (t>=dstStart && t<dstEnd) return (3600);  //Add back in one hours worth of seconds - DST in effect
   else return (0);  //NonDST
}

time_t lastSunday(time_t t)
{
   t = nextSunday(t);  //Once, first Sunday
   if(day(t) < 4) return t += 4 * SECS_PER_WEEK;
   else return t += 3 * SECS_PER_WEEK;
}

void printTime(const char s[],time_t t)
{
   flag =false;
   Serial.print(s);
   printLeading0(hour(t));
   Serial.print(":");
   printLeading0(minute(t));
   Serial.print(":");
   printLeading0(second(t));
   Serial.print(" ");
   Serial.print(dayStr(weekday(t)));
   Serial.print(" ");
   Serial.print(day(t));
   Serial.print(" ");
   Serial.print(monthStr(month(t)));
   Serial.print(" ");
   Serial.println(year(t));
}

void printLeading0(int n)
{
   if(n < 10) Serial.print("0");
   Serial.print(n);
}

void notime(void)
{
//   disp.drawColon(false);
//   disp.print(8888);
//   disp.writeDisplay();
}

