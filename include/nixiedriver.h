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

#ifndef _NIXIEDRIVER_H_
#define _NIXIEDRIVER_H_

#include <SPI.h>

extern SPIClass SPI;

// Task callback methods prototypes
void taskNixieFastUpdate();
void taskNixieSlowUpdate();

// Function prototypes
void nixiePrint(int Pos, char *Str, uint8_t blending);
void nixieFade(bool fade_in, uint16_t speed_ms, uint16_t pause_ms);
bool nixieFadeFinished();
void setupNixie();

#endif // _NIXIEDRIVER_H_
/* EOF */
