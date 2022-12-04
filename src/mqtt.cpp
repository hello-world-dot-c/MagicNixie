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
  StaticJsonDocument<256> doc;

  _PP(MODULE"Message arrived [");
  _PP(topic);
  _PL("]: ---");
  for (unsigned int i=0;i<length;i++) {
    _PF("%c", (char)payload[i]);
  }
  _PL("---");
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    _PL(MODULE"JSON parsing failed");
  }
  else {
    _PL(MODULE"JSON: ---");
    serializeJsonPretty(doc, Serial);
    _PL("");
    _PL("---");
  }
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
    _PL(MODULE"MQTT connection lost, reconnecting...");
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
      _PP(MODULE"MQTT subscribing to ");
      _PL(MQTT_IN_TOPIC);
      mqttClient.subscribe(MQTT_IN_TOPIC);
      connected = true;

      t_MqttRun.enable();
    } else {
      _PP(MODULE"MQTT connection failed, rc=");
      _PP(mqttClient.state());
      _PL(" try again later");
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
