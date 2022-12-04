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
#define MODULE "*TIM: "

#ifndef USE_NTP
#define USE_NTP 1
#endif
#ifndef USE_RTC
#define USE_RTC 1
#endif

#define TIME_ZONE_STR  F("de")

#define NTP_SERVER_STR  F("fritz.box") //F("192.168.1.1")
#define NTP_UPDATE_PERIOD  60 //600 // 10 minutes
#define NTP_UPDATE_TICKS  (NTP_UPDATE_PERIOD*1000/SYS_TIME_UPD_PERIOD)
#define NTP_UPDATE_REPEATS  3

// sets how often the main system time update handler is called before
// the system time is attempted to be updated from the RTC again.
#define RTC_UPDATE_PERIOD   600  // 25 seconds
#define RTC_UPDATE_TICKS  (RTC_UPDATE_PERIOD*1000/SYS_TIME_UPD_PERIOD)


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
Timezone myTZ;

/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
#if USE_RTC
RTC_DS3231 RTC;
#endif
bool setUpdateRtc = false;

/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
#if USE_RTC
bool setRTCDateTime(byte h, byte m, byte s, byte d, byte mon, word y) {
  byte rh, rm, rs, rd, rmon;
  word ry;
  
  _PF(MODULE"RTC want to update with: %02d:%02d.%02d, %02d.%02d.%04d\n", h,m,s,d,mon,y);
  RTC.adjust(DateTime(y, mon, d, h, m, s));
  DateTime now = RTC.now();
  rh = now.hour();
  rm = now.minute();
  rs = now.second();
  rd = now.day();
  rmon = now.month();
  ry = now.year();
  _PF(MODULE"RTC read back          : %02d:%02d.%02d, %02d.%02d.%04d\n", rh,rm,rs,rd,rmon,ry);
  // Verify if the RTC update worked
  if (
    (rh == h) &&
    (rm == m) &&
    (rs == s) &&
    (rd == d) &&
    (rmon == mon) &&
    (ry == y)
  ) {
    return true;
  }
  else {
    return false;
  }
}

