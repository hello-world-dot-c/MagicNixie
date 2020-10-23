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


void setup() {
  // Running at 160 MHz for snappier performance.
  //system_update_cpu_freq(160);
  // Init UART for debugging output.
  Serial.begin(115200);
  Serial.printf("CPU frequency: %d\n", system_get_cpu_freq());

  if (LittleFS.begin()) {  
    LittleFS.format();
    
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      Serial.print(dir.fileName());
      File f = dir.openFile("r");
      Serial.println(f.size());
    }
  }
  else
  {
    _PL("Error mounting file system");
  }
  
  setupWifi();
  setupMqtt();
  setupNixie();
  setupTasks();

  nixiePrint(0, "01:23'45");
  testLeds();
}

void loop() {
  loopTasks();
}
/* EOF */
