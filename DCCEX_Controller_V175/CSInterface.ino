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
uint8_t connectWiFi(uint8_t retries)
{
  #if defined WIFI
    initPage(WiFiPage);                         //Switch Nextion to Status Page
    if(WiFiEnabled != 1) 
    {
      Console.println("WiFi not Enabled so continuing...");
      initPage(MenuPage);
      return(1);
    }
    WiFi.disconnect();
    wait(1000);
    client.stop();
    Console.println("Setting Mode to STA");
    WiFi.mode(WIFI_STA);
  	wait(100);
    Console.println("WiFi Begin");
    WiFi.begin(ssid.c_str(), password.c_str());
    int i = 0;
    wifiProgress = ("SSID Selected: " + ssid);
    nextionSetText(F("Progress"), wifiProgress);
    wait(1000);
    nextionSetText("RSSI", String(WiFi.RSSI(i)));
    nextionSetText("Progress", "Attempting to Connect");
    Console.println("Attempting to Connect");
    Console.print("Using SSID: ");
    Console.println(ssid);
    Console.print("and Password: ");
    Console.println(password);
    nextionSetText("WiFiState", "Please Wait...");
    uint8_t rcounts = 0;
    while(WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Console.print('.');
      retries--;
      rcounts ++;
      if(retries == 0)
      {
        nextionSetText("WiFiState", "Timeout");
        return(1);
      }
    }
    Console.println();
    Console.print("Retries before Connect: ");
    Console.println(rcounts);
    #if defined ENABLE_OTA
      ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
          type = "sketch";
        } else {  // U_FS
          type = "filesystem";
        }
        Console.println("Start updating " + type);
      });
      ArduinoOTA.onEnd([]() {
        Console.println("\nEnd");
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Console.printf("Progress: %u%%\r", (progress / (total / 100)));
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Console.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
          Console.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
          Console.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
          Console.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
          Console.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
          Console.println("End Failed");
        }
      });
      ArduinoOTA.begin();
      Console.println("Ready");
      Console.print("IP address: ");
      Console.println(WiFi.localIP());
    #endif
    wait(100);
    nextionSetText("WiFiState", "Connected to AP");      //Now to connect to DCCEX...
    nextionSetText("RSSI", String(WiFi.RSSI()));
    if(client.connect(host.c_str(), port))              //DCCEX Host on port 2560
    {
      nextionSetText("WiFiState", "DCCEX Connection Successful");
      csImage = CS_G;
    }else{
      nextionSetText("Progress", "DCCEX Connection Pending...");
      csImage = CS_W;
    }
    if(WiFiEnabled == 0) nextionSetText("Progress", "But WiFi disabled...");
  #endif
