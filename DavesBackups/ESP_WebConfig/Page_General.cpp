#include "global.h"

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Device Name</td>
	<td><input type="text" id="devicename" name="devicename" value="" maxlength="20"></td>
	<input type="hidden" name="name" value="1">
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--s btn--blue" value="Save"></td></tr>
</table>
</form>
<hr><strong>Wifi Configuration</strong><br>
<form action='' method='get'>
<table border='0'  cellspacing='0' cellpadding='3' style='width:310px' >
<tr><td align='right'>SSID:</td><td><input type='text' id='ssid' name='ssid' value='' maxlength='32'></td></tr>
<tr><td align='right'>Password:</td><td><input type='text' id='password' name='password' value='' maxlength='64'></td></tr>
<tr><td align='right'>DHCP:</td><td><input type='checkbox' id='dhcp' name='dhcp'></td></tr>
<tr><td align='right'>IP:     </td><td><input type='text' id='ip_0' name='ip_0' size='3'>.<input type='text' id='ip_1' name='ip_1' size='3'>.<input type='text' id='ip_2' name='ip_2' size='3'>.<input type='text' id='ip_3' name='ip_3' value='' size='3'></td></tr>
<tr><td align='right'>Netmask:</td><td><input type='text' id='nm_0' name='nm_0' size='3'>.<input type='text' id='nm_1' name='nm_1' size='3'>.<input type='text' id='nm_2' name='nm_2' size='3'>.<input type='text' id='nm_3' name='nm_3' size='3'></td></tr>
<tr><td align='right'>Gateway:</td><td><input type='text' id='gw_0' name='gw_0' size='3'>.<input type='text' id='gw_1' name='gw_1' size='3'>.<input type='text' id='gw_2' name='gw_2' size='3'>.<input type='text' id='gw_3' name='gw_3' size='3'></td></tr>
<input type="hidden" name="name" value="2">
<tr><td colspan='2' align='center'><input type='submit' style='width:150px' class='btn btn--s btn--blue' value='Save'></td></tr>
</table>
</form>
<hr><strong>Network Information</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID :</td><td><span id="x_ssid"></span></td></tr>
<tr><td align="right">IP :</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Netmask :</td><td><span id="x_netmask"></span></td></tr>
<tr><td align="right">Gateway :</td><td><span id="x_gateway"></span></td></tr>
<tr><td align="right">Mac :</td><td><span id="x_mac"></span></td></tr>
</table>
<hr>
<strong>Connection State:</strong><div id='connectionstate'>N/A</div>
<hr>
<strong>Networks:</strong><br>
<table border='0'  cellspacing='3' style='width:310px' >
<tr><td><div id='networks'>Scanning...</div></td></tr>
<tr><td align='center'><a href='javascript:GetState()' style='width:150px' class='btn btn--s btn--blue'>Refresh</a></td></tr>
</table>

<hr><strong>NTP Configuration</strong><br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Server:</td><td><input type="text" id="ntpserver" name="ntpserver" maxlength="172" value=""></td></tr>
<tr><td align="right">Update:</td><td><input type="text" id="update" name="update" size="3"maxlength="6" value=""> minutes (0=disable)</td></tr>
<tr><td>Timezone</td><td>
<select  id="tz" name="tz">
	<option value="-120">(GMT-12:00)</option>
	<option value="-110">(GMT-11:00)</option>
	<option value="-100">(GMT-10:00)</option>
	<option value="-90">(GMT-09:00)</option>
	<option value="-80">(GMT-08:00)</option>
	<option value="-70">(GMT-07:00)</option>
	<option value="-60">(GMT-06:00)</option>
	<option value="-50">(GMT-05:00)</option>
	<option value="-40">(GMT-04:00)</option>
	<option value="-35">(GMT-03:30)</option>
	<option value="-30">(GMT-03:00)</option>
	<option value="-20">(GMT-02:00)</option>
	<option value="-10">(GMT-01:00)</option>
	<option value="0">(GMT+00:00)</option>
	<option value="10">(GMT+01:00)</option>
	<option value="20">(GMT+02:00)</option>
	<option value="30">(GMT+03:00)</option>
	<option value="35">(GMT+03:30)</option>
	<option value="40">(GMT+04:00)</option>
	<option value="45">(GMT+04:30)</option>
	<option value="50">(GMT+05:00)</option>
	<option value="55">(GMT+05:30)</option>
	<option value="57">(GMT+05:45)</option>
	<option value="60">(GMT+06:00)</option>
	<option value="65">(GMT+06:30)</option>
	<option value="70">(GMT+07:00)</option>
	<option value="80">(GMT+08:00)</option>
	<option value="90">(GMT+09:00)</option>
	<option value="95">(GMT+09:30)</option>
	<option value="100">(GMT+10:00)</option>
	<option value="110">(GMT+11:00)</option>
	<option value="120">(GMT+12:00)</option>
	<option value="120">(GMT+12:00)</option>
	<option value="130">(GMT+13:00)</option>
