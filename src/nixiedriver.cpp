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

#define MODULE "*ND: "

/*
The relationship of digits to the bits in the shift data is a hot mess
with no rhyme nor reason. Therefore, just using a table seems
appropriate.
 */
static const uint8_t digitBitNum[7][10] = {
  { 57, 41, 25,  9, 58, 42, 26, 10, 59, 43 }, /* hours tens 0-9 */
  { 27, 11, 60, 44, 28, 12, 61, 45, 29, 13 }, /* hours singles 0-9 */
  { 62, 46, 30, 14, 63, 47, 31, 15, 64, 48 }, /* minutes tens 0-9 */
  { 49, 33, 17,  1, 50, 34, 18,  2, 51, 35 }, /* minutes singles 0-9 */
  { 19,  3, 52, 36, 20,  4, 53, 37, 21,  5 }, /* seconds tens 0-9 */
  { 54, 38, 22,  6, 55, 39, 23,  7, 56, 40 }, /* seconds singles 0-9 */
  { 32, 16, 24,  8,  0,  0,  0,  0,  0,  0 }  /* columns: lower left, upper left, lower right, upper right */
};

static uint64_t digitMask[8];  // is filled on startup

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
  static uint64_t dmem_save;

  // Update complete shift register, then latch to outputs
  dmem_save = displayMem.lword;  // transfer function destroys the data, so save it here
  SPI.transfer(&displayMem, sizeof(displayMem));
  displayMem.lword = dmem_save;
  digitalWrite(PIN_LE, HIGH);
  digitalWrite(PIN_LE, LOW);
  digitalWrite(PIN_SHDN, HIGH); //HIGH = ON 
}


void nixiePrint(int Pos, String Str) {
  int i, len, digitpos;
  char ch;
  char str[10];

  if (Pos>8) {
    return;
  }

  Str.toCharArray(str, sizeof(str));
  len = strlen(str);
  if (Pos+len > 8) {
    len = 8-Pos;
  }
  for (i=Pos; i<Pos+len; i++) {
    if ((i == 2) || (i == 5)) {
      digitpos = 6;
    } else if (i>5) {
      digitpos = i-2;
    } else if (i>2) {
      digitpos = i-1;
    } else {
      digitpos = i;
    }
    ch = str[i-Pos];
    if (digitpos != 6) {
      displayMem.lword &= digitMask[digitpos];
      // position has a digit
      if ((ch >= '0') && (ch <= '9')) {
        uint8_t digit = ch-'0';
        displayMem.lword |= (1ULL << (digitBitNum[digitpos][digit] -1));
      }
    }
    else {
      displayMem.lword &= digitMask[6 + ((i==5)?1:0)];
      uint64_t bitmask = 0ULL;
      // position has a column character
      if ((ch == ':') || (ch == '.')) { // lower or both dots
        bitmask |= (1ULL << (digitBitNum[digitpos][((i==5)?2:0)] -1));
      }
      if ((ch == ':') || (ch == '\'')) { // upper or both dots
        bitmask |= (1ULL << (digitBitNum[digitpos][1+((i==5)?2:0)] -1));
      }
      displayMem.lword |= bitmask;
    }
  }
  _PF(MODULE"Printed: %s\n", str);
  _PF(MODULE"Display mem: 0x%016llX\n", displayMem.lword);
}

void setupNixie() {
  int i, j;

  displayMem.lword = 0ULL;
  for (i=0; i<6; i++)
  {
    digitMask[i] = (uint64_t)-1;  // all bits to 1
    for (j=0; j<10; j++)
    {
      if (digitBitNum[i][j] > 0)
      {
        digitMask[i] &= ~(1ULL << (digitBitNum[i][j]-1));
      }
    }
    _PF(MODULE"Mask %d: 0x%016llX\n", i, digitMask[i]);
  }
  for (i=0; i<2; i++)
  {
    digitMask[6+i] = (uint64_t)-1;  // all bits to 1
    digitMask[6+i] &= ~(1ULL << (digitBitNum[6][0+2*i]-1));
    digitMask[6+i] &= ~(1ULL << (digitBitNum[6][1+2*i]-1));
    _PF(MODULE"Mask %d: 0x%016llX\n", 6+i, digitMask[6+i]);
  }
  pinMode(PIN_LE, OUTPUT);
  pinMode(PIN_SHDN, OUTPUT);
  digitalWrite(PIN_SHDN, LOW); //HIGH = ON 

  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));

  _PL(MODULE"SPI for nixie shift register initialized");
}

void loopNixie() {
}
/* EOF */
