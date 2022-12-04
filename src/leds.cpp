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
#define MODULE "*LED: "

#define NUMPIXELS      8

#define LEDS_MILLISECONDS_BY_TICKS(time_ms)  (((time_ms) + LED_UPD_PERIOD - 1) / LED_UPD_PERIOD)
#define LEDS_SECONDS_BY_TICKS(time_s) LEDS_MILLISECONDS_BY_TICKS(1000*(time_s))


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
Adafruit_NeoPixel Leds = Adafruit_NeoPixel(NUMPIXELS, PIN_PWM1, NEO_GRB + NEO_KHZ800);


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
static bool selfTest = false;
static int selfTestCnt;


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
void turnLedsOff()
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    Leds.setPixelColor(i, Leds.Color(0, 0, 0)); 
  }
  Leds.show();
}

void setAllLeds(uint8_t r, uint8_t g, uint8_t b)
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    Leds.setPixelColor(i, Leds.Color(r, g, b)); 
  }
  Leds.show(); // This sends the updated pixel color to the hardware.
}


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskLedsUpdate() {
  if (t_LedsUpdate.isFirstIteration()) {
    selfTestCnt = 0;
    selfTest = true;
    _PL(MODULE"Self test started");
  }

  if (selfTest) {
    selfTestCnt++;
    if (selfTestCnt == LEDS_SECONDS_BY_TICKS(1)) {
      setAllLeds(255,0,0);
    }
    else if (selfTestCnt == LEDS_SECONDS_BY_TICKS(2)) {
      setAllLeds(0,255,0);
    }
    else if (selfTestCnt == LEDS_SECONDS_BY_TICKS(3)) {
      setAllLeds(0,0,255);
    }
    else if (selfTestCnt == LEDS_SECONDS_BY_TICKS(4)) {
      turnLedsOff();
      selfTest = false;
    }
  }
}


void setupLeds() {
  Leds.begin(); // This initializes the NeoPixel library.
  Leds.setBrightness(50);
  turnLedsOff();
}

/* EOF */
