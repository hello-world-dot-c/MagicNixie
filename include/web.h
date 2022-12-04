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

#ifndef _WEB_H_
#define _WEB_H_

#include "magicnixie.h"


/**************************************************************************
DEFINITIONS AND SETTINGS
***************************************************************************/


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
// Task callback methods prototypes
extern void taskWebConnect();
extern void taskWebRun();

// Function prototypes
void setupWeb();

#endif // _MQTT_H_
/* EOF */
