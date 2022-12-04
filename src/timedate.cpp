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
#define NTP_UPDATE_PERIOD  600 // 10 minutes
#define NTP_UPDATE_TICKS  (NTP_UPDATE_PERIOD*1000/SYS_TIME_UPD_PERIOD)

// sets how often the main system time update handler is called before
// the system time is attempted to be updated from the RTC again.
#define RTC_UPDATE_PERIOD   25  // 25 seconds
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

/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
#if USE_RTC
bool isValidDay(int month)
{
  int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month % 4 == 0) days[1] = 29;
  if (month > days[month - 1]) return false;
  else return true;
}

bool setRTCDateTime(byte h, byte m, byte s, byte d, byte mon, byte y) {

  RTC.adjust(DateTime(y, mon, d, h, m, s));
  DateTime now = RTC.now();
  // Verify if the RTC update worked
  if (
    (now.hour() == h) &&
    (now.minute() == m) &&
    (now.second() == s) &&
    (now.day() == d) &&
    (now.month() == mon) &&
    (now.year() == y)
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
    (now1.year() <= 2099) &&
    (now1.dayOfTheWeek() < 7)
  ) {
    curTime->hours = now1.hour();
    curTime->minutes = now1.minute();
    curTime->seconds = now1.second();
    curTime->day = now1.day();
    curTime->month = now1.month();
    curTime->year = now1.year();
    curTime->day_of_week = now1.dayOfTheWeek();
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
  static int rtc_update_ticks = 1; // update asap
  static bool rtc_set_current = false; // flag to request RTC to be written/updated
  rtcTime_t rtc_time;

  if (t_SystemTimeUpdate.isFirstIteration()) {
    _PL(MODULE"System time update task started");
#if USE_RTC
    Wire.begin(); // Start the I2C
    if (! RTC.begin())  // Init RTC
      _PL(MODULE"Could not find RTC");
    Wire.setClock(50000); // I2C works better at reduced speed
#endif // USE_RTC
//    setDebug(DEBUG, Serial);
    setDebug(NONE);
    myTZ.setLocation(TIME_ZONE_STR);
    setInterval(0); // disable automatic updates
#if USE_NTP
    setServer(NTP_SERVER_STR);
    ntp_available = true;
#endif // USE_NTP
  }

#if USE_NTP
  ntp_available = (WiFi.status() == WL_CONNECTED);

  last_update_time = lastNtpUpdateTime();
  if (last_update_time != last_saved_update_time) {
    _PL(MODULE"NTP update successful, current time: "+myTZ.dateTime());
    last_saved_update_time = last_update_time;
    rtc_set_current = true;
    ntp_update_triggered = false;
  }

  // Periodically update system time from NTP server
  if (ntp_available) {
    ntp_update_ticks--;
    if (ntp_update_ticks==0) {
      updateNTP();
      ntp_update_triggered = true;
      ntp_update_ticks = NTP_UPDATE_TICKS;
    } else if (ntp_update_triggered) {
      _PL(MODULE"NTP update timeout!");
      ntp_update_triggered = false;
      ntp_update_ticks = 1;  // try immediately next call until we are successful
    }
  }
#endif // USE_NTP

#if USE_RTC
  if (rtc_set_current) {
    if (setRTCDateTime(myTZ.hour(), myTZ.minute(), myTZ.second(), myTZ.day(), myTZ.month(), myTZ.year())) {
      _PL(MODULE"RTC updated, time: "+myTZ.dateTime()); 
      rtc_set_current = false;
    } else {
      _PL(MODULE"RTC not updated, FAILED"); 
    }
  }
  // Periodically update system time from RTC
  rtc_update_ticks--;
  if (rtc_update_ticks==0) {
    _PL(MODULE"RTC update scheduled");
    if (getRTCTime(&rtc_time)) {
      myTZ.setTime(rtc_time.hours, rtc_time.minutes, rtc_time.seconds,
        rtc_time.day, rtc_time.month, rtc_time.year);
      _PL(MODULE"RTC update time: "+myTZ.dateTime()); 
      rtc_update_ticks = RTC_UPDATE_TICKS;  // update again later
    } else {
      _PL(MODULE"RTC get update failed!");
      rtc_update_ticks = 1;  // try immediately next call until we are successful
    }
  }
#endif // USE_RTC
}


void taskTimeUpdate() {
  if (t_TimeUpdate.isFirstIteration()) {
  }
  static uint8_t last_second = 0;
  uint8_t this_second = second();
 
  // really do something only when the full second has changed
  if (this_second != last_second) {
    char separation_char;
    char timeStr[10] = {'\0'};
    if (this_second % 2)
      separation_char = ' ';
    else
      separation_char = ':';
    sprintf(timeStr, "%02d%c%02d%c%02d", hour(), separation_char, minute(), separation_char, this_second);
    nixiePrint(0, timeStr);
    last_second = this_second;
  }
}


void setupTime() {
}

void loopTime() {
}
/* EOF */
