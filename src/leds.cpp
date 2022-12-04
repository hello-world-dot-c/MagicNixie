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

#define MODULE "*LED: "

#define NUMPIXELS      8
#define LEDsSpeed      10
const int LEDsDelay=40;

/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
Adafruit_NeoPixel Leds = Adafruit_NeoPixel(NUMPIXELS, PIN_PWM1, NEO_GRB + NEO_KHZ800);


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskLedsUpdate() {
  if (t_LedsUpdate.isFirstIteration()) {
  }

}


void setupLeds() {
  Leds.begin(); // This initializes the NeoPixel library.
  Leds.setBrightness(50);
  turnLedsOff();
}


void turnLedsOff()
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    Leds.setPixelColor(i, Leds.Color(0, 0, 0)); 
  }
  Leds.show();
}


void testLeds()
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    Leds.setPixelColor(i, Leds.Color(255, 0, 0)); 
  }
  Leds.show(); // This sends the updated pixel color to the hardware.
  delay(1000);
  for(int i=0;i<NUMPIXELS;i++)
  {
    Leds.setPixelColor(i, Leds.Color(0, 255, 0)); 
  }
  Leds.show(); // This sends the updated pixel color to the hardware.
  delay(1000);
  for(int i=0;i<NUMPIXELS;i++)
  {
    Leds.setPixelColor(i, Leds.Color(0, 0, 255)); 
  }
  Leds.show(); // This sends the updated pixel color to the hardware.
  delay(1000);
  turnLedsOff();
}

void loopLeds() {
}
/* EOF */
