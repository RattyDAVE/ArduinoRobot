#include "global.h"

ESP8266WebServer server(80);									// The Webserver
boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
DateTime dateTime;											// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;											// NTP Client
unsigned long unixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
unsigned long tick = 0;  // tick counter
Ticker ticker;												// Second - Timer for Updating Datetime Structure
boolean adminEnabled = false;		// Enable Admin Mode for a given Time
const int adminTimeOut  = 60;  // Defines the Time in Seconds, when the Admin-Mode will be diabled : 0 = disable

const char text_html_type[] PROGMEM = "text/html";
const char text_plain_type[] PROGMEM = "text/plain";
const char text_css_type[] PROGMEM = "text/css";

SystemConfig config;

void configureWifi() {
	Serial.println("Configuring Wifi");
	WiFi.begin (config.ssid.c_str(), config.wifiPassword.c_str());
	if (!config.dhcp) {
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3]),  
								IPAddress(config.gateway[0],config.gateway[1],config.gateway[2],config.gateway[3]), 
								IPAddress(config.netmask[0],config.netmask[1],config.netmask[2],config.netmask[3]));
	}
}


unsigned long ntpUnixTime (UDP &udp) {
  int udpInited = udp.begin(123); // open socket on arbitrary port
  // Only the first four bytes of an outgoing NTP packet need to be set
  // appropriately, the rest can be whatever.
  const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header

  // Fail if WiFiUdp.begin() could not init a socket
  if (! udpInited)
    return 0;

  // Clear received data from possible stray received packets
  udp.flush();

  // Send an NTP request
  if (! (udp.beginPacket(config.ntpServerName.c_str(), 123) // 123 is the NTP port
	 && udp.write((byte *)&ntpFirstFourBytes, 48) == 48
	 && udp.endPacket()))
    return 0;				// sending request failed

  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 150;		// poll every this many ms
  const byte maxPoll = 15;		// poll up to this many times
  int pktLen;				// received packet length
  for (byte i=0; i<maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == 48)
      break;
    delay(pollIntv);
  }
  if (pktLen != 48)
    return 0;				// no correct packet received

  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  // Read the integer part of sending time
  unsigned long time = udp.read();	// NTP time
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  // Round to the nearest second if we want accuracy
  // The fractionary part is the next byte divided by 256: if it is
  // greater than 500ms we round to the next second; we also account
  // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
  // additionally, we account for how much we delayed reading the packet
  // since its arrival, which we assume on average to be pollIntv/2.
  time += (udp.read() > 115 - pollIntv/8);

  // Discard the rest of the packet
  udp.flush();

  return time - 2208988800ul;		// convert NTP time to Unix time
}

bool NTPRefresh() {
	if (WiFi.status() == WL_CONNECTED) {
		WiFiUDP udp;
		unixTimestamp = ntpUnixTime(udp);
		// Serial.println(unixTimestamp);

		if (unixTimestamp == 0) {
			Serial.println("NTP update failed!");
			return false;
		}
		return true;
	}
	return false;
}

void tickHandler() {
	DateTime tempDateTime;
	tick += TICK_PERIOD;
	unixTimestamp += TICK_PERIOD;
	convertUnixTimeStamp(unixTimestamp +  (config.timezone *  360) , &tempDateTime);
	if (config.daylight) { 
		if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0)) {
			convertUnixTimeStamp(unixTimestamp +  (config.timezone *  360) + 3600, &dateTime);
		} else {
			dateTime = tempDateTime;
		}
	} else {
		dateTime = tempDateTime;
	}
	refresh = true;  // will print freemem and date/time on serial
}


/////////////////// CONFIG //////////////////////
void initConfig() {
	Serial.println("Default config");
	config.ssid =  "YOURSSID";
	config.wifiPassword = "PASSWORD";
	config.dhcp = true;
	config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
	config.netmask[0] = 255; config.netmask[1] = 255; config.netmask[2] = 255; config.netmask[3] = 0;
	config.gateway[0] = 192; config.gateway[1] = 168; config.gateway[2] = 1; config.gateway[3] = 1;
	config.ntpServerName  = "0.fr.pool.ntp.org";
	config.ntpUpdatePeriod =  1440;  // every 24 hours
	config.timezone = 10;
	config.daylight = true;
	config.deviceName = "Unnamed";
	userInitConfig();
}

void loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
  	initConfig();
  	saveConfig();
		adminEnabled = true;
		return;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
  	adminEnabled = true;
	  return;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  //StaticJsonBuffer<1024> jsonBuffer;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
		adminEnabled = true;
    return;
  }

	if (json["header"] != 1236) {
    Serial.println("Bad config file header");
  	initConfig();
  	saveConfig();
		adminEnabled = true;
    return;
	}
	
  config.ssid = (const char *)json["ssid"];
  config.wifiPassword = (const char *)json["wifiPassword"];
  config.IP[0] = json["IP"][0];
  config.IP[1] = json["IP"][1];
  config.IP[2] = json["IP"][2];
  config.IP[3] = json["IP"][3];

  config.netmask[0] = json["netmask"][0];
  config.netmask[1] = json["netmask"][1];
  config.netmask[2] = json["netmask"][2];
  config.netmask[3] = json["netmask"][3];

  config.gateway[0] = json["gateway"][0];
  config.gateway[1] = json["gateway"][1];
  config.gateway[2] = json["gateway"][2];
  config.gateway[3] = json["gateway"][3];
  
  config.dhcp = json["dhcp"] == 1 ?  true : false;

	config.ntpServerName = (const char *)json["ntpServerName"];
	config.ntpUpdatePeriod = json["ntpUpdatePeriod"];
	config.timezone = json["timezone"];
  config.daylight = json["daylight"] == 1 ?  true : false;
	config.deviceName = (const char *)json["deviceName"];
	userLoadConfig(json);
/*
	Serial.print("wifipassword : ");
  Serial.println(config.wifiPassword);
	Serial.print("config.ntpUpdatePeriod : ");
  Serial.println(config.ntpUpdatePeriod);
*/  
}

boolean saveConfig() {
//  StaticJsonBuffer<1024> jsonBuffer;
  DynamicJsonBuffer jsonBuffer;

  JsonObject& json = jsonBuffer.createObject();

  json["header"] = 1236;
  json["ssid"] = config.ssid.c_str();
  json["wifiPassword"] = config.wifiPassword.c_str();
  JsonArray& ip = json.createNestedArray("IP");
	ip.add(config.IP[0]);
	ip.add(config.IP[1]);
	ip.add(config.IP[2]);
	ip.add(config.IP[3]);
  JsonArray& netmask = json.createNestedArray("netmask");
	netmask.add(config.netmask[0]);
	netmask.add(config.netmask[1]);
	netmask.add(config.netmask[2]);
	netmask.add(config.netmask[3]);
  JsonArray& gateway = json.createNestedArray("gateway");
	gateway.add(config.gateway[0]);
	gateway.add(config.gateway[1]);
	gateway.add(config.gateway[2]);
	gateway.add(config.gateway[3]);
	json["dhcp"] = config.dhcp == true ? 1 : 0;
  json["ntpServerName"] = config.ntpServerName.c_str();
  json["ntpUpdatePeriod"] = config.ntpUpdatePeriod;
  json["timezone"] = config.timezone;
	json["daylight"] = config.daylight == true ? 1 : 0;
  json["deviceName"] = config.deviceName.c_str();
	
	userSaveConfig(json);

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  } 

  json.printTo(configFile);
  return true;
}
