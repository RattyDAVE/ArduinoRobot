#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <WiFiUdp.h>



#include "helpers.h"
#include "global.h"
/*
Include the HTML, STYLE and Script "Pages"
*/
#include "pages.h"
#include "user.h"


void reset() {
	// erase configuration file
  SPIFFS.remove("/config.json");
	delay(2000);
	ESP.restart();	
}

void setup ( void ) {
	Serial.begin(115200);
Serial.setDebugOutput(true);
	delay(3000);
  Serial.println("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }
	Serial.println("Starting ESP8266");
// initConfig();
	loadConfig();
/*	
	initConfig();  // for test only
	saveConfig();  // for test only
*/	
	if (adminEnabled) {
		Serial.println("Config Mode ON");
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP( getAPSSID().c_str() , ACCESS_POINT_PASSWORD);
		//	server.on( "/", showAdminMainPage);
	} else {
		WiFi.mode(WIFI_STA);
	}
	configureWifi();
	server.on( "/favicon.ico", []() { server.send(200, "text/html", ""); });
//server.on( "/admin.html", showAdminMainPage);
	server.on( "/reset.html", reset);
//server.on( "/config.html", send_network_configuration_html);
//server.on( "/info.html", sendInformationPage);
//server.on( "/ntp.html", send_NTP_configuration_html);
	server.on( "/admin.html", send_general_html);
server.on( "/style.css", sendCSS);
	server.on( "/microajax.js", sendJavascript);
//server.on( "/admin/values", send_network_configuration_values_html);
	server.on( "/connectionstate", send_connection_state_values_html);
//server.on( "/admin/infovalues", send_information_values_html);
//server.on( "/admin/ntpvalues", send_NTP_configuration_values_html);
	server.on( "/generalvalues", send_general_configuration_values_html);
	server.on( "/devicename",     send_devicename_value_html);
 	server.onNotFound( []() { Serial.println("Page Not Found"); server.send(400, "text/html", "Page not Found"); });
 	userInit();
	server.begin();
	Serial.println( "HTTP server started" );
	ticker.attach(TICK_PERIOD, tickHandler);
	UDPNTPClient.begin(2390);  // Port for NTP receive
}

void loop ( void ) {
  if (Serial.available())
    serial();

	// admin mode
	if (adminEnabled) {
		if (adminTimeOut != 0 && tick > adminTimeOut) {
			adminEnabled = false;
			Serial.println("Config Mode OFF");
			WiFi.mode(WIFI_STA);
		}
	}
	// NTP update	
	if (config.ntpUpdatePeriod  > 0 ) {
		if (firstStart == true) {
			if (NTPRefresh() == true) {
				firstStart = false;
			}
		} else {
			if (tick > 86400 /* (config.ntpUpdatePeriod * 60) */) {
				if (NTPRefresh() == false) {
					firstStart = true;  // try again on next tick
				}
			}
		}
	}
	// WEB Server
	server.handleClient();
	userLoop();
	// traces
	if (refresh == true) {
		refresh = false;
		if (adminEnabled == true) {
			Serial.printf("FreeMem:%d \r\n",ESP.getFreeHeap());
		} else {
			Serial.printf("FreeMem:%d %02d:%02d:%02d %d.%02d.%02d \r\n",ESP.getFreeHeap() , dateTime.hour,dateTime.minute, dateTime.second, dateTime.year, dateTime.month, dateTime.day);
		}
	}
  delay(1000);
}

