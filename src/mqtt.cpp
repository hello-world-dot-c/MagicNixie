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

#define MODULE "*MQ: "

/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
// MQTT client
PubSubClient mqttClient(wclient);


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
static void receiveMQTTCallback (char* topic, byte* payload, unsigned int length) {
#ifdef USE_MQTT_JSON
  StaticJsonDocument<256> doc;
#endif
  byte payload_save[100];
  memcpy(payload_save,payload,length);
  _PF(MODULE"Message arrived [%s][%d]: [", topic, length);
  memcpy(payload,payload_save,length);
  for (unsigned int i=0;i<length;i++) {
    _PF("%c", (char)payload[i]);
  }
  _PF("]\n");
#ifdef USE_MQTT_JSON
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    _PF(MODULE"JSON parsing failed\n");
  }
  else {
    _PF(MODULE"JSON: ---\n");
    serializeJsonPretty(doc, Serial);
    _PF("\n");
    _PF("---\n");
  }
#endif
}

static IPAddress applyMqttServerIP()
{
  IPAddress IP;
  char str[20];

  WiFi.hostByName(MQTT_SERVER,IP);
  IP.toString().toCharArray(str,sizeof(str));
  _PF(MODULE"Address of MQTT server %s is %s\n", MQTT_SERVER, str);
  mqttClient.setServer(IP, MQTT_SERVER_PORT);

  return IP;
}


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void mqttLogPrint(char * outstr) {
  if (mqttClient.connected()) {
    mqttClient.publish(MQTT_LOG_TOPIC,outstr);
  }  
}

void taskMqttConnect() {
  static IPAddress IP;
  static boolean connected = false;
  char str[20];

  if (t_MqttConnect.isFirstIteration()) {
  }
    
  // Don't do anything unless we are connected to WiFi
  if ((WiFi.status() != WL_CONNECTED) || (WiFi.localIP().toString() == "0.0.0.0"))
    return;

  if (t_MqttConnect.isFirstIteration()) {
    IP = applyMqttServerIP();
    mqttClient.setCallback(receiveMQTTCallback);
  }
    
  if (!mqttClient.connected()) {
    _PF(MODULE"MQTT connection lost, reconnecting...\n");
    if (IP.toString() == "255.255.255.255")
    {
      IP = applyMqttServerIP();
    }
    // Connect or check if we're connected
    if (mqttClient.connect(MQTT_CLIENT)) {
      IP.toString().toCharArray(str,sizeof(str));
      _PF(MODULE"MQTT connection established to server at %s\n", str);
      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_OUT_TOPIC,"hello world");
      // ... and resubscribe
      _PF(MODULE"MQTT subscribing to ");
      _PF(MQTT_IN_TOPIC"\n");
      mqttClient.subscribe(MQTT_IN_TOPIC);
      connected = true;

      t_MqttRun.enable();
    } else {
      _PF(MODULE"MQTT connection failed, rc=%d, try again later\n",mqttClient.state());
      connected = false;

      t_MqttRun.disable();
    }
  }
  else if (!connected)
  {
    connected = true;
  }
}

void taskMqttRun() {

  if (mqttClient.connected()) {
    mqttClient.loop();
  }
}

void setupMqtt() {
}

void loopMqtt() {
  if (mqttClient.connected()) {
    // Client connected
  }
}
/* EOF */
