#ifndef GLOBAL_H
#define GLOBAL_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FS.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <WiFiUdp.h>
#include "ArduinoJson.h"

#include "user.h"
#include "helpers.h"

extern ESP8266WebServer server;									// The Webserver
extern boolean firstStart;										// On firststart = true, NTP will try to get a valid time
extern DateTime dateTime;											// Global DateTime structure, will be refreshed every Second
extern WiFiUDP UDPNTPClient;											// NTP Client
extern unsigned long unixTimestamp;								// GLOBALTIME  ( Will be set by NTP)
extern boolean refresh; // For Main Loop, to refresh things like GPIO / WS2812
extern unsigned long tick;  // tick counter

extern Ticker ticker;												// Second - Timer for Updating Datetime Structure
extern boolean adminEnabled;		// Enable Admin Mode for a given Time
extern const int adminTimeOut;  // Defines the Time in Seconds, when the Admin-Mode will be diabled : 0 = disable

#define ACCESS_POINT_NAME  "ESP"				
#define ACCESS_POINT_PASSWORD  "12345678" 


#define NTP_SERVERNAME_MAX_SIZE	172
#define DEVICE_NAME_MAX_SIZE		20
#define SSID_MAX_SIZE						32
#define WIFI_PASSWORD_MAX_SIZE		64
#define DEVICE_NAME_MAX_SIZE		20

typedef struct _SystemConfig {
	String ssid;
	String wifiPassword;
	unsigned char IP[4];
	unsigned char netmask[4];
	unsigned char gateway[4];
	boolean dhcp;
	String ntpServerName;
	long ntpUpdatePeriod;
	long timezone;
	boolean daylight;
	String deviceName;
	UserConfig userConfig;
} SystemConfig;



//extern strConfig config;
extern SystemConfig config;

// CONFIGURATION
void configureWifi();
void loadConfig();
void initConfig();
boolean saveConfig();
// !CONFIGURATION

// NTP
bool NTPRefresh();
// !NTP

// tick
#define TICK_PERIOD	60  // every x seconds
void tickHandler();

// Serial
void serial();


//HTML
extern const char text_html_type[];
extern const char text_plain_type[];
extern const char text_css_type[];


#endif