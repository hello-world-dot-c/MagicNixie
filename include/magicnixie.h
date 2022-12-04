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

#include <Arduino.h>
#include <user_interface.h>
#include <FS.h>
#include <SPI.h>
#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
//#include <Dns.h>
//#include <Ethernet.h>
//#include <WiFiClient.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>


#include "config.h"
#include "tasks.h"
#include "wifi.h"
#include "mqtt.h"
#include "web.h"
#include "timedate.h"
#include "nixiedriver.h"
#include "leds.h"

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

void keypressWait();
boolean isNumeric(String str);
String formatBytes(size_t bytes);


#endif // _MAGICNIXIE_H_
/* EOF */
