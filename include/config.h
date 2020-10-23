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

#ifndef _CONFIG_H_
#define _CONFIG_H_

// Name of AP when in WifiManager config mode
#define WIFIAPNAME "MagicNixieConfigAP"

// MQTT settings
#define MQTT_SERVER "homeslave.fritz.box"
#define MQTT_SERVER_PORT 1883
#define MQTT_CLIENT "nixie-clock"
#define MQTT_MYNAME "magicnixie"
#define MQTT_OUT_TOPIC "tele/" MQTT_MYNAME "/STATE"
#define MQTT_IN_TOPIC "cmnd/" MQTT_MYNAME "/#"

// Debug and Test options
#define _DEBUG_
//#define _TEST_

#define PIN_LE   15  // LE signal for nixie driver
#define PIN_PWM1  2  // serial LED signal in v3 hardware
#define PIN_SHDN  0  // shutdown signal for nixie driver
#define PIN_BUZZ 16  // buzzer signal
// the pins for SPI and I2C are fixed

#endif // _CONFIG_H_
/* EOF */
