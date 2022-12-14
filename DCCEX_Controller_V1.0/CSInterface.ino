/***********************************************************************
*                  
* COPYRIGHT (c) 2022 Norman Halland (NormHal@gmail.com)
*
*  This program and all its associated modules is free software: 
*  you can redistribute it and/or modify it under the terms of the 
*  GNU General Public License as published by the Free Software 
*  Foundation, either version 3 of the License, or (at your option) 
*  any later version.
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see http://www.gnu.org/licenses
*
************************************************************************/
/*
 **************************************************************************************************************************
 * Routines to Process Data to and from the Command Station 
 **************************************************************************************************************************
 * Connect to WiFi if ESP
 * Nextion Messages:
 * "WiFiState" displays the current state of the WiFi Link
 * - "Starting Network"
 * - "Connected to Network"
 * - "No Network Found"
 * "Progress" displays what is pending/happening
 * - Waiting for WiFi...
 * - Retrying...
 * - Connecting to Base Station
 * - Timed out - Check Credentials
 ***************************************************
*/
int connectWiFi(uint8_t retries)
{
  #if defined WIFI
    initPage(StatusPage);                 //Switch Nextion to Status Page
      WiFi.begin(ssid.c_str(), password.c_str());
      myNextion.setComponentText("Progress", wifiProgress);
      wifiProgress = "Connecting to AP...";
      myNextion.setComponentText("RSSI", String(WiFi.RSSI()));
      myNextion.setComponentText("WiFiState", wifiStatus);
      const char* Timing = "\x7C\x2F\x2D\x5C";
      int i, j = 0;
      while(WiFi.status() != WL_CONNECTED) {
        myNextion.setComponentText("W", String(Timing[i]));
        i++;
        j++;
        if(i>3) i=0;
        delay(500);                                              //change this!!!
        myNextion.setComponentText("Count",String(j));           //Update the retry counter
        if(j>retries) return(1);
      myNextion.setComponentText("Progress", "Connected to AP!");
      }
      wifiStatus = "Connected to Network!";
      myNextion.setComponentText("WiFiState", wifiStatus);
      Serial.println(F(""));
      Serial.println(wifiStatus);
  #endif
  return(0);
}
/*
 **************************************************************************************************************************
 * Routines to Process Data to and from the Command Station 
 **************************************************************************************************************************
 * Send DCC++ Command String to Command Station
 ***************************************************
*/
void sendCMD(String CMD)
{
  #if defined (WIFI)
    if(wifiEnabled == 1)
    {
      if(!client.connect(host.c_str(), port))
      {
        wifiStatus = "Connection to DCCEX Failed";
        myNextion.setComponentText("Progress", wifiStatus);
        Serial.println(wifiStatus);
      }else{
        client.print(String(CMD)); 
        wifiStatus = "Connection Successful";
        myNextion.setComponentText("Progress", wifiStatus);
      }
    }else
    {
      Serial.println(CMD);          //Print to Serial if WiFi disabled
    }
  #else
    Serial.println(CMD);            //Print to Serial if no WiFi support
  #endif
}
/*
 ***************************************************
 * Process Data received from the Command Station
 ***************************************************
*/
void receiveCMD()
{
  char c;
  #if defined WIFI
    if(wifiEnabled == 1)
    {
      while(client.available()>0)
      {                                                      // while there is data on the serial line
        c=client.read();
        if(c=='<') sprintf(commandString,"");                //Don't store the "<"
        else if(c=='>') parse(commandString);                //Also strip off the ">" and parse the received String
        else if(strlen(commandString)<MAX_COMMAND_LENGTH)    // if commandString still has space, append character just read from serial line
        sprintf(commandString,"%s%c",commandString,c);     // otherwise, character is ignored (but continue to look for '<' or '>')
      }
    } 
  #else
    while(Serial.available()>0)
    {
      c=Serial.read();
      if(c=='<') sprintf(commandString,"");                //Don't store the "<"
      else if(c=='>') parse(commandString);                //Also strip off the ">" and parse the received String
      else if(strlen(commandString)<MAX_COMMAND_LENGTH)    // if commandString still has space, append character just read from serial line
      sprintf(commandString,"%s%c",commandString,c);     // otherwise, character is ignored (but continue to look for '<' or '>')
    }
  #endif
}
/*
 ***************************************************
 * Parse Data received from the Command Station
 * This code is intended to grow as more integration 
 * with the Command Station is included
 ***************************************************
*/
void parse(char *com)
{
  switch(com[0])
  {
    case 'p':
    {
      if(com[1] == '0') myNextion.setComponentValue("Power",0);
      if(com[1] == '1') myNextion.setComponentValue("Power",1);
      break;
    }
    default:
      break;
  }
}
/*
 *************************************************************************************************************************
 * Routines to Construct Command Strings and send them to the Command Station
 *************************************************************************************************************************
 * Build Loco instruction to Send to Command Station
 *********************************************************
*/
void doDCC() 
{
  String dccppCMD = "";
  dccppCMD = ("<t " + String(selectedIDs[activeSlot] + 1) + " " + String(readLocoAddress(selectedIDs[activeSlot])) + " "
                  + String(locos[selectedIDs[activeSlot]].speed ) + " " + String(locos[selectedIDs[activeSlot]].dir ) + ">");
  sendCMD(dccppCMD);
  myNextion.setComponentValue("throttle", encoderPos);
  myNextion.setComponentValue("speedVal", encoderPos);
}
/*
 *********************************************************
 * Build Function instructions and Send to Command Station
 *********************************************************
*/
void doDCCfunction04() 
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[activeSlot]))) +
                    " " + String(LocoFN0to4[selectedIDs[activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction58() 
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[activeSlot]))) +
                    " " + String(LocoFN5to8[selectedIDs[activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction912()          
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[activeSlot]))) +
                    " " + String(LocoFN9to12[selectedIDs[activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction1320()         
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[activeSlot]))) + " " + String(fGroup4) +
                    " " + String(LocoFN13to20[selectedIDs[activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction2128()         
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[activeSlot]))) + " " + String(fGroup5)+
                    " " + String(LocoFN21to28[selectedIDs[activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
/*
 *********************************************************
 * Send Accessory command to Command Station
 *********************************************************
*/
void doDCCAcc(uint8_t accID)      //Send DCC Accessory command
{
  String dccppCMD = "";
  dccppCMD = ("<a " + String(readAccAddress(accID)) + " " + String(accStates[accID]) + ">");
  sendCMD(dccppCMD);
}
#if defined WIFI
/*
 *********************************************************
 * Check WiFi Signal and Update Nextion
 * 8 WiFi images are available:
 * - No WiFi - not Enabled
 * - 7 levels from Full Signal to No signal 
 * WiFi image only gets updated if there is a change
 *********************************************************
*/
void checkWiFi()
{
  uint8_t lastImage = wifiImage;
  uint8_t i;
  if(wifiEnabled == 1)
  {
    uint8_t wifiSignal = WiFi.RSSI();
    wifiSignal = 256 - wifiSignal;
    if(wifiSignal < 110) wifiImage = WIFI_0;
    if(wifiSignal < 100) wifiImage = WIFI_1;
    if(wifiSignal < 90) wifiImage = WIFI_2;
    if(wifiSignal < 80) wifiImage = WIFI_3;
    if(wifiSignal < 70) wifiImage = WIFI_4;
    if(wifiSignal < 60) wifiImage = WIFI_5;
    if(wifiSignal < 50) wifiImage = WIFI_6;
  }else{
    wifiImage = WIFI_OFF;
  }
  if(wifiImage != lastImage) myNextion.sendCommand(("Sig.pic=" + String(wifiImage)).c_str());
}
#endif
/*
 *********************************************************
 * Update WiFi State and Status
 *********************************************************
*/
void updateWiFi()
{
  #if defined ESP
    if(myNextion.getComponentValue("WiFi") == 1)
    {
      updateCredentials();
      #if defined WIFI
        if(WiFi.status() != WL_CONNECTED) {
          connectWiFi(readEEPROMByte(eeWiFiRetries));
        }
      #endif
      myNextion.setComponentText("WiFiState", "WiFi Enabled...");
      wifiEnabled = 1;
    }else{
      #if defined WIFI
        WiFi.disconnect();
      #endif
      wifiEnabled = 0;
      writeEEPROMByte(eewifiEnabled, 0);
      myNextion.setComponentText("WiFiState", "WiFi Disconnected...");
      myNextion.setComponentText("Progress","");
    }
  #endif    
}