//  initPage(MenuPage);
  return(0);
}
/*  
 **************************************************************************************************************************
 *  Refresh Loco DCC Data (if Enabled)
 **************************************************************************************************************************
*/
void refreshDCC()                          // Refresh DCC Loco Data 
{
  if (readEEPROMByte(eeDCCRefresh) == 1)
  {
    DCCcurrentTime = millis(); //send DCC every 30000 ms
    if (((DCCcurrentTime - DCCtime) >= 30000) | (DCCflag == 1))
    {
      DCCflag = 0;
      DCCtime = DCCcurrentTime;
      doDCC(activeSlot);
    }
  }
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
    if(WiFiEnabled == 1)
    {
      if(WiFi.status() != WL_CONNECTED)
      {
        wifiProgress = "DCCEX Connection Failed";
        csImage = CS_X;
        nextionSetText("WiFiState", wifiProgress);
      }else
      {
        client.print(String(CMD)); 
        while (client.available()) 
        {
          String line = client.readStringUntil('\r');
        }
      }
      if(client.connected()) csImage = CS_G;
      #if defined SHOW_WIFI
        Console.println(CMD);          //to monitor data sent by WiFi
      #endif
    }else
    {
      Console.println(CMD);          //Print to Serial if WiFi disabled
    }
  #else
    Console.println(CMD);            //Print to Serial if no WiFi support
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
    if(WiFiEnabled == 1)
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
    while(console.available()>0)
    {
      c=console.read();
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
    case 'p':       //Power Command Received from Command Station
    {
      if(com[1] == '1') nextionCommand("Power.val=1");
      if(com[1] == '0') nextionCommand("Power.val=0");
      break;
    }
    case 'r':       //Response to Programming Track "Read" command
    {
      if(replyExpected == true)
      {
        char * token = strtok(com, " ");
        String lastToken = "";
        while (token != NULL )
        {
          if(String(token).startsWith("-1")) nextionSetText("S3", "Read Unsuccessful");
          else 
          {
            nextionSetText("S3", "Read Successful");
          }
          token = strtok(NULL, " ");
          lastToken = token;
          switch(nextionDataType)
          {
//             case CV_NUMBER:
//              nextionSetText("CVV", String(lastToken));
//              cv_Num = lastToken.toInt();
//              break;
            case CV_VALUE:
              nextionSetText("CVV", String(lastToken));
              cv_Value = lastToken.toInt();                        
              break;
            case CV2_VALUE:
              nextionSetText("CV2", String(lastToken));
              cv2_Value = lastToken.toInt();                         
              break;
            case CV3_VALUE:
              nextionSetText("CV3", String(lastToken));   
              cv3_Value = lastToken.toInt();                     
              break;
            case CV4_VALUE:
              nextionSetText("CV4", String(lastToken));                         
              cv4_Value = lastToken.toInt();                     
              break;
            case CV5_VALUE:
              nextionSetText("CV5", String(lastToken));                         
              cv5_Value = lastToken.toInt();                     
              break;
            case CV6_VALUE:
              nextionSetText("CV6", String(lastToken));                         
              cv6_Value = lastToken.toInt();                     
              break;
            default:
              break;
          }
        }
        replyExpected = false;
      }
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
 *
*/
void doDCC(uint8_t locoSlot) 
{
  String dccppCMD = "";
  if(readLocoAddress(selectedIDs[thNum][locoSlot]) != 0){
    #if defined DCCPP
      dccppCMD = ("<t " + String(selectedIDs[thNum][locoSlot] + 1) + " " + String(readLocoAddress(selectedIDs[thNum][locoSlot])) + " "
             + String(locos[selectedIDs[thNum][locoSlot]].speed ) + " " + String(locos[selectedIDs[thNum][locoSlot]].dir ) + ">");
    #else
      dccppCMD = ("<t " + String(readLocoAddress(selectedIDs[thNum][locoSlot])) + " "
             + String(locos[selectedIDs[thNum][locoSlot]].speed ) + " " + String(locos[selectedIDs[thNum][locoSlot]].dir ) + ">");
    #endif
    sendCMD(dccppCMD);
  }
  updateNextionThrottle(encoderPos);
}
/*
 *********************************************************
 * Build Function instructions and Send to Command Station
 *********************************************************
*/
#if defined DCCPP
  void doDCCfunction04() 
  {
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[thNum][activeSlot]))) +
                    " " + String(LocoFN0to4[selectedIDs[thNum][activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction58() 
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[thNum][activeSlot]))) +
                    " " + String(LocoFN5to8[selectedIDs[thNum][activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction912()          
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[thNum][activeSlot]))) +
                    " " + String(LocoFN9to12[selectedIDs[thNum][activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction1320()         
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[thNum][activeSlot]))) + " " + String(fGroup4) +
                    " " + String(LocoFN13to20[selectedIDs[thNum][activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
void doDCCfunction2128()         
{
  String dccppCMD = "";
  dccppCMD = ("<f " + String(readLocoAddress((selectedIDs[thNum][activeSlot]))) + " " + String(fGroup5)+
                    " " + String(LocoFN21to28[selectedIDs[thNum][activeSlot]]) + ">");
  sendCMD(dccppCMD);
}
#endif
//Required command is "<F Address Function State>"
#if !defined DCCPP
  void doDCCfunctions(uint8_t fNum)
  {
    String dccppCMD = "";
    dccppCMD = ("<F " + String(readLocoAddress((selectedIDs[thNum][activeSlot]))) +
              " " + String(fNum) + 
              " " + String(functions[selectedIDs[thNum][activeSlot]][g_fSlot]) + ">");
    sendCMD(dccppCMD);
  }
#endif
/*
 *********************************************************
 * Send Accessory command to Command Station
 *********************************************************
*/
void doDCCAcc(uint8_t accID)      //Send DCC Accessory command
{
  String dccppCMD = "";
  dccppCMD = ("<a " + String(readAccAddress(accID)) + " " + String(accStates[accID]) + ">");
//  nextionCommand("P2.pic=258");
  sendCMD(dccppCMD);
//  nextionCommand("P2.pic=258");
/*
#if defined WIFI
  if(client.connected()) nextionCommand("P2.pic=258"); else nextionCommand("P2.pic=259");
#endif
*/
}

/*
 *********************************************************
 * Check WiFi Signal and Update Nextion
 * 8 WiFi images are available:
 * - No WiFi - not Enabled
 * - 7 levels from Full Signal to No signal 
 * WiFi image only gets updated if there is a change
 *********************************************************
*/
#if defined WIFI
  void checkWiFi()
  {
      if(WiFiEnabled == 0) return;
      uint8_t lastImage = wifiImage;
      uint8_t lastCsImage = csImage;
      uint8_t wifiSignal;
      int rawSignal;    
      if(WiFi.status() == WL_CONNECTED)
      {
        wifiTimeout = 0;
        rawSignal = WiFi.RSSI();
        wifiSignal = rawSignal;
        wifiSignal = 256 - wifiSignal;
        if(wifiSignal < 110) wifiImage = WIFI_0;
        if(wifiSignal < 100) wifiImage = WIFI_1;
        if(wifiSignal < 90) wifiImage = WIFI_2;
        if(wifiSignal < 80) wifiImage = WIFI_3;
        if(wifiSignal < 70) wifiImage = WIFI_4;
        if(wifiSignal < 60) wifiImage = WIFI_5;
        if(wifiSignal < 50) wifiImage = WIFI_6;
        if(nextionPage == UpdatePage)
        {
          nextionSetText("Line1", "Ready to Proceed");
          nextionSetText("Line4", "WiFi Connected!");
        }
      }else
      {
        wifiImage = WIFI_X;
        if(millis() >= wifiTimeout)
        {
          Console.println("Timed Out");
          if(nextionPage == UpdatePage)
          {
            nextionSetText("Line1", "");
            nextionSetText("Line2", "Timeout waiting for Connection!");
            nextionSetText("Line3", "");
            nextionSetText("Line4", "WiFi NOT Connected!");
            nextionSetText("Line5", "Check Credentials and Retry");
          }
          wifiTimeout = 0;
        }
      }
      if(wifiImage != lastImage) nextionCommand(("Sig.pic=" + String(wifiImage)).c_str());  //Only write to Nextion if changed
      if(wifiImage == WIFI_X) {
        client.stop();
        csImage = CS_X;
        wifiStatus = "DCCEX Connection Lost";
      }
      if(csImage != lastCsImage) nextionCommand(("P2.pic=" + String(csImage)).c_str());
      wifiTimer = millis();
  }
#endif
/*
 *********************************************************
 * Update WiFi State and Status
 *********************************************************
*/
void updateWiFi()
{
  #if defined WIFI
    if(WiFiEnabled == 1)
    {
      if(WiFi.status() != WL_CONNECTED) 
      {
        connectWiFi(retries);
      }
      nextionSetText("RSSI", String(WiFi.RSSI()));
      wait(200);
      if(!client.connect(host.c_str(), port))
      {
        nextionSetText("WiFiState", "DCCEX Connection Failed");
        csImage = CS_X;
        nextionSetText("Progress","");
      }else{
        wifiStatus = "Connected to DCCEX";
        csImage = CS_G;
        nextionSetText("WiFiState", wifiStatus);
      }
      nextionSetText("Progress","");
    }else{
      client.stop();
      WiFi.disconnect();
      nextionSetText("WiFiState", "WiFi Disconnected");
      nextionSetText("Progress", "");
    }
  #endif
}
/*
 *********************************************************
 * Update Nextion Slider position and Speed value
 *********************************************************
*/
void updateNextionThrottle(uint8_t speed)
{
  encoderPos = speed;
  nextionSetValue("S", speed);
  nextionSetValue("S1", speed);
  nextionSetValue("T", speed);
}
