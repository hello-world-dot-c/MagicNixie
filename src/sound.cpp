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
#define MODULE "*SND: "

#define SND_MILLISECONDS_BY_TICKS(time_ms)  (((time_ms) + SOUND_UPD_PERIOD - 1) / SOUND_UPD_PERIOD)
#define SND_SECONDS_BY_TICKS(time_s) SND_MILLISECONDS_BY_TICKS(1000*(time_s))


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
uint32_t notes[] = { 0,
                NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
                NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
                NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
                NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
              };
const char song0[]="MissionImp:d=16,o=6,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";
const char song1[]="PinkPanther:d=4,o=5,b=160:8d#,8e,2p,8f#,8g,2p,8d#,8e,16p,8f#,8g,16p,8c6,8b,16p,8d#,8e,16p,8b,2a#,2p,16a,16g,16e,16d,2e";
const char song2[]="VanessaMae:d=4,o=6,b=70:32c7,32b,16c7,32g,32p,32g,32p,32d#,32p,32d#,32p,32c,32p,32c,32p,32c7,32b,16c7,32g#,32p,32g#,32p,32f,32p,16f,32c,32p,32c,32p,32c7,32b,16c7,32g,32p,32g,32p,32d#,32p,32d#,32p,32c,32p,32c,32p,32g,32f,32d#,32d,32c,32d,32d#,32c,32d#,32f,16g,8p,16d7,32c7,32d7,32a#,32d7,32a,32d7,32g,32d7,32d7,32p,32d7,32p,32d7,32p,16d7,32c7,32d7,32a#,32d7,32a,32d7,32g,32d7,32d7,32p,32d7,32p,32d7,32p,32g,32f,32d#,32d,32c,32d,32d#,32c,32d#,32f,16c";
const char song3[]="DasBoot:d=4,o=5,b=100:d#.4,8d4,8c4,8d4,8d#4,8g4,a#.4,8a4,8g4,8a4,8a#4,8d,2f.,p,f.4,8e4,8d4,8e4,8f4,8a4,c.,8b4,8a4,8b4,8c,8e,2g.,2p";
const char song4[]="Scatman:d=4,o=5,b=200:8b,16b,32p,8b,16b,32p,8b,2d6,16p,16c#.6,16p.,8d6,16p,16c#6,8b,16p,8f#,2p.,16c#6,8p,16d.6,16p.,16c#6,16b,8p,8f#,2p,32p,2d6,16p,16c#6,8p,16d.6,16p.,16c#6,16a.,16p.,8e,2p.,16c#6,8p,16d.6,16p.,16c#6,16b,8p,8b,16b,32p,8b,16b,32p,8b,2d6,16p,16c#.6,16p.,8d6,16p,16c#6,8b,16p,8f#,2p.,16c#6,8p,16d.6,16p.,16c#6,16b,8p,8f#,2p,32p,2d6,16p,16c#6,8p,16d.6,16p.,16c#6,16a.,16p.,8e,2p.,16c#6,8p,16d.6,16p.,16c#6,16a,8p,8e,2p,32p,16f#.6,16p.,16b.,16p.";
const char song5[]="Popcorn:d=4,o=5,b=160:8c6,8a#,8c6,8g,8d#,8g,c,8c6,8a#,8c6,8g,8d#,8g,c,8c6,8d6,8d#6,16c6,8d#6,16c6,8d#6,8d6,16a#,8d6,16a#,8d6,8c6,8a#,8g,8a#,c6";
const char song6[]="WeWishYou:d=4,o=5,b=200:d,g,8g,8a,8g,8f#,e,e,e,a,8a,8b,8a,8g,f#,d,d,b,8b,8c6,8b,8a,g,e,d,e,a,f#,2g,d,g,8g,8a,8g,8f#,e,e,e,a,8a,8b,8a,8g,f#,d,d,b,8b,8c6,8b,8a,g,e,d,e,a,f#,1g,d,g,g,g,2f#,f#,g,f#,e,2d,a,b,8a,8a,8g,8g,d6,d,d,e,a,f#,2g";
const char *songs[] = { song0, song1, song2, song3, song4, song5, song6 };
#define OCTAVE_OFFSET 0

static bool selfTest = false;
static int selfTestCnt;

static unsigned long lastTimeNotePlaying = 0;
static byte default_dur = 4;
static byte default_oct = 6;
static long wholenote;
static long duration;
static byte scale;