</select>
</td></tr>
<tr><td align="right">Daylight saving:</td><td><input type="checkbox" id="dst" name="dst"></td></tr>
<tr><td align="right">NTP Time:</td><td><span id="x_ntp"></span></td></tr>
<input type="hidden" name="name" value="3">
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--s btn--blue" value="Save"></td></tr>
</table>
</form>
<script>
// network config
function GetState() {
	setValues('/connectionstate');
}
function selssid(value) {
	document.getElementById('ssid').value = value; 
}
// !network config
window.onload = function () {
	load("microajax.js","js", function() {
		setValues("/generalvalues");
		// network config
		setTimeout(GetState,3000);
		// !network config
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
</script>
)=====";


/*
window.onload = function () {
	load("style.css","css", function() {
		load("microajax.js","js", function() {
			setValues("/generalvalues");
			// network config
			setTimeout(GetState,3000);
			// !network config
		});
	});
}
*/

const char PAGE_WaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=admin.html">
Please Wait....Configuring and Restarting.
)=====";


// Functions for this Page
void send_devicename_value_html() {
	String values = "devicename|" + (String) config.deviceName + "|div\n";
	// network config
	String state = "N/A";
	String networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILABLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECTION FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";
	
	int n = WiFi.scanNetworks();
 	if (n == 0) {
		networks = "<font color='#FF0000'>No networks found!</font>";
	} else {
		networks = "Found " +String(n) + " Networks<br>";
		networks += "<table border='0' cellspacing='0' cellpadding='3'>";
		networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i) {
			int quality=0;
			if (WiFi.RSSI(i) <= -100) {
				quality = 0;
			} else {
				if (WiFi.RSSI(i) >= -50) {
					quality = 100;
				} else {
					quality = 2 * (WiFi.RSSI(i) + 100);
				}
			}
			networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*")  + "</td></tr>";
		}
		networks += "</table>";
	}
   
	values += "connectionstate|" +  state + "|div\n";
	values += "networks|" +  networks + "|div\n";	
	server.send ( 200, "text/plain", values);
	//Serial.println(__FUNCTION__); 
}

void send_general_html() {
	int saveId = 0;
	if (server.args() > 0) { // Save Settings
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "name")  saveId = server.arg(i).toInt();
			if (server.argName(i) == "devicename") config.deviceName = urldecode(server.arg(i));
			// network config
			if (server.argName(i) == "ssid") config.ssid = urldecode(server.arg(i));
			if (server.argName(i) == "password") config.wifiPassword = urldecode(server.arg(i)); 
			if (server.argName(i) == "ip_0") if (checkRange(server.arg(i))) 	config.IP[0] =  server.arg(i).toInt();
			if (server.argName(i) == "ip_1") if (checkRange(server.arg(i))) 	config.IP[1] =  server.arg(i).toInt();
			if (server.argName(i) == "ip_2") if (checkRange(server.arg(i))) 	config.IP[2] =  server.arg(i).toInt();
			if (server.argName(i) == "ip_3") if (checkRange(server.arg(i))) 	config.IP[3] =  server.arg(i).toInt();
			if (server.argName(i) == "nm_0") if (checkRange(server.arg(i))) 	config.netmask[0] =  server.arg(i).toInt();
			if (server.argName(i) == "nm_1") if (checkRange(server.arg(i))) 	config.netmask[1] =  server.arg(i).toInt();
			if (server.argName(i) == "nm_2") if (checkRange(server.arg(i))) 	config.netmask[2] =  server.arg(i).toInt();
			if (server.argName(i) == "nm_3") if (checkRange(server.arg(i))) 	config.netmask[3] =  server.arg(i).toInt();
			if (server.argName(i) == "gw_0") if (checkRange(server.arg(i))) 	config.gateway[0] =  server.arg(i).toInt();
			if (server.argName(i) == "gw_1") if (checkRange(server.arg(i))) 	config.gateway[1] =  server.arg(i).toInt();
			if (server.argName(i) == "gw_2") if (checkRange(server.arg(i))) 	config.gateway[2] =  server.arg(i).toInt();
			if (server.argName(i) == "gw_3") if (checkRange(server.arg(i))) 	config.gateway[3] =  server.arg(i).toInt();
			if (server.argName(i) == "dhcp") config.dhcp = true;
			// NTP config
			if (server.argName(i) == "ntpserver") config.ntpServerName = urldecode( server.arg(i)).c_str();
			if (server.argName(i) == "update") config.ntpUpdatePeriod =  server.arg(i).toInt(); 
			if (server.argName(i) == "tz") config.timezone =  server.arg(i).toInt(); 
			if (server.argName(i) == "dst") config.daylight = true; 

		}
		saveConfig();
		if (saveId == 2) {  // wifi configuration change
			server.send_P( 200, text_html_type, PAGE_WaitAndReload );
			configureWifi();
			return;
		}
		if (saveId == 3) {  // wifi configuration change
			firstStart = true;
		}

	} 
