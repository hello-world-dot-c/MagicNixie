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
DEFINITIONS AND SETTINGS
***************************************************************************/
#define MODULE "*TMP: "
#define CELSIUS 0
#define FAHRENHEIT 1


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire;

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
bool TempPresent = false;
DeviceAddress tempDeviceAddress;


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskTempUpdate() {
  if (TempPresent) {
    if (t_TempUpdate.isFirstIteration()) {
      // Send the command to get temperatures
      sensors.requestTemperatures(); 
      t_TempUpdate.setInterval(1000);
      return;
    }

    t_TempUpdate.setInterval(10000);
    if (TempPresent) {
      String varStr = "TEMP2";
      float temp = sensors.getTempCByIndex(0);
      String valStr = String(temp);
      (void)nvSetThisVarByString(&varStr, &valStr);

      _PF(MODULE"Current temperature is %.1f degC\n", temp);
      // Send the command to get temperatures
      sensors.requestTemperatures(); 
    }
  }
}


void setupTemp() {
  uint8_t num_devices;
  oneWire.begin(PIN_TMP);
  sensors.begin();
  // locate devices on the bus
  num_devices = sensors.getDS18Count();
  _PF(MODULE"Found %d DS18 devices.\n", num_devices);
  TempPresent = (num_devices==1);
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(tempDeviceAddress, 12);
  sensors.setWaitForConversion(false);
}

/* EOF */
