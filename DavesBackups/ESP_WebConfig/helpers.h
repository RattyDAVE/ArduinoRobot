#ifndef HELPERS_H
#define HELPERS_H
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <WiFiUdp.h>

typedef struct  _DateTime {
	byte hour;
	byte minute;
	byte second;
	int year;
	byte month;
	byte day;
	byte wday;
}DateTime;

//
// Summertime calculates the daylight saving for a given date.
//
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
boolean summertime(int year, byte month, byte day, byte hour, byte tzHours);

boolean checkRange(String Value);


void convertUnixTimeStamp( unsigned long timeStamp, DateTime* dateTime);

String getMacAddress();
String getAPSSID();

// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c);

// (based on https://code.google.com/p/avr-netino/)
String urldecode(String input);
#endif