//	server.send ( 200, "text/html", F("<meta name='viewport' content='width=device-width, initial-scale=1' />\r\n<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />\r\n<a href='admin.html'  class='btn btn--s'><</a>&nbsp;&nbsp;<strong>General Settings</strong>\r\n<hr>\r\n<form action='' method='post'>\r\n<table border='0'  cellspacing='0' cellpadding='3' >\r\n<tr>\r\n	<td align='right'>Name of Device</td>\r\n	<td><input type='text' id='devicename' name='devicename' value='' maxlength='20'></td>\r\n</tr>\r\n<tr><td colspan='2' align='center'><input type='submit' style='width:150px' class='btn btn--m btn--blue' value='Save'></td></tr>\r\n</table>\r\n</form>\r\n<script>\r\n\r\n \r\n\r\nwindow.onload = function ()\r\n{\r\n	load('style.css','css', function() \r\n	{\r\n		load('microajax.js','js', function() \r\n		{\r\n				setValues('/admin/generalvalues');\r\n		});\r\n	});\r\n}\r\nfunction load(e,t,n){if('js'==t){var a=document.createElement('script');a.src=e,a.type='text/javascript',a.async=!1,a.onload=function(){n()},document.getElementsByTagName('head')[0].appendChild(a)}else if('css'==t){var a=document.createElement('link');a.href=e,a.rel='stylesheet',a.type='text/css',a.async=!1,a.onload=function(){n()},document.getElementsByTagName('head')[0].appendChild(a)}}\r\n\r\n\r\n\r\n</script>\r\n"));
	server.send_P ( 200, text_html_type, PAGE_AdminGeneralSettings);
	//Serial.println(__FUNCTION__); 
}

void send_general_configuration_values_html() {
	String values = "devicename|" +  config.deviceName +  "|input\n";
	// network config
	values += "ssid|" + config.ssid + "|input\n";
	values += "password|" +  config.wifiPassword + "|input\n";
	values += "ip_0|" +  (String) config.IP[0] + "|input\n";
	values += "ip_1|" +  (String) config.IP[1] + "|input\n";
	values += "ip_2|" +  (String) config.IP[2] + "|input\n";
	values += "ip_3|" +  (String) config.IP[3] + "|input\n";
	values += "nm_0|" +  (String) config.netmask[0] + "|input\n";
	values += "nm_1|" +  (String) config.netmask[1] + "|input\n";
	values += "nm_2|" +  (String) config.netmask[2] + "|input\n";
	values += "nm_3|" +  (String) config.netmask[3] + "|input\n";
	values += "gw_0|" +  (String) config.gateway[0] + "|input\n";
	values += "gw_1|" +  (String) config.gateway[1] + "|input\n";
	values += "gw_2|" +  (String) config.gateway[2] + "|input\n";
	values += "gw_3|" +  (String) config.gateway[3] + "|input\n";
	// network info
	values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
	values += "x_ip|" +  (String)WiFi.localIP()[0] + "." +  (String)WiFi.localIP()[1] + "." +  (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3] +  "|div\n";
	values += "x_gateway|" +  (String)WiFi.gatewayIP()[0] + "." +  (String)WiFi.gatewayIP()[1] + "." +  (String)WiFi.gatewayIP()[2] + "." + (String)WiFi.gatewayIP()[3] +  "|div\n";
	values += "x_netmask|" +  (String)WiFi.subnetMask()[0] + "." +  (String)WiFi.subnetMask()[1] + "." +  (String)WiFi.subnetMask()[2] + "." + (String)WiFi.subnetMask()[3] +  "|div\n";
	values += "x_mac|" + getMacAddress() +  "|div\n";
	values += "dhcp|" +  (String) (config.dhcp ? "checked" : "") + "|chk\n";
	// NTP config
	values += "ntpserver|" + (String) config.ntpServerName + "|input\n";
	values += "update|" +  (String) config.ntpUpdatePeriod + "|input\n";
	values += "tz|" +  (String) config.timezone + "|input\n";
	values += "dst|" +  (String) (config.daylight ? "checked" : "") + "|chk\n";
	values += "x_ntp|" +  (String)dateTime.hour + ":" + (String)dateTime.minute +  ":"  + (String)dateTime.second + " " + (String)dateTime.year + "-" + 
												(String)dateTime.month + "-" + (String)dateTime.day +  "|div\n";

	server.send ( 200, "text/plain", values);
	//Serial.println(__FUNCTION__); 
}

void send_connection_state_values_html() {
	String state = "N/A";
	String networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";
	
	int n = WiFi.scanNetworks();
 	if (n == 0) {
		networks = "<font color='#FF0000'>No networks found!</font>";
	} else {
		networks = "Found " +String(n) + " Networks<br>";
		networks += "<table border='0' cellspacing='0' cellpadding='3'>";
		networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i) {
			int quality=0;
			if (WiFi.RSSI(i) <= -100) {
				quality = 0;
			} else {
				if (WiFi.RSSI(i) >= -50) {
					quality = 100;
				} else {
					quality = 2 * (WiFi.RSSI(i) + 100);
				}
			}
			networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*")  + "</td></tr>";
		}
		networks += "</table>";
	}
   
	String values ="";
	values += "connectionstate|" +  state + "|div\n";
	values += "networks|" +  networks + "|div\n";
	server.send ( 200, "text/plain", values);
	//Serial.println(__FUNCTION__); 
}
