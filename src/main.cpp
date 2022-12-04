/* 
 * This file is part of the MagicNixie project (https://github.com/hello-world-dot-c/MagicNixie).
 * Copyright (c) 2020 Chris Keydel.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "magicnixie.h"

#define MODULE "*M: "

/*---------------------------------------------------------------*/
/* Utility function                                              */
/* Halt the program and wait for "key press" at the serial port. */
/* For DEBUG purpose only                                        */
/*---------------------------------------------------------------*/
void keypressWait()
{
  Serial.println("** Hit a key to continue");
  while(Serial.available() == 0){}
  while(Serial.available()){Serial.read();}
}

/*---------------------------------------------------------------*/
/* Checks if a string is numeric.                                */
/* Removed check for decimal point as it was not needed          */
/* Original code found here:                                     */
/*     http://tripsintech.com/arduino-isnumeric-function/        */
/*---------------------------------------------------------------*/
boolean isNumeric(String str) 
{
  unsigned int stringLength = str.length();
 
  if (stringLength == 0) return false;
 
  for(unsigned int i = 0; i < stringLength; ++i) 
  {
     if (isDigit(str.charAt(i))) 
     {
       continue;
     }
     return false;
  }
  return true;
}

/*--------------------------------------------------------------------*/
/*  Utility function                                                  */
/*  Original code found here:                                         */
/*  https://tttapa.github.io/ESP8266/Chap16%20-%20Data%20Logging.html */
/*------------------------------------------------------------------- */
String formatBytes(size_t bytes)  // convert sizes in bytes to KB and MB
{
  if (bytes < 1024) 
  {
    return String(bytes) + "B";
  } 
  else if (bytes < (1024 * 1024)) 
  {
    return String(bytes / 1024.0) + "KB";
  } 
  else if (bytes < (1024 * 1024 * 1024)) 
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  return "";
}

void setup() {
  // Running at 160 MHz for snappier performance.
  //system_update_cpu_freq(160);
  // Init UART for debugging output.
  Serial.begin(115200);
  _PL(MAGICNIXIE_VERSION);
  _PF(MODULE"CPU frequency: %d MHz\n", system_get_cpu_freq());

  if (SPIFFS.begin()) {  
//    SPIFFS.format();    
    // Start the SPIFFS and list all contents
    _PP(MODULE"** Starting SPIFFS.");
    SPIFFS.begin();

    _PL(" [OK]");
    _PL(MODULE"** Contents:");
    {
      Dir dir = SPIFFS.openDir("/");
      while (dir.next())                       // List the file system contents
      {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        _PF("\t%s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
      _PL("");
    }
  }
  else
  {
    _PL(MODULE"Error mounting file system");
  }
  
  setupNixie();
  setupLeds();
  setupWifi();
  setupMqtt();
  setupWeb();
  setupTasks();

  nixiePrint(0, "01:23'45");
  testLeds();
}

void loop() {
  loopTasks();
}
/* EOF */
