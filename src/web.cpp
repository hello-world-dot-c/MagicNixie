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

#define MODULE "*WB: "

#define DEBUG    1 // 0: No debug info. 1: Show debug info.


/**************************************************************************
GLOBAL VARIABLES/CLASSES
***************************************************************************/
// Web server at port 80
ESP8266WebServer server(80);
String HTMLMainFile = "/MagicNixieWeb.html"; // The main HTML file to load


/**************************************************************************
LOCAL VARIABLES/CLASSES
***************************************************************************/
byte handleApi = 0; // 1: Received a HTTP POST request via API interface. 

// See the description in the documentation
byte fadeInterpolation[97] =
{ 6,  6,  6,  7,  7,  8,  9, 10, 10, 11, //  1
 12, 13, 13, 14, 15, 16, 17, 19, 20, 22, //  2
 23, 25, 26, 28, 29, 32, 35, 38, 40, 45, //  3
 49, 53, 57, 62, 66, 71, 75, 79, 82, 86, //  4
 89, 91, 93, 95, 97, 98, 99,100,100,100, //  5
 99, 98, 97, 95, 93, 91, 89, 86, 82, 79, //  6
 75, 71, 66, 62, 57, 53, 49, 45, 40, 38, //  7
 35, 32, 29, 28, 26, 25, 23, 22, 20, 19, //  8
 17, 16, 15, 14, 13, 13, 12, 11, 10, 10, //  9
  9,  8,  7,  7,  6,  6,  6 };           // 10 (97)
  

/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/
String hexFromRGB (uint8_t r, uint8_t g, uint8_t b)
{
  String ret = "";
  if (r<16)
    ret += "0";
  ret += String(r, HEX);
  if (g<16)
    ret += "0";
  ret += String(g, HEX);
  if (b<16)
    ret += "0";
  ret += String(b, HEX);

  return ret;  
}

/*--------------------------------------------------------------------*/
/* Original code found here:                                          */
/*  https://tttapa.github.io/ESP8266/Chap16%20-%20Data%20Logging.html */
/*--------------------------------------------------------------------*/
String getContentType(String filename) 
{ // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".svg")) return "image/svg+xml"; // HHS new 2019-12-18
  return "text/plain";
}

/*--------------------------------------------------------------------*
 * Original code found here:                                          *
 *  https://tttapa.github.io/ESP8266/Chap16%20-%20Data%20Logging.html *
 *--------------------------------------------------------------------*/