/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
const char* parseSong(const char *p)
{
  int bpm = 63;
  int num;
  
  // Absolutely no error checking in here
  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  _PF(MODULE"Playing song: ");

  while (*p != ':') {
    _PF("%c", *p);
    p++;   // ignore name
  }
  p++;                     // skip ':'
  _PF(", ");

  // get default duration
  if (*p == 'd')
  {
    p++; p++;              // skip "d="
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if (num > 0) default_dur = num;
    p++;                   // skip comma
  }
  _PF("d=%d, ", default_dur);

  // get default octave
  if (*p == 'o')
  {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if (num >= 3 && num <= 7) default_oct = num;
    p++;                   // skip comma
  }
  _PF("o=%d, ", default_oct);

  // get BPM
  if (*p == 'b')
  {
    p++; p++;              // skip "b="
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                   // skip colon
  }
  _PF("b=%d, ", bpm);

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)
  _PF("whole note: %ld ms\n", wholenote);

  duration = 0;
  lastTimeNotePlaying = millis();

  return p;
}

// now begin note loop
const char* playmusic(const char *p)
{
  int num;
  byte note;

  if (*p == 0)
  {
    return p;
  }
  if (millis() - lastTimeNotePlaying > duration)
    lastTimeNotePlaying = millis();
  else return p;
  // first, get note duration, if available
  num = 0;
  while (isdigit(*p))
  {
    num = (num * 10) + (*p++ - '0');
  }

  if (num) duration = wholenote / num;
  else duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

  // now get the note
  note = 0;

  switch (*p)
  {
    case 'c':
      note = 1;
      break;
    case 'd':
      note = 3;
      break;
    case 'e':
      note = 5;
      break;
    case 'f':
      note = 6;
      break;
    case 'g':
      note = 8;
      break;
    case 'a':
      note = 10;
      break;
    case 'b':
      note = 12;
      break;
    case 'p':
    default:
      note = 0;
  }
  p++;

  // now, get optional '#' sharp
  if (*p == '#')
  {
    note++;
    p++;
  }

  // now, get optional '.' dotted note
  if (*p == '.')
  {
    duration += duration / 2;
    p++;
  }

  // now, get scale
  if (isdigit(*p))
  {
    scale = *p - '0';
    p++;
  }
  else
  {
    scale = default_oct;
  }

  scale += OCTAVE_OFFSET;

  if (*p == ',')
    p++;       // skip comma for next note (or we may be at the end)

  // now play the note

  if (note)
  {
    int frequency = (notes[(scale - 4) * 12 + note] +50)/ 100;
//    _PF(MODULE"Playing: note %d, scale %d, frequency %d Hz, duration %ld ms\n", note, scale, frequency, duration);
    EasyBuzzer.singleBeep(frequency, duration);
    if (millis() - lastTimeNotePlaying > duration)
      lastTimeNotePlaying = millis();
    else return p;
    EasyBuzzer.beep(0);
    EasyBuzzer.stopBeep();
  }
  else
  {
//    _PF(MODULE"Playing pause: duration %ld ms\n", duration);
    return p;
  }
  
  _PF(MODULE"Incorrect Song Format!\n");

  return NULL; //error
}


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskSoundUpdate() {
  static bool ready_to_run = false;
  if (t_SoundUpdate.isFirstIteration()) {
  }

  if (isTimeValid()) {
    if (!ready_to_run) {
      ready_to_run = true;
      EasyBuzzer.setPin(PIN_BUZZ);
      selfTestCnt = 0;
      selfTest = !gConf.quietNights || 
        (gConf.quietNights &&
          ( (myTZ.hour()>=6) && (myTZ.hour()<=22) 
          )
        );
      if (selfTest)
        _PF(MODULE"Self test started\n");
      else
        _PF(MODULE"Night time, self test not started\n");
    }
  } else {
    return;
  }
  
  static const char *sndptr = NULL;
  static bool play_song = false;

  if (selfTest) {
    selfTestCnt++;
    if (selfTestCnt == SND_SECONDS_BY_TICKS(1)) {
      if ((myTZ.month()==12) && 
        ((myTZ.day()>=24) && (myTZ.day()<=26)) ) {
        sndptr = parseSong(songs[6]); // playing "We Wish You..."
      } else {
        sndptr = parseSong(songs[random(1000) % 6]); // playing other random song
      }
      play_song = true;
    }
  }

  if (NULL != sndptr) {
    sndptr = playmusic(sndptr);
    if (NULL != sndptr) {
      if (*sndptr == 0) {
        sndptr = NULL;
      }
    }
  } else if (play_song) {
    _PF(MODULE"Song finished playing\n");
    play_song = false;
    if (selfTest)
      selfTest = false;
  }
  
	/* Always call this function in the loop for EasyBuzzer to work. */
	EasyBuzzer.update();
}


void setupSound() {
}

/* EOF */
