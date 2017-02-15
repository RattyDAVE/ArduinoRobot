/* 
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public



  
  */

 #define PROGMEM
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"
#include "global.h"
/*
Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "Page_FwUpdate.h"
#include "example.h"


#define ACCESS_POINT_NAME  "ESP"				
#define ACCESS_POINT_PASSWORD  "12345678" 
#define AdminTimeOut 180  // Defines the Time in Seconds, when the Admin-Mode will be diabled


void setup ( void ) {
	EEPROM.begin(512);
	Serial.begin(115200);
	delay(500);
	Serial.println("Starting ES8266");
	if (!ReadConfig())
	{
		// DEFAULT CONFIG
		config.ssid = "SSID";
		config.password = "PASSWORD";
		config.dhcp = true;
		config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
		config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
		config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
		config.ntpServerName = "0.de.pool.ntp.org";
		config.Update_Time_Via_NTP_Every =  0;
		config.timezone = -10;
		config.daylight = true;
		config.DeviceName = "Not Named";
		config.AutoTurnOff = false;
		config.AutoTurnOn = false;
		config.TurnOffHour = 0;
		config.TurnOffMinute = 0;
		config.TurnOnHour = 0;
		config.TurnOnMinute = 0;
		WriteConfig();
		Serial.println("General config applied");
	}
	
	
	if (AdminEnabled)
	{
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
	}
	else
	{
		WiFi.mode(WIFI_STA);
	}

	ConfigureWifi();
	

	server.on ( "/", processExample  );
	server.on ( "/admin/filldynamicdata", filldynamicdata );
	
	server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );


	server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
	server.on ( "/config.html", send_network_configuration_html );
	server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
	server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/fwupdate.html", []() { server.send ( 200, "text/html", PAGE_FwUpdate );  } );
	server.on ( "/general.html", send_general_html  );
//	server.on ( "/example.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
	server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
	server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
	server.on ( "/admin/values", send_network_configuration_values_html );
	server.on ( "/admin/connectionstate", send_connection_state_values_html );
	server.on ( "/admin/infovalues", send_information_values_html );
	server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
	server.on ( "/admin/generalvalues", send_general_configuration_values_html);
	server.on ( "/admin/devicename",     send_devicename_value_html);
 
	server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
	server.begin();
	Serial.println( "HTTP server started" );
	tkSecond.attach(1,Second_Tick);
	UDPNTPClient.begin(2390);  // Port for NTP receive

     server.onFileUpload([](){
if(server.uri() != "/fwupdated.html") return;
    HTTPUpload& upload = server.upload();
if(upload.status == UPLOAD_FILE_START){
     WiFiUDP::stopAll();
     Serial.println("FW Updating...");
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){//start with max available size
          //Update.printError(Serial);
         }
      } else if(upload.status == UPLOAD_FILE_WRITE){
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
           Update.printError(Serial);
         }
      } else if(upload.status == UPLOAD_FILE_END){
       if(Update.end(true)){ //true to set the size to the current progress
          Serial.println("Updating Success");
       } else {
          Serial.println("Updating Error");
          ESP.restart();
       }
    }
    yield();
    });
    server.on("/fwupdated.html", HTTP_POST, [](){
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "<meta http-equiv='refresh' content='15; URL=admin.html'><p>Firmware Upgrade Success<p>Device Rebooting Pleas Wait...</form>");
    delay(500);
    ESP.restart();
    });
}

void loop ( void ) {
	if (AdminEnabled)
	{
		if (AdminTimeOutCounter > AdminTimeOut)
		{
			 AdminEnabled = false;
			 Serial.println("Admin Mode disabled!");
			 WiFi.mode(WIFI_STA);
		}
	}
	if (config.Update_Time_Via_NTP_Every  > 0 )
	{
		if (cNTP_Update > 5 && firstStart)
		{
			NTPRefresh();
			cNTP_Update =0;
			firstStart = false;
		}
		else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
		{

			NTPRefresh();
			cNTP_Update =0;
		}
	}

	if(DateTime.minute != Minute_Old)
	{
		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOn)
		 {
			 if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
			 {
				  Serial.println("SwitchON");
			 }
		 }


		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOff)
		 {
			 if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
			 {
				  Serial.println("SwitchOff");
			 }
		 }
	}
	server.handleClient();


	/*
	*    Your Code here
	*/

	if (Refresh)  
	{
		Refresh = false;
	  Serial.println("Refreshing...");
		Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
	}



 


}

