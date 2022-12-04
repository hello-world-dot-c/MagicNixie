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

#define MODULE "*TIM: "

#define NTP_UPDATE_TICKS  (60*1000/SYS_TIME_UPD_PERIOD)  // 60 seconds
#define NTP_SERVER  "192.168.1.1"
#define NTP_UDP_SEND_PORT 123 // port for NTP requests
#define NTP_UDP_LISTEN_PORT 8888 // port for NTP responses
#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message

// sets how often the main system time update handler is called before
// the system time is attempted to be updated from the RTC again
#define RTC_UPDATE_TICKS     12  // numbers of SYS_TIME_UPD_PERIOD


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/

/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
RTC_DS3231 RTC;
AsyncUDP udp;
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
static char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
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

bool isValidDay(int month)
{
  int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month % 4 == 0) days[1] = 29;
  if (month > days[month - 1]) return false;
  else return true;
}

/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskSystemTimeUpdate() {
  static bool ntp_init = false;
  static int ntp_update_ticks = NTP_UPDATE_TICKS;
  static int rtc_update_ticks = RTC_UPDATE_TICKS;
  rtcTime_t rtc_time;

  if (t_SystemTimeUpdate.isFirstIteration()) {
    _PL(MODULE"System time update task started");
    Wire.begin(); // Start the I2C
    if (! RTC.begin())  // Init RTC
      _PL(MODULE"Could not find RTC");
    Wire.setClock(50000); // I2C works better at reduced speed
  }

  // As soon as WiFi is available, set up the NTP listener once
  if (!ntp_init) {
    if (WiFi.status() == WL_CONNECTED) {
      if(udp.listen(NTP_UDP_LISTEN_PORT)) {
          Serial.print("UDP Listening on IP: ");
          Serial.println(WiFi.localIP());
          udp.onPacket([](AsyncUDPPacket packet) {
              Serial.print("UDP Packet Type: ");
              Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
              Serial.print(", From: ");
              Serial.print(packet.remoteIP());
              Serial.print(":");
              Serial.print(packet.remotePort());
              Serial.print(", To: ");
              Serial.print(packet.localIP());
              Serial.print(":");
              Serial.print(packet.localPort());
              Serial.print(", Length: ");
              Serial.print(packet.length());
              Serial.print(", Data: ");
              Serial.write(packet.data(), packet.length());
              Serial.println();
              //reply to the client
              packet.printf("Got %u bytes of data", packet.length());
          });
      }
      ntp_init = true;
      _PL(MODULE"NTP initialized since WiFi became available");
    }
  }
/*
  // Check NTP handler and when it updated system time, also update the RTC
  if (!ntp_init) {
    if (timeClient.forceUpdate()) {
      byte h = timeClient.getHours();
      byte m = timeClient.getMinutes();
      byte s = timeClient.getSeconds();
      byte d = timeClient.getDay();
//      byte mon = timeClient. ();
//      byte y = timeClient();
        bool update_rtc = setRTCDateTime(h, m, s, d, mon, y);
        _PF(MODULE"NTP Update Time: %02d:%02d:%02d %s, the %02d.%02d.%04d\n", 
          h, m, s, daysOfTheWeek[weekday()-1], d, mon, y);
        if (update_rtc)
          _PL(MODULE"RTC set update successful");
        else
          _PL(MODULE"RTC set update failed!");
      } else
      {
        _PL(MODULE"System time not set!");
      }
    }
  }
*/
  // Periodically update system time from RTC
  rtc_update_ticks--;
  if (rtc_update_ticks==0) {
    _PL(MODULE"RTC update scheduled");
    if (getRTCTime(&rtc_time)) {
      setTime(rtc_time.hours, rtc_time.minutes, rtc_time.seconds,
        rtc_time.day, rtc_time.month, rtc_time.year);
      _PF(MODULE"RTC Update Time: %02d:%02d:%02d %s, the %02d.%02d.%04d\n", 
        rtc_time.hours, rtc_time.minutes, rtc_time.seconds,
        daysOfTheWeek[rtc_time.day_of_week], rtc_time.day, rtc_time.month, rtc_time.year);
      rtc_update_ticks = RTC_UPDATE_TICKS;  // update again later
    } else {
      _PL(MODULE"RTC get update failed!");
      rtc_update_ticks = 1;  // try immediately next call until we are successful
    }
  }

  // Periodically update system time from NTP server
  if (ntp_init) {
    ntp_update_ticks--;
    if (ntp_update_ticks==0) {
      // Initialize values needed to form NTP request
      // (see URL above for details on the packets)
      memset(packetBuffer, 0, NTP_PACKET_SIZE);
      packetBuffer[0] = 0b11100011;   // LI, Version, Mode
      packetBuffer[1] = 0;     // Stratum, or type of clock
      packetBuffer[2] = 6;     // Polling Interval
      packetBuffer[3] = 0xEC;  // Peer Clock Precision
      // 8 bytes of zero for Root Delay & Root Dispersion
      packetBuffer[12]  = 49;
      packetBuffer[13]  = 0x4E;
      packetBuffer[14]  = 49;
      packetBuffer[15]  = 52;

      // all NTP fields have been given values, now
      // you can send a packet requesting a timestamp:
      IPAddress ip;
      if (ip.fromString(NTP_SERVER)) {
        // it is a valid IP address
        _PL(MODULE"NTC request sent to "+ip.toString());
        udp.writeTo(packetBuffer, NTP_PACKET_SIZE, ip, NTP_UDP_SEND_PORT);
        ntp_update_ticks = 1;  // try immediately next call until we are successful
      }
    }
  }
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
/*
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
*/
}

void loopTime() {
}
/* EOF */