bool getRTCTime(rtcTime_t *curTime) {
  bool ret_val = false;

  // read twice and only accept the reading if they are identical and the values make sense
  DateTime now1 = RTC.now();
  DateTime now2 = RTC.now();
  if (
    (now1.hour() == now2.hour()) &&
    (now1.minute() == now2.minute()) &&
    (now1.second() == now2.second()) &&
    (now1.day() == now2.day()) &&
    (now1.month() == now2.month()) &&
    (now1.year() == now2.year()) &&
    (now1.dayOfTheWeek() == now2.dayOfTheWeek()) &&
    (now1.hour() < 24) &&
    (now1.minute() < 60) &&
    (now1.second() < 60) &&
    (now1.day() <= 31) &&
    (now1.month() <= 12) &&
    (now1.year() <= 2099) 
  ) {
    curTime->hours = now1.hour();
    curTime->minutes = now1.minute();
    curTime->seconds = now1.second();
    curTime->day = now1.day();
    curTime->month = now1.month();
    curTime->year = now1.year();
    ret_val = true;
  }  

  return ret_val;
}
#endif // USE_RTC


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskSystemTimeUpdate() {
  static time_t last_saved_update_time;
  time_t last_update_time;
  static bool ntp_available = false;
  static bool ntp_update_triggered = false;
  static int ntp_update_ticks = 1; // update asap
  static int rtc_update_ticks = 3; // update asap
  static int ntp_update_repeats = 3;
  static int rtc_update_repeats = 3;
  static bool rtc_available = false;
  rtcTime_t rtc_time;

  if (t_SystemTimeUpdate.isFirstIteration()) {
    _PL(MODULE"System time update task started");
#if USE_RTC
    Wire.begin(); // Start the I2C
    if (! RTC.begin())  // Init RTC
      _PL(MODULE"Could not find RTC");
    Wire.setClock(50000); // I2C works better at reduced speed
#endif // USE_RTC
//    setDebug(INFO, Serial);
    setDebug(NONE);
    myTZ.setCache(NVOL_START_TZ);
    myTZ.setLocation(TIME_ZONE_STR); // set "our" time zone
    setInterval(0); // disable automatic updates
#if USE_NTP
    setServer(NTP_SERVER_STR);
    last_saved_update_time = lastNtpUpdateTime();
#endif // USE_NTP
  }

#if USE_NTP
  // Determine indirectly whether an NTP update happened since
  // the last time we checked.
  last_update_time = lastNtpUpdateTime();
  if (last_update_time != last_saved_update_time) {
    _PL(MODULE"NTP update successful, current time: "+myTZ.dateTime());
    last_saved_update_time = last_update_time;
    ntp_update_ticks = NTP_UPDATE_TICKS-1-(NTP_UPDATE_REPEATS-ntp_update_repeats);
    ntp_update_repeats = NTP_UPDATE_REPEATS;
    ntp_update_triggered = false;
    ntp_available = true;
    rtc_available = false;
    setUpdateRtc = true;
  }

  // Periodically update system time from NTP server if we are online
  if (WiFi.status() == WL_CONNECTED) {
    ntp_update_ticks--;
    if (ntp_update_ticks==0) {
      updateNTP();  // manually trigger NTP update
      if (ntp_update_triggered) {
        ntp_update_repeats--;
        if (ntp_update_repeats==0) {
          _PF(MODULE"NTP update FAILED: Giving up for now");
          ntp_update_ticks = NTP_UPDATE_TICKS-NTP_UPDATE_REPEATS;
          ntp_update_triggered = false;
          ntp_update_repeats = NTP_UPDATE_REPEATS;
          ntp_available = false;
          rtc_available = true;
          rtc_update_ticks = RTC_UPDATE_TICKS;
        } else {
          ntp_update_ticks = 1;  // try immediately next call until we are successful
        }
      } else {
        ntp_update_repeats = NTP_UPDATE_REPEATS;
        ntp_update_triggered = true;
      }
    } else if (ntp_update_triggered) {
      _PL(MODULE"NTP update FAILED: Timeout");
      ntp_update_triggered = false;
      ntp_update_ticks = 1;  // try immediately next call until we are successful
    }
  } else {
    ntp_available = false;
    rtc_available = true;
    rtc_update_ticks = RTC_UPDATE_TICKS;
  }
#endif // USE_NTP

#if USE_RTC
  // Periodically update system time from RTC as an alternative if NTP is not available
  if (!ntp_available && rtc_available) {
    rtc_update_ticks--;
    if (rtc_update_ticks==0) {
      if (getRTCTime(&rtc_time)) {
        rtcTime_t nowTime;
        nowTime.hours = myTZ.hour();
        nowTime.minutes = myTZ.minute();
        nowTime.seconds = myTZ.second();
        nowTime.day = myTZ.day();
        nowTime.month = myTZ.month();
        nowTime.year = myTZ.year();
        if (0 == memcmp(&rtc_time, &nowTime, sizeof(rtcTime_t))) {
          _PL(MODULE"RTC read same time, not updated: "+myTZ.dateTime()); 
        } else {
          myTZ.setTime(rtc_time.hours, rtc_time.minutes, rtc_time.seconds,
            rtc_time.day, rtc_time.month, rtc_time.year);
          _PL(MODULE"RTC read time, updated: "+myTZ.dateTime()); 
          rtc_update_ticks = RTC_UPDATE_TICKS;  // update again later
        }
      } else {
        _PL(MODULE"RTC read time FAILED!");
        rtc_update_ticks = RTC_UPDATE_TICKS;  // update again later
        rtc_update_ticks = 5;  // try immediately next call until we are successful
      }
    }
  }
#endif // USE_RTC
}


void taskTimeUpdate() {
  if (t_TimeUpdate.isFirstIteration()) {
  }

}


void taskTimeFastUpdate() {
  char separation_char;
  char timeStr[10];
  uint8_t this_second;
  uint8_t singles;
  uint8_t tens;
  uint8_t hour;

  if (secondChanged()) {
    if (gConf.use12hDisplay) {
      hour = myTZ.hourFormat12();
    } else {
      hour = myTZ.hour();
    }
    if (gConf.omitLeading0Hour) {
      sprintf(timeStr, "%2d", hour);
    } else {
      sprintf(timeStr, "%02d", hour);
    }
    this_second = myTZ.second();
    singles = this_second % 10;
    tens = this_second / 10;
    separation_char = (singles % 2) ? ' ' : ':';

    if (gConf.useSoftBlend) {
      sprintf(timeStr+2, "*%02d*%1d*", myTZ.minute(), tens);
      nixiePrint(0, timeStr, 2);
      sprintf(timeStr, "**%c**%c*%1d", separation_char, separation_char, singles);
      nixiePrint(0, timeStr, 1);
    } else {
      sprintf(timeStr+2, "%c%02d%c%02d", separation_char, myTZ.minute(), separation_char, myTZ.second());
      nixiePrint(0, timeStr, 0);
    }

#if USE_RTC
    if (setUpdateRtc) {
      if (setRTCDateTime(myTZ.hour(), myTZ.minute(), myTZ.second(), myTZ.day(), myTZ.month(), myTZ.year())) {
        _PL(MODULE"RTC updated, time: "+myTZ.dateTime()); 
        setUpdateRtc = false;
      } else {
        _PL(MODULE"RTC not updated, FAILED"); 
      }
    }
#endif // USE_RTC
  }
}


void setupTime() {
}

void loopTime() {
}
/* EOF */
