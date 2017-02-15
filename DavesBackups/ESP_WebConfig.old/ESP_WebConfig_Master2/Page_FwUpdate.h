#ifndef PAGE_FwUpdate_H
#define PAGE_FwUpdate_H
//
//   The EXAMPLE PAGE
//
const char PAGE_FwUpdate[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Firmware Update</strong>
<hr>
<form method="POST" action="/fwupdated.html" enctype="multipart/form-data"><input type="file" accept=".bin" name="update"value="Update"><p><input type="submit" value="Update"></form>    
    
)=====";
#endif

void processUpdate()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
            }
        }
    }
    server.send ( 200, "text/html", PAGE_FwUpdate  ); 
}