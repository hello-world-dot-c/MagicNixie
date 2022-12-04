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

#define MODULE "*WF: "


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
// WiFi and MQTT clients
WiFiClient wclient;


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
static void configModeCallback (WiFiManager *myWiFiManager) {
  _PL(MODULE"Entered config mode");
  _PP(MODULE);
  _PL(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  _PL(myWiFiManager->getConfigPortalSSID());
}


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskWiFiConnect() {
  static boolean connected = false;

  if (t_WiFiConnect.isFirstIteration()) {
  }
    
 if ((WiFi.status() != WL_CONNECTED) || (WiFi.localIP().toString() == "0.0.0.0")) {
    _PL(MODULE"WiFi connection lost, reconnecting...");
    connected = false;
  }
  else if (!connected)
  {
    _PL(MODULE"WiFi connection established, using " + WiFi.localIP().toString());
    connected = true;
    t_MqttConnect.enable();
    t_WebConnect.enable();
  }
}


void setupWifi() {
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
//  wifiManager.resetSettings();

  //wifi_status_led_install(2, PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2); 

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(WIFIAPNAME)) {
    _PL(MODULE"failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
//    ESP.reset();
//    delay(1000);
  } 

  //if you get here you have connected to the WiFi
  _PL(MODULE"Connected to SSID " + WiFi.SSID() + ", own IP is " + WiFi.localIP().toString());
}

void loopWifi() {
}
/* EOF */
