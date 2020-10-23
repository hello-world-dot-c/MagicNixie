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

/*
  The bits in the 64-bit shift register are allocated to the display function
  as follows:
  Bits   Function
   0- 9  Hours tens 0-9
  10-19  Hours singles 0-9
  20-29  Minutes tens 0-9
  30-31  Column left lower/upper
  32-41  Minutes singles 0-9
  42-51  Seconds tens 0-9
  52-61  Seconds singles 0-9
  62-63  Column right lower/upper
 */
static const uint8_t  startBits[] = {0, 10, 30, 20, 32, 62, 42, 52};
static const uint64_t maskBits[] = {
  0xFFFFFFFFFFFFFC00ULL,
  0xFFFFFFFFFFF003FFULL,
  0xFFFFFFFF3FFFFFFFULL,
  0xFFFFFFFFC00FFFFFULL,
  0xFFFFFC00FFFFFFFFULL,
  0x3FFFFFFFFFFFFFFFULL,
  0xFFF003FFFFFFFFFFULL,
  0xC00FFFFFFFFFFFFFULL
};


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
static char displayStr[] = "00:00:00"; // Content of this string will be displayed on tubes
static union {
  uint8_t  bytes[8];
  uint64_t lword;
} displayMem;


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskNixieUpdate() {
  if (t_NixieUpdate.isFirstIteration()) {
  }

  // Update complete shift register, then latch to outputs
  SPI.transfer(&displayMem, sizeof(displayMem));
  digitalWrite(PIN_LE, HIGH);
  digitalWrite(PIN_LE, LOW);
}


void nixiePrint(int Pos, String Str) {
  int i, len;
  char ch;

  if (Pos>8) {
    return;
  }

  len = Str.length();
  if (Pos+len > 8) {
    len = 8-Pos;
  }
  for (i=Pos; i<Pos+len; i++) {
    displayMem.lword &= maskBits[i];
    ch = Str[i-Pos];
    if ((i != 2) && (i != 5)) {
      // position has a digit
      if ((ch >= '0') && (ch <= '9')) {
        uint8_t digit = ch-'0';
        displayMem.lword |= (1ULL << (startBits[i] + digit));
      }
    }
    else {
      uint64_t bitmask = 0ULL;
      // position has a column character
      if (ch == ':') {
        bitmask = 3ULL; // both bits set
      } else if (ch == '.') {
        bitmask = 1ULL; // lower bit set
      } else if (ch == '\'') {
        bitmask = 2ULL; // upper bit set
      }
      displayMem.lword |= (bitmask << (startBits[i]));
    }
  }
  _PL("Printed: "+Str);
  printf("Display mem: 0x%016llX\n", displayMem.lword);
}

void setupNixie() {
  displayMem.lword = 0ULL;
  pinMode(PIN_LE, OUTPUT);
  pinMode(PIN_SHDN, OUTPUT);
  digitalWrite(PIN_SHDN, HIGH); //HIGH = ON 

  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));

  _PL("SPI for nixie shift register initialized");
}

void loopNixie() {
}
/* EOF */
