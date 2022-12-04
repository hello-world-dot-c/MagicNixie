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
#define LEDS_UPDATE_PERIOD_S 20


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
Adafruit_NeoPixel Leds = Adafruit_NeoPixel(NUMPIXELS, PIN_PWM1, NEO_GRB + NEO_KHZ800);


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
static bool selfTest = false;
static bool endSelfTest = false;


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
  static int tickCnt;
  static bool updateNext = false;
  static uint32_t next_full_second;
  static uint8_t r=0, b=0, g=0, br=0;

  if (t_LedsUpdate.isFirstIteration()) {
    tickCnt = 0;
    selfTest = true;
    _PL(MODULE"Self test started");
    next_full_second = 1;
  }

  tickCnt++;
  if (selfTest) {
    if (tickCnt == LEDS_SECONDS_BY_TICKS(next_full_second)) {
      next_full_second++;
      switch (next_full_second % 3) {
        case 0: setAllLeds(255,0,0); break;
        case 1: setAllLeds(0, 255, 0); break;
        case 2: setAllLeds(0, 0, 255); break;
      }
    }
    if (endSelfTest || next_full_second==12)  { // end self test after 12 seconds latest
      endSelfTest = false;
      selfTest = false;
      updateNext = true;
      next_full_second += LEDS_UPDATE_PERIOD_S;
    }
  }
  else {
    if (br != gConf.ledBrightness) {
      br = gConf.ledBrightness;
      Leds.setBrightness(gConf.ledBrightness);
      updateNext = true;
    }
    if ((r!=gConf.ledRed) || (g!=gConf.ledGreen) || (b!=gConf.ledBlue)) {
      updateNext = true;
    }
    if (tickCnt == LEDS_SECONDS_BY_TICKS(next_full_second)) {
      updateNext = true;
      next_full_second += LEDS_UPDATE_PERIOD_S;
    }
    if (updateNext) {
      updateNext = false;
      r=gConf.ledRed;
      g=gConf.ledGreen;
      b=gConf.ledBlue;
      setAllLeds(r,g,b);
    }
  }
}


void endLedsSelfTest() {
  endSelfTest = true;
}

void setupLeds() {
  Leds.begin(); // This initializes the NeoPixel library.
  Leds.setBrightness(50);
  turnLedsOff();
}

/* EOF */
