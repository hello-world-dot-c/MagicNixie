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

/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
gConf_t gConf;


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
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

#define MAX_STR 120
void logPrintf(String fmt, ...)
{
  static char outstrln[MAX_STR+1] = { '\0' };
  char outstr[MAX_STR+1];
  char fmtstr[MAX_STR+1];
	va_list argp;
  char *outstrptr;
  int len;
  bool println;
  bool printov;

  fmt.toCharArray(fmtstr,MAX_STR);
	va_start(argp, fmt);
	(void)vsnprintf(outstr, MAX_STR, fmtstr, argp);
	va_end(argp);
  outstrptr = outstr;
  do {
    println = false;
    printov = false;
    char *ptr = strstr(outstrptr, "\n");
    if (ptr != NULL) {
      len = (ptr-outstrptr);
      println = true;  // print when there is a new line
    } else {
      len = strlen(outstrptr);
    }
    if (strlen(outstrln)+len > MAX_STR) {
      len = MAX_STR-strlen(outstrln);
      printov = true;  // print when the line becomes too long
    }
    strncat(outstrln, outstrptr, len);
    if (println || printov) {
      Serial.printf("%s\n",outstrln);
      mqttLogPrint(outstrln);
      outstrptr += len;
      if (println)
        outstrptr += strlen("\n");
      *outstrln = '\0'; // clear line buffer
    }
  } while (println || printov);
}


/*--------------------------------------------------------------------*/
/*  Utility function                                                  */
/*  Calculates the CRC8 checksum for a byte stream.                   */
/*  Returns urrent CRC8 checksum.                                     */
/*------------------------------------------------------------------- */
uint8_t calculateCRC8 (
  uint8_t data,  // New data byte for CRC8 calculation
  bool reset     // if true, reset algorithm to start new CRC, data is ignored
  )
{
  static uint8_t crc = 0;
  uint8_t i        = 0x00U;
  uint8_t feedback = 0x00U;
  uint8_t temp     = 0x00U;

  if (reset) {
    crc = 0;
    return 0;
  }

  // Calculate CRC-8
  for (i = 0; i < 8; i++) {
    feedback  = (crc & 0x01U);
    crc     >>= 1;
    temp      = feedback ^ (data & 0x01U);
    if ( temp != 0x00U ) {
      crc ^= 0x9C;  // Preferred polynomial
    }
    data >>= 1;
  }
  return (crc);
}

void saveConfig() {
  uint8_t *datPtr;
  uint16_t i;
  uint8_t crc;

  calculateCRC8(0,true);
    // Save present configuration in EEPROM.
  EEPROM.begin(NVOL_SIZE); // Needed in order to start read or write to flash.
  datPtr = (uint8_t *)&gConf;
  _PF(MODULE"Writing EEPROM: ");
  for (i=0; i<sizeof(gConf_t); i++) {
    crc = calculateCRC8(*datPtr,false);
    EEPROM.write(NVOL_START_SETTINGS+i, *datPtr);
    _PF("%02X ", *datPtr);
    datPtr++;
  }
  EEPROM.write(NVOL_START_SETTINGS+i, crc);
  _PF("%02X\n", crc);
  EEPROM.commit(); // Needed in order to save content to flash.
}

bool readConfig() {
  uint8_t *datPtr;
  uint16_t i;
  uint8_t rcrc;
  uint8_t ccrc;
  gConf_t tmp_conf;

  calculateCRC8(0,true);
    // Save present configuration in EEPROM.
  EEPROM.begin(NVOL_SIZE); // Needed in order to start read or write to flash.
  datPtr = (uint8_t *)&tmp_conf;
  _PF(MODULE"Reading EEPROM: ");
  for (i=0; i<sizeof(gConf_t); i++) {
    *datPtr = EEPROM.read(NVOL_START_SETTINGS+i);
    _PF("%02X ", *datPtr);
    ccrc = calculateCRC8(*datPtr,false);
    datPtr++;
  }
  rcrc = EEPROM.read(NVOL_START_SETTINGS+i);
  _PF("%02X\n", rcrc);
  if (ccrc == rcrc) {
    memcpy(&gConf, &tmp_conf, sizeof(gConf_t));
    _PF(MODULE"Configuration settings restored from EEPROM\n");
    return true;
  } else {
    _PF(MODULE"Configuration settings not restored from EEPROM, CRC mismatch\n");
    return false;
  }
}


void setup() {
  // Initialize critical hardware first
  setupLeds();
  setupNixie();

  // Init UART for debugging output.
  Serial.begin(921600);
  Serial.flush();
  while(!Serial){} // Waiting for serial connection

  delay(1000);
  _PF(MAGICNIXIE_VERSION"\n");
  _PF(MODULE"CPU frequency: %d MHz\n", system_get_cpu_freq());

  if (!readConfig()) {
    memset(&gConf, 0, sizeof(gConf_t));
    gConf.use12hDisplay = false;
    gConf.omitLeading0Hour = false;
    gConf.useSoftBlend = true;
    gConf.syncRTC = true;
    gConf.nixieBrightness = 100;
    gConf.altDisplayPeriod_s = 45;
    gConf.altDisplayDuration_ms = 5000; 
    gConf.altFadeSpeed_ms = 800;
    gConf.altFadeDarkPause_ms = 400;
    gConf.antiPoisoningLevel = 2;
    gConf.ledRed = 30;
    gConf.ledGreen = 220; 
    gConf.ledBlue = 60; 
    gConf.ledBrightness = 50;
  }
  
  if (SPIFFS.begin()) {  
//    SPIFFS.format();    
    // Start the SPIFFS and list all contents
    _PF(MODULE"** Starting SPIFFS [OK]\n");
    _PF(MODULE"** Contents:\n");
    {
      Dir dir = SPIFFS.openDir("/");
      while (dir.next())                       // List the file system contents
      {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        _PF("\t%s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
      _PF("\n");
    }
  }
  else
  {
    _PF(MODULE"Error mounting file system\n");
  }
  
  setupTime();
  setupWifi();
  setupMqtt();
  setupWeb();
  setupTasks();
}

void loop() {
  loopTasks();
}
/* EOF */
