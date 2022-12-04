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

#ifndef _TIMEDATE_H_
#define _TIMEDATE_H_

#include "magicnixie.h"

typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t day;
  uint8_t month;
  uint16_t year;
} rtcTime_t;

extern bool gTimeDatesetUpdateRtc;
extern Timezone myTZ;

// Task callback methods prototypes
extern void taskSystemTimeUpdate();
extern void taskTimeUpdate();
extern void taskTimeFastUpdate();

// Function prototypes
void setupTime();

#endif // _TIMEDATE_H_
/* EOF */
