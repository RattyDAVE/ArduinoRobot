/********************************************************************************************\
* Process data from Serial Interface
\*********************************************************************************************/
#include "global.h"
/********************************************************************************************\
* Find positional parameter in a char string
\*********************************************************************************************/
bool GetArgv(char *string, char *argv, int argc) {
  int string_pos = 0, argv_pos = 0, argc_pos = 0;
  char c, d;

  while (string_pos < strlen(string)) {
    c = string[string_pos];
    d = string[string_pos + 1];

    if       (c == ' ' && d == ' ') {}
    else if  (c == ' ' && d == ',') {}
    else if  (c == ',' && d == ' ') {}
    else if  (c == ' ' && d >= 33 && d <= 126) {}
    else if  (c == ',' && d >= 33 && d <= 126) {}
    else {
      argv[argv_pos++] = c;
      argv[argv_pos] = 0;

      if (d == ' ' || d == ',' || d == 0) {
        argv[argv_pos] = 0;
        argc_pos++;

        if (argc_pos == argc) {
          return true;
        }

        argv[0] = 0;
        argv_pos = 0;
        string_pos++;
      }
    }
    string_pos++;
  }
  return false;
}

extern "C" {
#include <user_interface.h>
}

#define INPUT_COMMAND_SIZE          80
void ExecuteCommand(char *Line) {
  char TmpStr1[80];
  TmpStr1[0] = 0;
  char Command[80];
  Command[0] = 0;
  int Par1 = 0;
  int Par2 = 0;
  GetArgv(Line, Command, 1);
  if (GetArgv(Line, TmpStr1, 2)) Par1 = atof(TmpStr1);
  if (GetArgv(Line, TmpStr1, 3)) Par2 = atof(TmpStr1);

  // ****************************************
  // configure settings commands:
  // ****************************************
#if 0  
  if (strcasecmp(Command, "WifiSSID") == 0)
    strcpy(Settings.WifiSSID, Line+9);

  if (strcasecmp(Command, "WifiKey") == 0)
    strcpy(Settings.WifiKey, Line+8);

  if (strcasecmp(Command, "WifiScan") == 0)
    WifiScan();

  if (strcasecmp(Command, "WifiConnect") == 0)
    WifiConnect();

  if (strcasecmp(Command, "WifiDisconnect") == 0)
    WifiDisconnect();

  if (strcasecmp(Command, "Reboot") == 0)
    ESP.reset();

  if (strcasecmp(Command, "Reset") == 0)
    ResetFactory();

  if (strcasecmp(Command, "Save") == 0)
    Save_Settings();

  if (strcasecmp(Command, "Delay") == 0)
    Settings.Delay = Par1;

  if (strcasecmp(Command, "Debug") == 0)
    Settings.SerialLogLevel = Par1;

  if (strcasecmp(Command, "IP") == 0) {
    if (GetArgv(Line, TmpStr1, 2))
      if (!str2ip(TmpStr1, Settings.IP))
        Serial.println("?");
  }
#endif  

  if (strcasecmp(Command, "?") == 0) {
    char str[20];
    Serial.println();

    Serial.println("System Info");
    IPAddress ip = WiFi.localIP();
    sprintf_P(str, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
    Serial.print("  IP Address    : "); Serial.println(str);
//    Serial.print("  Build         : "); Serial.println((int)BUILD);
    Serial.print("  Name          : "); Serial.println(config.deviceName);
    Serial.print("  WifiSSID      : "); Serial.println(config.ssid);
    Serial.print("  WifiKey       : "); Serial.println(config.wifiPassword);
    Serial.print("  Settings size : "); Serial.println(sizeof(config));
    Serial.print("  Free mem      : "); Serial.println(system_get_free_heap_size());
  }
}


/********************************************************************************************\
* Get data from Serial Interface
\*********************************************************************************************/
#define INPUT_BUFFER_SIZE          128

byte SerialInByte;
int SerialInByteCounter = 0;
char InputBuffer_Serial[INPUT_BUFFER_SIZE + 2];

void serial() {
  while (Serial.available()) {
    yield();
    SerialInByte = Serial.read();

    if (isprint(SerialInByte)) {
      if (SerialInByteCounter < INPUT_BUFFER_SIZE) // add char to string if it still fits
        InputBuffer_Serial[SerialInByteCounter++] = SerialInByte;
    }
    if (SerialInByte == '\r' || SerialInByte == '\n') {
      InputBuffer_Serial[SerialInByteCounter] = 0; // serial data completed
      Serial.write('>');
      Serial.println(InputBuffer_Serial);
      ExecuteCommand(InputBuffer_Serial);
      SerialInByteCounter = 0;
      InputBuffer_Serial[0] = 0; // serial data processed, clear buffer
    }
  }
}

