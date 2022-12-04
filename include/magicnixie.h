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

#ifndef _MAGICNIXIE_H_
#define _MAGICNIXIE_H_

// System and library includes
#include <Arduino.h>
#include <user_interface.h>
#include <FS.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <ezTime.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "EasyBuzzer.h"

// Application includes
#include "config.h"
#include "tasks.h"
#include "wifi.h"
#include "mqtt.h"
#include "web.h"
#include "timedate.h"
#include "nixiedriver.h"
#include "leds.h"
#include "tone.h"
#include "sound.h"

// Debug output macros
#ifdef _DEBUG_
#define _PF(f_, ...) Serial.printf((f_), ##__VA_ARGS__)
#define _PP(a) Serial.print(a)
#define _PL(a) Serial.println(a)
#else
#define _PF(f_, ...)
#define _PP(a)
#define _PL(a)
#endif

/**************************************************************************
DEFINITIONS AND SETTINGS
***************************************************************************/
#define SYS_TIME_UPD_PERIOD 600  // main system time update handler is executed every 600 ms
#define TIME_UPD_PERIOD 20  // Medium-fast time related functions updated every 20 ms
#define ND_FAST_UPD_PERIOD 1  // Nixies updated every 1 ms
#define ND_SLOW_UPD_PERIOD 50  // Nixie driver slow update every 50 ms
#define LED_UPD_PERIOD 100  // LEDs updated every 100 ms
#define SOUND_UPD_PERIOD 20  // Sound updated every 20 ms

// EEPROM locations
#define NVOL_SIZE 4096
#define NVOL_START_SETTINGS 0
#define NVOL_START_TZ  (NVOL_START_SETTINGS+40)  // length is EEPROM_CACHE_LEN


typedef enum {
  SHOW_TIME,
  SHOW_DATE,
  SHOW_TEMP1,
  SHOW_TEMP2
} gShowContent_t;

typedef struct {
  bool     use12hDisplay;
  bool     omitLeading0Hour;  
  bool     useSoftBlend;
  bool     syncRTC;
  uint8_t  nixieBrightness;
  uint16_t altDisplayPeriod_s;
  uint16_t altDisplayDuration_ms;  
  uint16_t altFadeSpeed_ms;
  uint16_t altFadeDarkPause_ms;
  uint8_t  antiPoisoningLevel;
  uint8_t  ledRed;
  uint8_t  ledGreen; 
  uint8_t  ledBlue; 
  uint8_t  ledBrightness;
} gConf_t;

extern gConf_t gConf;


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void keypressWait();
boolean isNumeric(String str);
String formatBytes(size_t bytes);
uint8_t calculateCRC8(uint8_t data,bool reset);
void saveConfig();
bool readConfig();

#endif // _MAGICNIXIE_H_
/* EOF */
 