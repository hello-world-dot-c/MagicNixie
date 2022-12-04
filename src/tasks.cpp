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

/**************************************************************************
DEFINITIONS AND SETTINGS
***************************************************************************/
#define MODULE "*TS: "
#define CPU_LOAD_UPD_PERIOD 1000  // update period for CPU calculation in ms
void taskTasksMonitor();


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
// Tasks
Task t_WiFiConnect(5000, TASK_FOREVER, &taskWiFiConnect);
Task t_MqttConnect(5000, TASK_FOREVER, &taskMqttConnect);
Task t_WebConnect(5000, TASK_FOREVER, &taskWebConnect);
Task t_NixieFastUpdate(ND_FAST_UPD_PERIOD, TASK_FOREVER, &taskNixieFastUpdate);
Task t_NixieSlowUpdate(ND_SLOW_UPD_PERIOD, TASK_FOREVER, &taskNixieSlowUpdate);
Task t_TimeUpdate(TIME_UPD_PERIOD, TASK_FOREVER, &taskTimeUpdate);
Task t_TimeFastUpdate(1, TASK_FOREVER, &taskTimeFastUpdate);
Task t_SystemTimeUpdate(SYS_TIME_UPD_PERIOD, TASK_FOREVER, &taskSystemTimeUpdate);
Task t_LedsUpdate(LED_UPD_PERIOD, TASK_FOREVER, &taskLedsUpdate);
Task t_SoundUpdate(SOUND_UPD_PERIOD, TASK_FOREVER, &taskSoundUpdate);
Task t_MqttRun(200, TASK_FOREVER, &taskMqttRun);
Task t_WebRun(50, TASK_FOREVER, &taskWebRun);


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
// Scheduler
static Scheduler ts;
// Tasks
Task t_TasksMonitor(CPU_LOAD_UPD_PERIOD, TASK_FOREVER, &taskTasksMonitor);


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
static void addAndRun(Task *taskToRun) {
  ts.addTask(*taskToRun);
  taskToRun->enable();
}

void taskTasksMonitor() {
  if (t_TasksMonitor.isFirstIteration()) {
    _PF(MODULE"Starting CPU measurement\n");
    ts.cpuLoadReset();
    return;
  }

  double cpuLoad = 100.0d - ((double)ts.getCpuLoadTotal() / (CPU_LOAD_UPD_PERIOD*10));
  _PF(MODULE"CPU load: %0.2f\n", fabs(cpuLoad));
  ts.cpuLoadReset();
  return;
}


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void setupTasks() {
  ts.init();
  _PF(MODULE"Initialized scheduler\n");

  // Define tasks that are run later
  ts.addTask(t_WebConnect);
  ts.addTask(t_MqttRun);
  ts.addTask(t_WebRun);
  // Set up tasks that we need to run right away
//  addAndRun(&t_TasksMonitor);
  addAndRun(&t_LedsUpdate);
  addAndRun(&t_SoundUpdate);
  addAndRun(&t_TimeUpdate);
  addAndRun(&t_WiFiConnect);
  addAndRun(&t_MqttConnect);
  addAndRun(&t_NixieSlowUpdate);
  addAndRun(&t_NixieFastUpdate);
  addAndRun(&t_SystemTimeUpdate);
//  addAndRun(&t_TimeFastUpdate);
  ts.addTask(t_TimeFastUpdate); // Start only when self test has finished in nixie driver
//  ts.addTask(t_SystemTimeUpdate);
//  t_SystemTimeUpdate.enableDelayed(2500);
}

void loopTasks() {
  ts.execute();
}
/* EOF */
