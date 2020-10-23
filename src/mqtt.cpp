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

  _PP("Message arrived [");
  _PP(topic);
  _PL("]: ---");
  for (unsigned int i=0;i<length;i++) {
    _PP((char)payload[i]);
  }
  _PL("---");
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    _PL("JSON parsing failed");
  }
  else {
    _PL("JSON: ---");
    serializeJsonPretty(doc, Serial);
    _PL();
    _PL("---");
  }
}

static IPAddress applyMqttServerIP()
{
  IPAddress IP;

  WiFi.hostByName(MQTT_SERVER,IP);
  _PP("Address of MQTT server ");
  _PP(MQTT_SERVER);
  _PL(" is " + IP.toString());

  mqttClient.setServer(IP, MQTT_SERVER_PORT);

  return IP;
}


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskMqttConnect() {
  static IPAddress IP;
  static boolean connected = false;

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
    _PL("MQTT connection lost, reconnecting...")
    if (IP.toString() == "255.255.255.255")
    {
      IP = applyMqttServerIP();
    }
    // Connect or check if we're connected
    if (mqttClient.connect(MQTT_CLIENT)) {
      _PL("MQTT connection established to server at " + IP.toString());
      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_OUT_TOPIC,"hello world");
      // ... and resubscribe
      _PP("MQTT subscribing to ");
      _PL(MQTT_IN_TOPIC);
      mqttClient.subscribe(MQTT_IN_TOPIC);
      connected = true;
    } else {
      _PP("MQTT connection failed, rc=");
      _PP(mqttClient.state());
      _PL(" try again later");
      connected = false;
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
