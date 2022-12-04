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
//#include <TimeLib.h>
#include <ezTime.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ESPAsyncUDP.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// Application includes
#include "config.h"
#include "tasks.h"
#include "wifi.h"
#include "mqtt.h"
#include "web.h"
#include "timedate.h"
#include "nixiedriver.h"
#include "leds.h"

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

#define MAGICNIXIE_VERSION "\n\nThis is MagicNixie ver: 2020-10-23 v0.1\n\n" 
#define SYS_TIME_UPD_PERIOD 600  // main system time update handler is executed every 600 ms

boolean isNumeric(String str);


#endif // _MAGICNIXIE_H_
/* EOF */
 