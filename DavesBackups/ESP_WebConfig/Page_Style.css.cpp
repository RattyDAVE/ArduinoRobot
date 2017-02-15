#include "global.h"

const char PAGE_Style_css[] PROGMEM = R"=====(
body { color: #000000; font-family: avenir, helvetica, arial, sans-serif;  letter-spacing: 0.15em;} 
hr {    background-color: #eee;    border: 0 none;   color: #eee;    height: 1px; } 
.btn, .btn:link, .btn:visited {  
	border-radius: 0.3em;  
	border-style: solid;  
	border-width: 1px;  
color: #111;  
display: inline-block;  
	font-family: avenir, helvetica, arial, sans-serif;  
	letter-spacing: 0.15em;  
	margin-bottom: 0.5em;  
padding: 1em 0.75em;  
	text-decoration: none;  
	text-transform: uppercase;  
	-webkit-transition: color 0.4s, background-color 0.4s, border 0.4s;  
transition: color 0.4s, background-color 0.4s, border 0.4s; 
} 
.btn:hover, .btn:focus {
color: #7FDBFF;  
border: 1px solid #7FDBFF;  
	-webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
transition: background-color 0.3s, color 0.3s, border 0.3s; 
}
	.btn:active {  
color: #0074D9;  
border: 1px solid #0074D9;  
		-webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
transition: background-color 0.3s, color 0.3s, border 0.3s; 
	} 
	.btn--s 
	{  
		font-size: 12px; 
	}
	.btn--m { 
		font-size: 14px; 
	}
	.btn--l {  
		font-size: 20px;  border-radius: 0.25em !important; 
	} 
	.btn--full, .btn--full:link {
		border-radius: 0.25em; 
display: block;  
			margin-left: auto; 
			margin-right: auto; 
			text-align: center; 
width: 100%; 
	} 
	.btn--blue:link, .btn--blue:visited {
color: #fff;  
		background-color: #0074D9; 
	}
	.btn--blue:hover, .btn--blue:focus {
color: #fff !important;  
		background-color: #0063aa;  
		border-color: #0063aa; 
	}
	.btn--blue:active {
color: #fff; 
		background-color: #001F3F;  border-color: #001F3F; 
	}
	@media screen and (min-width: 32em) {
		.btn--full {  
			max-width: 16em !important; } 
	}
)=====";

void sendCSS() {
	server.send_P(200, text_css_type, PAGE_Style_css); 
	// server.send(200, "text/css", F("body { color: #000000; font-family: avenir, helvetica, arial, sans-serif;  letter-spacing: 0.15em;} \r\nhr {    background-color: #eee;    border: 0 none;   color: #eee;    height: 1px; } \r\n.btn, .btn:link, .btn:visited {  \r\n	border-radius: 0.3em;  \r\n	border-style: solid;  \r\n	border-width: 1px;  \r\ncolor: #111;  \r\ndisplay: inline-block;  \r\n	font-family: avenir, helvetica, arial, sans-serif;  \r\n	letter-spacing: 0.15em;  \r\n	margin-bottom: 0.5em;  \r\npadding: 1em 0.75em;  \r\n	text-decoration: none;  \r\n	text-transform: uppercase;  \r\n	-webkit-transition: color 0.4s, background-color 0.4s, border 0.4s;  \r\ntransition: color 0.4s, background-color 0.4s, border 0.4s; \r\n} \r\n.btn:hover, .btn:focus {\r\ncolor: #7FDBFF;  \r\nborder: 1px solid #7FDBFF;  \r\n	-webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  \r\ntransition: background-color 0.3s, color 0.3s, border 0.3s; \r\n}\r\n	.btn:active {  \r\ncolor: #0074D9;  \r\nborder: 1px solid #0074D9;  \r\n		-webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  \r\ntransition: background-color 0.3s, color 0.3s, border 0.3s; \r\n	} \r\n	.btn--s \r\n	{  \r\n		font-size: 12px; \r\n	}\r\n	.btn--m { \r\n		font-size: 14px; \r\n	}\r\n	.btn--l {  \r\n		font-size: 20px;  border-radius: 0.25em !important; \r\n	} \r\n	.btn--full, .btn--full:link {\r\n		border-radius: 0.25em; \r\ndisplay: block;  \r\n			margin-left: auto; \r\n			margin-right: auto; \r\n			text-align: center; \r\nwidth: 100%; \r\n	} \r\n	.btn--blue:link, .btn--blue:visited {\r\ncolor: #fff;  \r\n		background-color: #0074D9; \r\n	}\r\n	.btn--blue:hover, .btn--blue:focus {\r\ncolor: #fff !important;  \r\n		background-color: #0063aa;  \r\n		border-color: #0063aa; \r\n	}\r\n	.btn--blue:active {\r\ncolor: #fff; \r\n		background-color: #001F3F;  border-color: #001F3F; \r\n	}\r\n	@media screen and (min-width: 32em) {\r\n		.btn--full {  \r\n			max-width: 16em !important; } \r\n	}\r\n"));
}
 