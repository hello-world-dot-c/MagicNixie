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

#ifndef _WIFI_H_
#define _WIFI_H_

#include "magicnixie.h"


// WiFi client
extern WiFiClient wclient;

// Task callback methods prototypes
extern void taskWiFiConnect();
extern void taskMqttConnect();
extern void taskMqttRun();

// Function prototypes
void setupWifi();

#endif // _WIFI_H_
/* EOF */