bool handleFileRead(String path) 
{ 
  // send the right file to the client (if it exists)
  Serial.println(MODULE"  handleFileRead: " + path); 
  if (path.endsWith("/")) path = HTMLMainFile;          // If a folder is requested, send the main file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path))                              // If the file exists
  {                            
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println(MODULE"\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

/*---------------------------------------------------------------*/
/* This function reads the incoming arguments and updates the   */
/* status of the lamp.                                           */
/* http://<ip address>/                                          */
/*---------------------------------------------------------------*/
void handleRoot() 
{
  String str, strText;

  if( DEBUG ) Serial.println(MODULE"** HandleRoot called.");    
  
  if (server.hasArg("BLEND") )
  {
    str = server.arg("BLEND");
    if ( str == "1" ) 
    {
      // Received BLEND=1
      gConf.useSoftBlend = 1;
//      EEPROM.write(L_STATUS, 1); // Save new lamp status
//      EEPROM.commit();           // Needed in order to save content to flash.
      if( DEBUG ) Serial.println(MODULE"**  BLEND ON.");
      if( handleApi )
      {  // Request received via API
         server.send ( 200, "text/html", "OK" ); // Return OK
      }

      if( DEBUG ) Serial.println(MODULE"**  New blending status saved (ON).");
    } 
    else if ( str == "0" ) 
    {
      // Received BLEND=0
      gConf.useSoftBlend = 0;
//      EEPROM.write(L_STATUS, 0); // Save new lamp status
//      EEPROM.commit();           // Needed in order to save content to flash.
      if( DEBUG ) Serial.println(MODULE"**  BLEND OFF.");
      if( handleApi )
      {  // Request received via API
         server.send ( 200, "text/html", "OK" ); // Return OK
      }
/*
      for(uint16_t i = 0; i < strip.numPixels(); i++) 
      {
        strip.setPixelColor(i, strip.Color(0, 0, 0) );
        strip.show();
      }
*/
      if( DEBUG ) Serial.println(MODULE"**  New lamp status saved (OFF).");      
    } 
    else 
    {
      // Received something wrong.
      Serial.println(MODULE"Err Lamp status.");
      if( handleApi )
      {  // Request received via API
         server.send ( 200, "text/html", "ERROR" ); // Return OK
      }
    } 
    if( DEBUG ) Serial.println(MODULE"**  BLEND submit handled.");    
  }
  else if( server.hasArg("RED") || server.hasArg("GREEN") || server.hasArg("BLUE") )
  { 
    boolean colourError = 0;
    strText = "COLOUR: ";
    if( server.hasArg("RED") )
    {
      // Received RED=<value>
      str = server.arg("RED");
      strText += "RED: ";      
      if( isNumeric( str ) && (str.toInt() < 256) )
      {
        if( DEBUG ) Serial.println(MODULE"**  Data received are numeric");
        gConf.ledRed  = str.toInt();
        strText += str;
        if( DEBUG ) 
        {
          Serial.print("**  New RED value set: "); Serial.println(gConf.ledRed);
        }
      }
      else
      {
        if( DEBUG ) Serial.println(MODULE"**  Data received are NOT numeric");
        colourError = 1;
        strText += "N/A ";
        if( DEBUG ) 
        {
          Serial.print("**  Error new RED value: "); Serial.println(str);
        }
      }
    }
    if(server.hasArg("GREEN") )
    {
      // Received GREEN=<value>
      str = server.arg("GREEN");
      strText += " GREEN: ";
      if( isNumeric( str ) && (str.toInt() < 256) )
      {
        if( DEBUG ) Serial.println(MODULE"**  Data received are numeric");
        gConf.ledGreen = str.toInt();
        strText  += str;
        if( DEBUG ) 
        {
          Serial.print("**  New GREEN value set: "); Serial.println(gConf.ledGreen);
        }
      }
      else
      {
        if( DEBUG ) Serial.println(MODULE"**  Data received are NOT numeric");
        colourError = 1;
        strText += "N/A ";
        if( DEBUG ) 
        {
          Serial.print("**  Error new GREEN value: "); Serial.println(str);
        }
      }
    }
    if(server.hasArg("BLUE") )
    {
      // Received BLUE=<value>
      str      = server.arg("BLUE");
      strText += " BLUE: ";
      if( isNumeric( str ) && (str.toInt() < 256) )
      {
        if( DEBUG ) Serial.println(MODULE"**  Data received are numeric");
        gConf.ledBlue = str.toInt();
        strText += str;
        if( DEBUG ) 
        {
          Serial.print("**  New BLUE value set: "); Serial.println(gConf.ledBlue);
        }
      }
      else
      {
        if( DEBUG ) Serial.println(MODULE"**  Data received are NOT numeric");
        colourError = 1;
        strText    += "N/A ";
        if( DEBUG ) 
        {
          Serial.print("**  Error new BLUE value: "); Serial.println(str);
        }
      }
    } 
    if( handleApi )
    {  // Request received via API
      if( colourError )
        server.send ( 200, "text/html", "ERROR" ); // Return ERROR
      else
        server.send ( 200, "text/html", "OK" ); // Return OK
    }

    if( DEBUG ) Serial.println(MODULE"**  COLOUR submit handled.");  
  }
  else if(server.hasArg("BRIGHTNESS") )
  {
    // Received BRIGHTNESS=<value>
    str = server.arg("BRIGHTNESS");
    // Check if received value is numeric AND below 256
    if( isNumeric( str ) && (str.toInt() < 256) )
    {
      if( DEBUG ) Serial.println(MODULE"**  Data received are numeric");
      gConf.ledBrightness = str.toInt();
      if( handleApi )
      {
        server.send ( 200, "text/html", "OK" ); // Return OK  
      }
      else
      {
        handleFileRead( HTMLMainFile ); // Update WEB interface
      }
      if( DEBUG )
      {
        Serial.print("**  New BRIGHTNESS value set: "); Serial.println(gConf.ledBrightness);
      }          
    }
    else
    {
      if( DEBUG ) Serial.println(MODULE"**  Data received are NOT numeric");
      if( handleApi )
      {
        server.send ( 200, "text/html", "ERROR" ); // Return ERROR  
      }
      else
      {
        strText += "BRIGHTNESS error Wrong data: ";
        strText += str;
        handleFileRead( HTMLMainFile ); // Update WEB interface
      }
    }  
  }
  else if(server.hasArg("NIXIEBRIGHTNESS") )
  {
    // Received NIXIEBRIGHTNESS=<value>
    str = server.arg("NIXIEBRIGHTNESS");
    // Check if received value is numeric AND below 256
    if( isNumeric( str ) && (str.toInt() < 256) )
    {
      if( DEBUG ) Serial.println(MODULE"**  Data received are numeric");
      gConf.nixieBrightness = str.toInt();
      if( handleApi )
      {
        server.send ( 200, "text/html", "OK" ); // Return OK  
      }
      else
      {
        handleFileRead( HTMLMainFile ); // Update WEB interface
      }
      if( DEBUG )
      {
        Serial.print("**  New NIXIEBRIGHTNESS value set: "); Serial.println(gConf.nixieBrightness);
      }          
    }
    else
    {
      if( DEBUG ) Serial.println(MODULE"**  Data received are NOT numeric");
      if( handleApi )
      {
        server.send ( 200, "text/html", "ERROR" ); // Return ERROR  
      }
      else
      {
        strText += "NIXIEBRIGHTNESS error Wrong data: ";
        strText += str;
        handleFileRead( HTMLMainFile ); // Update WEB interface
      }
    }  
  }
  else if(server.hasArg("ANTIPOISON") )
  {
    // Received ANTIPOISON=<value>
    str = server.arg("ANTIPOISON");
    // Check if received value is numeric AND below 256
    if( isNumeric( str ) && (str.toInt() < 256) )
    {
      if( DEBUG ) Serial.println(MODULE"**  Data received are numeric");
      gConf.antiPoisoningLevel = str.toInt();
      if( handleApi )
      {
        server.send ( 200, "text/html", "OK" ); // Return OK  
      }
      else
      {
        handleFileRead( HTMLMainFile ); // Update WEB interface
      }
      if( DEBUG )
      {
        Serial.print("**  New ANTIPOISON value set: "); Serial.println(gConf.antiPoisoningLevel);
      }          
    }
    else
    {
      if( DEBUG ) Serial.println(MODULE"**  Data received are NOT numeric");
      if( handleApi )
      {
        server.send ( 200, "text/html", "ERROR" ); // Return ERROR  
      }
      else
      {
        handleFileRead( HTMLMainFile ); // Update WEB interface
      }
    }  
  }
  else if(server.hasArg("SAVE") )
  {
    // Save present configuration in EEPROM.
    saveConfig();
    
    if( handleApi )
    {  // Request received via API
       server.send ( 200, "text/html", "OK" ); // Return OK
    }

    if( DEBUG ) Serial.println(MODULE"**  New configuration saved.");    
  }
  else
  {
    if( handleApi )
    {  // Request received via API
      if( DEBUG ) Serial.println(MODULE"**  ERROR sent.");
      server.send ( 200, "text/html", "ERROR" ); // Return ERROR
    }
    else
    {
      handleFileRead( HTMLMainFile ); // Update WEB interface
      if( DEBUG ) Serial.println(MODULE"**  html text send.");
    }
  }
  handleApi = 0;
} // void handleRoot() END

/*---------------------------------------------------------------*/
/* This function is called when a HTTP POST request is received  */
/*   via http://<ip address>/API                                 */
/*---------------------------------------------------------------*/
void handleAPI() 
{
  handleApi = 1;
  handleRoot();
} // void handleAPI() END 

/*--------------------------------------------------------------------*/
/*                                                                    */
/*--------------------------------------------------------------------*/
void handleNotFound(void)  // if the requested file or page doesn't exist, return a 404 not found error
{
  Serial.println(MODULE"Received request");
  if (!handleFileRead(server.uri())) // check if the file exists in the flash memory (SPIFFS), if so, send it
  {
    server.send(404, "text/plain", "404: File Not Found");
  }
}

/*---------------------------------------------------------------*/
/* Holds the file: "MagicNixieWeb_load.js"                       */
/* This file includes the EEPROM stored values, which is needed  */
/*  in order to update the WEB configuration interface after     */
/*  reset or power up.                                           */
/*---------------------------------------------------------------*/
void MagicNixieWeb_loadScript ()
{
  Serial.println(MODULE"Received request");
  Serial.println(MODULE"  handleFileRead: MagicNixieWeb_load.js");
  
  String softBlendON = "OFF";
  String softBlendChecked = "false";
  if (gConf.useSoftBlend) 
  {
	  softBlendON = "ON";
	  softBlendChecked = "true";
  }
  String temp  = "document.addEventListener('DOMContentLoaded', function ()\n";
         temp += "{\n";
         temp += "  document.getElementById(\"blendingOnOffButton\").checked = \"";
         temp += softBlendChecked;
         temp += "\";\n";
         temp += "  document.getElementById(\"blendingStatus\").innerHTML = \"";
         temp += softBlendON;
         temp += "\";\n";
         temp += "  document.getElementById(\"brightnessSlider\").value = \"";
         temp += gConf.ledBrightness;
         temp += "\";\n";
         temp += "  document.getElementById(\"brightnessValue\").innerHTML = \"";
         temp += gConf.ledBrightness;
         temp += "\";\n";
         temp += "  document.getElementById(\"nixieBrightnessSlider\").value = \"";
         temp += gConf.nixieBrightness;
         temp += "\";\n";
         temp += "  document.getElementById(\"nixieBrightnessValue\").innerHTML = \"";
         temp += gConf.nixieBrightness;
         temp += "\";\n";
         temp += "  document.getElementById(\"antiPoisonSlider\").value = \"";
         temp += gConf.antiPoisoningLevel;
         temp += "\";\n";
         temp += "  document.getElementById(\"antiPoisonValue\").innerHTML = \"";
         temp += gConf.antiPoisoningLevel;
         temp += "\";\n";
         temp += "  document.getElementById(\"redSlider\").value = \"";
         temp += gConf.ledRed;
         temp += "\";\n";
         temp += "  document.getElementById(\"redValue\").innerHTML = \"";
         temp += gConf.ledRed;
         temp += "\";\n";
         temp += "  document.getElementById(\"greenSlider\").value = \"";
         temp += gConf.ledGreen;
         temp += "\";\n";
         temp += "  document.getElementById(\"greenValue\").innerHTML = \"";
         temp += gConf.ledGreen;
         temp += "\";\n";
         temp += "  document.getElementById(\"blueSlider\").value = \"";
         temp += gConf.ledBlue;
         temp += "\";\n";
         temp += "  document.getElementById(\"blueValue\").innerHTML = \"";
         temp += gConf.ledBlue;
         temp += "\";\n";
         temp += "  document.getElementById(\"square\").style.backgroundColor = \"#";
         temp += hexFromRGB(gConf.ledRed, gConf.ledGreen, gConf.ledBlue);
         temp += "\";\n";
         temp += "}, false);\n";
		 
  server.send ( 200, "application/javascript", temp );
  Serial.println(MODULE"  MagicNixieWeb_loadScript transmitted.");
} // String getPage ()



/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/
void taskWebConnect() {
  bool do_connect = false;

  if (t_WebConnect.isFirstIteration()) {
    do_connect = true;
  }

  if (do_connect)
  {
	// Associate the handler function to the path.
    server.on("/", handleRoot);        
	  server.on("/MagicNixieWeb_load.js", MagicNixieWeb_loadScript); 
    server.on("/API",handleAPI);       
    server.onNotFound(handleNotFound); // Any thing else
        
    server.begin();
    Serial.println(MODULE"**  HTTP server started");
    // Print the IP address
    Serial.print(MODULE"**   Use this URL : "); Serial.print("http://"); Serial.print(WiFi.localIP()); Serial.println("/");
    Serial.println(MODULE"** HTTP server setup done\n");
    do_connect = false; 
    t_WebRun.enable();
  }

}

void taskWebRun() {
  server.handleClient(); // Check for incoming requests
}

void setupWeb() {
  // nothing to be done yet  	 
}

/* EOF */
