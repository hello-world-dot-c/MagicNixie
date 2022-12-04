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
#define MODULE "*ND: "

// Definitions for blending parameters
#define NIXIE_BLENDS 2

#define NIXIE_BLEND_TICKS_PER_SLICE 1

#define NIXIE_BLEND1_SPEED 144 // 144 ms for blending to the next
#define NIXIE_BLEND1_PHASES 12
#define NIXIE_BLEND1_SLICES 12  // number of slices per phase
#define NIXIE_BLEND1_SLICES_SHIFT 1  // number of slices shift per phase
#define NIXIE_BLEND1_TIME (NIXIE_UPD_PERIOD*NIXIE_BLEND_TICKS_PER_SLICE*NIXIE_BLEND1_PHASES*NIXIE_BLEND1_SLICES)
#if  NIXIE_BLEND1_TIME != NIXIE_BLEND1_SPEED
#error Cannot do the blend with the given settings
#endif
#define NIXIE_BLEND2_SPEED 484 // 484 ms for blending to the next
#define NIXIE_BLEND2_PHASES 22
#define NIXIE_BLEND2_SLICES 22  // number of slices per phase
#define NIXIE_BLEND2_SLICES_SHIFT 1  // number of slices shift per phase
#define NIXIE_BLEND2_TIME (NIXIE_UPD_PERIOD*NIXIE_BLEND_TICKS_PER_SLICE*NIXIE_BLEND2_PHASES*NIXIE_BLEND2_SLICES)
#if  NIXIE_BLEND2_TIME != NIXIE_BLEND2_SPEED
#error Cannot do the blend with the given settings
#endif

