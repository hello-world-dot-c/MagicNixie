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
  char pathstr[40];
  path.toCharArray(pathstr, 40);
  // send the right file to the client (if it exists)
  _PF(MODULE"HandleFileRead: %s\n", pathstr); 
  if (path.endsWith("/")) path = HTMLMainFile;          // If a folder is requested, send the main file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path))                              // If the file exists
  {                            
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  _PF(MODULE"\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

/*---------------------------------------------------------------*/
/* This function reads the incoming arguments and updates the    */
/* network variables.                                            */
/*---------------------------------------------------------------*/
void handleRoot() 
{
  String str;
  long strval;
  bool var_handled = false;
  const netVars_t *netvars_p;

  _PF(MODULE"HandleRoot: /\n");    
  
  nvSetFirstVar();
  while (nvGetNextVar(&netvars_p)) {
    if (server.hasArg(netvars_p->name))
    {
      str = server.arg(netvars_p->name);
      if (nvSetVarByString(&str)) {
        _PF(MODULE"%s set to %s.\n", netvars_p->name, &str);
        if (handleApi) {  // Request received via API
          server.send(200, "text/html", "OK"); // Return OK
        } else {
          handleFileRead(HTMLMainFile); // Update WEB interface
          _PF(MODULE"html text send.\n");
        }
      } else {
        // Received something wrong.
        _PF(MODULE"ERROR: %s NOT set to %s.\n", netvars_p->name, &str);
        if (handleApi) {  // Request received via API
          server.send(200, "text/html", "ERROR"); // Return OK
        } else {
          handleFileRead(HTMLMainFile); // Update WEB interface
          _PF(MODULE"html text send.\n");
        }
      }
      var_handled = true;
    }
  }

  // handle non-variable commands
  if (!var_handled) {
    if (server.hasArg("SAVE")) {
      // Save present configuration in EEPROM.
      saveConfig();
      if (handleApi) {  // Request received via API
        server.send(200, "text/html", "OK"); // Return OK
      }
      _PF(MODULE"New configuration saved.\n");    
    }
    else {
      if (handleApi) {  // Request received via API
        _PF(MODULE"ERROR: Request not understood.\n");
        server.send(200, "text/html", "ERROR"); // Return ERROR
      } else {
        handleFileRead(HTMLMainFile); // Update WEB interface
        _PF(MODULE"html text send.\n");
      }
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
  String uriStr = server.uri();
  _PF(MODULE"HandleNotFound: %s\n",&uriStr);
  if (!handleFileRead(uriStr)) // check if the file exists in the flash memory (SPIFFS), if so, send it
  {
    server.send(404, "text/plain", "404: File Not Found");
  }
}

/*---------------------------------------------------------------*/
/* Generates the file: "MagicNixieWeb_load.js"                   */
/* This file includes the network variables to update the WEB    */
/* configuration interface.                                      */
/*---------------------------------------------------------------*/
void handleLoadScript()
{
  const netVars_t *netvars_p;

  _PF(MODULE"HandleLoadScript: MagicNixieWeb_load.js\n");

  String temp  = "document.addEventListener('DOMContentLoaded', function ()\n";
         temp += "{\n";

  nvSetFirstVar();
  while (nvGetNextVar(&netvars_p)) {
    for (int i=0; i<2; i++) {
      if (strlen(netvars_p->jsname[i]) >0) {
        temp += "  document.getElementById(\"";
        temp += netvars_p->jsname[i];
        if (netvars_p->type == TYPE_BOOL) {
          bool rval = *(bool *)(netvars_p->var);
          temp += "\").checked = \"";
          temp += (i==0) ? 
            ((rval) ? "true" : "false") : 
            ((rval) ? "ON"   : "OFF");
        } else {
          temp += (i==0) ? 
            "\").value = \"" :
            "\").innerHTML = \"";
          if (netvars_p->type == TYPE_UINT8) {
            uint8_t rval = *(uint8_t *)(netvars_p->var);
            temp += rval;
          } else if (netvars_p->type == TYPE_UINT16) {
            uint16_t rval = *(uint16_t *)(netvars_p->var);
            temp += rval;
          } else if (netvars_p->type == TYPE_UINT32) {
            uint32_t rval = *(uint32_t *)(netvars_p->var);
            temp += rval;
          }
        }
        temp += "\";\n";
      }
    }
  }
  temp += "  document.getElementById(\"colorBox\").style.backgroundColor = \"#";
  temp += hexFromRGB(gConf.ledRed, gConf.ledGreen, gConf.ledBlue);
  temp += "\";\n";
  temp += "}, false);\n";
		 
  server.send(200, "application/javascript", temp);
  _PF(MODULE"  MagicNixieWeb_loadScript transmitted.\n");
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
	  server.on("/MagicNixieWeb_load.js", handleLoadScript); 
    server.on("/API",handleAPI);       
    server.onNotFound(handleNotFound); // Anything else
        
    server.begin();
    _PF(MODULE"HTTP server started\n");
    // Print the IP address
    char chstr[20];
    WiFi.localIP().toString().toCharArray(chstr,20);
    _PF(MODULE"Use this URL: http://%s/\n", chstr);
    _PF(MODULE"HTTP server setup done\n");
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
