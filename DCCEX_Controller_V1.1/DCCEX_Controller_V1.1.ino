/***********************************************************************
*                  
* Nextion Based DCC++EX Controller                 
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
*  Designed and written by Norman Halland  2022
*  
**************************************************************************************************************************
*/
#include "DCCEX_Throttle.h"
/*
**************************************************************************************************************************
* Setup Code
**************************************************************************************************************************
*/
void setup() 
{
  Serial.begin(115200);     //For the direct connection to DCC++ or DCC-EX
  nextion.begin(38400);     //This speed has been found to be suitable for both Hard- and Software Nextion Connections.
  wait(100);
  initPage(CoverPage);
  wait(3000);

  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), PinA, RISING); //interrupt on PinA, rising edge signal & executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(digitalPinToInterrupt(pinB), PinB, RISING); // same for B

  #if defined ESP
    EEPROM.begin(eepromEnd+1);                     //Same as UNO - for now
    wifiEnabled = readEEPROMByte(eewifiEnabled);
    #if defined WIFI
      if(connectWiFi(readEEPROMByte(eeWiFiRetries)) == 1)
      {
        Serial.println(F("WiFi NOT connected! "));
        Serial.print(F("IP address: "));
        Serial.println(WiFi.localIP());
        Serial.print(F("RSSI: "));
        Serial.println(WiFi.RSSI());
      }
    #endif
  #endif

  #if defined SEND_POWER_STATE
    if (readEEPROMByte(eePUState) == 1) powerONButton();    // Set both the Nextion and Command Station Power State
    else powerOFFButton();
  #endif

  if (EEPROM.read(eepromEnd) != EEPROMCODE) initEEPROM();       //Test to see if EEPROM has been initialized before
  uint8_t r = 0;
  for(uint16_t s = eeSelIDs; s < (eeSelIDs + numLocoSlots); s++)
  {
    selectedIDs[r] = readEEPROMByte(s);
    r++;
  }
  activeSlot = readEEPROMByte(eeActiveSlot);
  #if !defined WIFI
    initPage(MenuPage);                       //Display the Menu Page first after the Cover Page/
  #endif
  nextionCommand("bkcmd=0");    //Suppress Error details from the Nextion
  //Initialize Function State arrays for all possible Locos
  for(uint8_t i = 0; i < numLocos; i++)
  {
    LocoFN0to4[i] = 128;
    LocoFN5to8[i] = 176;
    LocoFN9to12[i] = 160;
    LocoFN13to20[i] = 0;
    LocoFN21to28[i] = 0;
  }
  //Ensure Edit Mode is OFF
  eMode = 0;
}//Ready to Rock and Roll
/*
**************************************************************************************************************************
* Main Program Loop
**************************************************************************************************************************
*/
void loop() 
{
  encoderInterrupt();                     // check for rotary encoder activity
  receiveCMD();                           // Receive anything from the Command Station
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
  checkREButton();      // check for change in direction from Rotary Encoder
  buttonScheduler();    // Process the received Nextion data
  #if defined WIFI
    checkWiFi();
  #endif
}   
/*
 **************************************************************************************************************************
 * Process each Nextion Page according to its button message
 * First look for any action common to all Pages
 * Thereafter direct Button input to Routine appropriate to active Nextion Page
 **************************************************************************************************************************
*/
void buttonScheduler()
{
  {
    message = nextionReceive();           //returns a String of max 4 characters
    if (message != "") 
    {
      uint8_t button = message.toInt();     //It's all about the button:-)
      switch(button)
      {
        case StopButton:
          stopButton();
          break;
        case GoButton:
          goButton();
          break;
        case PowerONButton:
          powerONButton();
          break;
        case PowerOFFButton:
          powerOFFButton();
          break;
        case MenuButton:
          initPage(MenuPage);
          break;
        case ThrottleButton:
          initPage(ThrottlePage);
          break;
        #if defined WIFI
        case WiFiRetryButton:
          if(wifiEnabled == 1)
          {
            wifiImage = WIFI_OFF;
            Serial.println(F("Retrying..."));
            nextionCommand("Sig.pic=" + String(wifiImage));
            if(WiFi.status() != WL_CONNECTED) connectWiFi(readEEPROMByte(eeWiFiRetries));
          }
          if(!client.connect(host.c_str(), port))
          {
            wifiProgress = "DCCEX Connection Failed";
            nextionSetText("Progress", wifiProgress);
            Serial.println(wifiProgress);
          }else{
            wifiProgress = "DCCEX Connection Successful";
            nextionSetText("Progress", wifiProgress);
            Serial.println(wifiProgress);
          }
          break;
        #endif
        default:                //switch accordingly to active Page
          switch(nextionPage)   //These cases execute the "Process" routines
          {
            case MenuPage:
              menuPage(button);
              break;
            case ThrottlePage:
              throttlePage(button);
              break;
            case LocoEditPage:
              locoEditPage(button);
              break;
            case RosterPage:
              rosterPage(button);
              break;
            case AccPage:
              accPage(button);
              break;
            case RoutesPage:
              routesPage(button);
              break;
            case ProgramPage:
              programPage(button);
              break;
            case ConfigPage:
              configPage(button);
              break;
            case AccEditPage:
              accEditPage(button);
              break;
            case StatusPage:
              statusPage(button);
              break;
            case FunctionEditPage:
              functionEditPage(button);
              break;
            case CreditsPage:
              creditsPage(button);
              break;
            case InfoPage:
              infoPage(button);
              break;
            case TipsPage:
              tipsPage(button);
              break;
            default:
              break;
          }
      }
    }
  }
}
/*
 **************************************************************************************************************************
 * Process Buttons common to All Pages and non-functional Page "Catch-Alls"
 **************************************************************************************************************************
 * "STOP" button
 *************************************************************************************************************************
*/
void stopButton()
{
  if(readEEPROMByte(eeLocoStopAll) == 1)
  {
    for(uint8_t i = 0; i < numLocoSlots; i++)
    {
      resumeSpeeds[i] = locos[selectedIDs[i]].speed;  
      locos[selectedIDs[i]].speed = 0;
      doDCC(i);
    }
  }else {
    resumeSpeeds[activeSlot] = locos[selectedIDs[activeSlot]].speed;
    locos[selectedIDs[activeSlot]].speed = 0;
    doDCC(activeSlot);
  }
  activateSlot(activeSlot);  
}
/*
 *************************************************************************************************************************
 * "GO" button
 *************************************************************************************************************************
*/
void goButton()
{
  if(readEEPROMByte(eeLocoStopAll) == 1)
  {
    for(uint8_t i = 0; i < numLocoSlots; i++)
    {
      locos[selectedIDs[i]].speed = resumeSpeeds[i];  
      doDCC(i); 
    }
  }else {
    locos[selectedIDs[activeSlot]].speed = resumeSpeeds[activeSlot];
    doDCC(activeSlot);  
  }
  activateSlot(activeSlot);  
}
/*
 *************************************************************************************************************************
 * "PowerON" button
 *************************************************************************************************************************
*/
void powerONButton()
{
  PowerState = 1;
  nextionSetValue(F("Power"), 1);
  sendCMD(F("<1>"));
}
/*
 *************************************************************************************************************************
 * "PowerOFF" button
 *************************************************************************************************************************
*/
void powerOFFButton()
{
  PowerState = 0;
  nextionSetValue(F("Power"), 0);
  sendCMD(F("<0>"));
}
/*
 *************************************************************************************************************************
 * Process Menu Page
 *************************************************************************************************************************
*/
void menuPage(uint8_t button)
{
  originPage = MenuPage;
  prevPage = button;
  if(button != 0) initPage(button);
}
/*
 *************************************************************************************************************************
 * Process Credits Page
 *************************************************************************************************************************
*/
void creditsPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(MenuPage);
      break;
    case CancelButton:
      initPage(MenuPage);
      break;
  }
}
/*
 *************************************************************************************************************************
 * Process Information Page
 *************************************************************************************************************************
*/
void infoPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(MenuPage);
      break;
    case CancelButton:
      initPage(MenuPage);
      break;
  }
}
/*
 *************************************************************************************************************************
 * Process Accessory Images Page
 *************************************************************************************************************************
*/
void accImagesPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(AccEditPage);
      break;
    case CancelButton:
      initPage(AccEditPage);
      break;
  }
}  /*
 *************************************************************************************************************************
 * Process Tips Page
 *************************************************************************************************************************
*/
void tipsPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(MenuPage);
      break;
    case CancelButton:
      initPage(MenuPage);
      break;
  }
}  
