#include "global.h"
#include "user.h"

void filldynamicdata() {        
  String values ="";
  values += "mydynamicdata|" + (String) + "This is filled by AJAX. Millis since start: " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
  server.send ( 200, "text/plain", values);   
}

const char PAGE_User[] PROGMEM = R"=====(
  <!doctype html>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <h1>Default user page</h1>     
   <div id="mydynamicdata"></div>
   <script>                
		window.onload = function () {
			load("microajax.js","js", function() {
				setValues("/filldynamicdata");  //-- this function calls the function on the ESP      
			});
		}
		function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
   </script>
)=====";


void userPage() {
/*	
  if (server.args() > 0 ) {  // Are there any POST/GET Fields ? 
    for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
      if (server.argName(i) == "firstname") {
        // Your processing for the transmitted form-variable 
        String fName = server.arg(i);
      }
    }
  }
*/  
  server.send_P (200, text_html_type, PAGE_User); 
//  server.send (200, "text/html", PAGE_User); 
}

void userInit() {
	// user init code here
  server.on( "/", userPage);
  server.on( "/filldynamicdata", filldynamicdata);
  //	server.on( "/style.css", sendCSS);

}

void userLoop() {
	// user code here
}


// config
void userInitConfig() {
  
}
void userLoadConfig(JsonObject& json) {
  
}
void userSaveConfig(JsonObject& json) {
  
}
