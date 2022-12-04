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
#include <TaskScheduler.h>

#define MODULE "*TS: "

/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
// Tasks
Task t_WiFiConnect(5000, TASK_FOREVER, &taskWiFiConnect);
Task t_MqttConnect(5000, TASK_FOREVER, &taskMqttConnect);
Task t_WebConnect(5000, TASK_FOREVER, &taskWebConnect);
Task t_NixieUpdate(1000, TASK_FOREVER, &taskNixieUpdate);
Task t_TimeUpdate(10000, TASK_FOREVER, &taskTimeUpdate);
Task t_LedsUpdate(1000, TASK_FOREVER, &taskLedsUpdate);
Task t_MqttRun(200, TASK_FOREVER, &taskMqttRun);
Task t_WebRun(50, TASK_FOREVER, &taskWebRun);


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
// Scheduler
static Scheduler ts;


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
static void addAndRun(Task taskToRun) {
  ts.addTask(taskToRun);
  taskToRun.enable();
}

/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void setupTasks() {
  ts.init();
  _PL(MODULE"Initialized scheduler");
  ts.addTask(t_WiFiConnect);
  ts.addTask(t_MqttConnect);
  ts.addTask(t_WebConnect);
  ts.addTask(t_NixieUpdate);
  ts.addTask(t_LedsUpdate);
  ts.addTask(t_MqttRun);
  ts.addTask(t_WebRun);
  t_WiFiConnect.enable();
  t_MqttConnect.enable();
  t_NixieUpdate.enable();
  t_LedsUpdate.enable();
}

void loopTasks() {
  ts.execute();
}
/* EOF */