/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
/*
The relationship of digits to the bits in the shift data is a hot mess, There
is a symmetry but instead of trying to calculate it, just using a table seems
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

static uint64_t digitMask[8];  // masks to turn off bits for digit - filled on startup

static struct {
  uint64_t lword;
  uint64_t mask;
  uint16_t blend_ctr;
  bool     blend_active;
  uint16_t blend_slices;
  uint16_t blend_slices_shift;
  uint16_t blend_phases; 
} displayMem[NIXIE_BLENDS+1];


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskNixieUpdate() {
  if (t_NixieUpdate.isFirstIteration()) {
  }
  static uint64_t dmem_out;
  static uint8_t blend_ticks = 1;
  uint8_t num_phase;
  uint8_t slice_in_phase;

  // Always start with the last output value (channel 0)
  dmem_out = displayMem[0].lword;
  blend_ticks--;
  if (blend_ticks==0) {
    blend_ticks = NIXIE_BLEND_TICKS_PER_SLICE;
    // Blending algorithm: Apply all blending channels to the output data
    for (int i=1; i<NIXIE_BLENDS+1; i++) {
      if (displayMem[i].blend_active) {
        num_phase = displayMem[i].blend_ctr / displayMem[i].blend_slices;
        slice_in_phase = displayMem[i].blend_ctr % displayMem[i].blend_slices;
        // Slicing-in of the new display content. From each phase during the blending cycle to the next,
        // it happens at an earlier and earlier slice. This leads to a PWM signal for the affected outputs
        // with a decreasing duty cycle for the old output and an increasing duty cycle for the new, which
        // controls the brightness for the linked digits. Thereby, we can have "soft" transitions.
        if (slice_in_phase >= (displayMem[i].blend_slices-displayMem[i].blend_slices_shift*num_phase)) {
          dmem_out &= ~displayMem[i].mask;
          dmem_out |= (displayMem[i].lword & displayMem[i].mask);
        }
        displayMem[i].blend_ctr++;
        if (displayMem[i].blend_ctr==displayMem[i].blend_phases*displayMem[i].blend_slices) {
          displayMem[0].lword = dmem_out;  // copy new to old
          displayMem[i].blend_active = false; // end blending for this channel
        }
      }
    }
  }

  // Update complete shift register, then latch to outputs
  SPI.transfer(&dmem_out, sizeof(dmem_out));
  digitalWrite(PIN_LE, HIGH);
  digitalWrite(PIN_LE, LOW);
  digitalWrite(PIN_SHDN, HIGH); // Turn on nixie driver
}


void nixiePrint(int Pos, char *Str, uint8_t blending) {
  int i, len, digitpos;
  char ch;
  char str[10];

  if ((Pos>8) || (blending>NIXIE_BLENDS)) {
    return;
  }

  strncpy(str, Str, sizeof(str));
  len = strlen(str);
  if (Pos+len > 8) {
    len = 8-Pos;
  }
  displayMem[blending].mask = 0ULL; // all bits 0
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
      displayMem[blending].lword &= digitMask[digitpos];
      // position has a digit
      if ((ch >= '0') && (ch <= '9')) {
        uint8_t digit = ch-'0';
        displayMem[blending].mask |= ~digitMask[digitpos]; // set relevant bits to 1
        displayMem[blending].lword |= (1ULL << (digitBitNum[digitpos][digit] -1));
      }
    } else {
      bool set = false;
      uint64_t bitmask_set = 0ULL;
      uint8_t index = (i==5) ? 2 : 0;
      uint64_t bitmask_clear = digitMask[6 + ((i==5)?1:0)];
      displayMem[blending].lword &= bitmask_clear;
      // position has a column character
      if ((ch == ':') || (ch == '.')) { // lower or both dots
        bitmask_set |= (1ULL << (digitBitNum[digitpos][index] -1));
        set = true;
      }
      if ((ch == ':') || (ch == '\'')) { // upper or both dots
        bitmask_set |= (1ULL << (digitBitNum[digitpos][index+1] -1));
        set = true;
      }
      if (ch == ' ') { // space clears the position
        set = true;
      }
      if (set) {
        displayMem[blending].mask |= ~bitmask_clear;
        displayMem[blending].lword |= bitmask_set;
      }
    }
  }
  // Start blending if printed to channels >0
  if (blending > 0) {
    displayMem[blending].blend_ctr = 0;
    displayMem[blending].blend_active = true;
  }
}

void setupNixie() {
  // Set up SPI transmission and turn off all digits
  pinMode(PIN_LE, OUTPUT);
  pinMode(PIN_SHDN, OUTPUT);
  digitalWrite(PIN_SHDN, LOW); // HIGH = ON 
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));

  memset(displayMem, 0, sizeof(displayMem));
  // Set up blend parameters
#if NIXIE_BLENDS>=1
  displayMem[1].blend_phases = NIXIE_BLEND1_PHASES;
  displayMem[1].blend_slices = NIXIE_BLEND1_SLICES;
  displayMem[1].blend_slices_shift = NIXIE_BLEND1_SLICES_SHIFT;
#endif
#if NIXIE_BLENDS>=2
  displayMem[2].blend_phases = NIXIE_BLEND2_PHASES;
  displayMem[2].blend_slices = NIXIE_BLEND2_SLICES;
  displayMem[2].blend_slices_shift = NIXIE_BLEND2_SLICES_SHIFT;
#endif
#if NIXIE_BLENDS>=3
#error No initialization for blending pattern
#endif

  // Set up the mask values to clear full digits 
  for (int i=0; i<6; i++) {
    digitMask[i] = (uint64_t)-1;  // all bits to 1
    for (int j=0; j<10; j++) {
      if (digitBitNum[i][j] > 0) {
        digitMask[i] &= ~(1ULL << (digitBitNum[i][j]-1));
      }
    }
  }
  for (int i=0; i<2; i++) {
    digitMask[6+i] = (uint64_t)-1;  // all bits to 1
    digitMask[6+i] &= ~(1ULL << (digitBitNum[6][0+2*i]-1));
    digitMask[6+i] &= ~(1ULL << (digitBitNum[6][1+2*i]-1));
  }
}

void loopNixie() {
}
/* EOF */
