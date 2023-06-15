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
 *  Schedule each Nextion Action according to its button message
 *  First look for any action common to all Pages
 *  Thereafter direct Button input to Routine appropriate to active Nextion Page
 **************************************************************************************************************************
*/
void buttonScheduler()
{
  message = nextionReceive();           //returns a String of max 4 characters
  if(replyExpected == true)
  {
    if((millis() - startTime) >= (readTimeout * 1000))
    {
      replyExpected = false;
      nextionSetText("Status","Timeout waiting for CV Data");
    }
  }
  if (message != "")
  {
    if(message.startsWith("NNN") || message.startsWith("TTT"))       //Nextion Sending Numeric Text
    {
      getNextionText(message.substring(3));
      return;
    }
    button = message.toInt();
    switch (button)
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
        #if defined ESP
          EEPROM.commit();        //
        #endif
        initPage(MenuPage);
        break;
    #if defined WIFI
      case WiFiRetryButton:
        if (wifiEnabled == 1)
          {
            wifiImage = WIFI_X;
            Serial.println(F("Retrying..."));
            nextionCommand("Sig.pic=" + String(wifiImage));
            if (WiFi.status() != WL_CONNECTED) connectWiFi(readEEPROMByte(eeWiFiRetries));
          }
          if (!client.connect(host.c_str(), port))
          {
            wifiProgress = "DCCEX Connection Failed";
            nextionSetText("Progress", wifiProgress);
            Serial.println(wifiProgress);
          } else {
            wifiProgress = "DCCEX Connection Successful";
            nextionSetText("Progress", wifiProgress);
            Serial.println(wifiProgress);
          }
          break;
        #endif
      default:                //switch accordingly to active Page
        switch (nextionPage)  //These cases execute the "Process" routines
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
            case LocosPage:
              locosPage(button);
              break;
            #if !defined NO_ACCESSORIES
              case AccPage:
                accPage(button);
                break;
              case RoutesPage:
                routesPage(button);
                break;
            #endif
            #if !defined NO_PROGRAMMING
              case ProgramPage:
                programPage(button);
                break;
            #endif
            case ConfigPage:
              configPage(button);
              break;
            #if !defined NO_ACCESSORIES
              case AccEditPage:
                accEditPage(button);
                break;
              #endif
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
/*
 **************************************************************************************************************************
   Process Buttons common to All Pages and non-functional Page "Catch-Alls"
 **************************************************************************************************************************
   "STOP" button
 *************************************************************************************************************************
*/
void stopButton()
{
  if (readEEPROMByte(eeLocoStopAll) == 1)
  {
    for (uint8_t i = 0; i < numLocoSlots; i++)
    {
      resumeSpeeds[i] = locos[selectedIDs[i]].speed;
      locos[selectedIDs[i]].speed = 0;
      doDCC(i);
    }
    activateSlot(activeSlot);
  } else {
    resumeSpeeds[activeSlot] = locos[selectedIDs[activeSlot]].speed;
    locos[selectedIDs[activeSlot]].speed = 0;
    doDCC(activeSlot);
    activateSlot(activeSlot);
  }
}
/*
 *************************************************************************************************************************
   "GO" button
 *************************************************************************************************************************
*/
void goButton()
{
  if (readEEPROMByte(eeLocoStopAll) == 1)
  {
    for (uint8_t i = 0; i < numLocoSlots; i++)
    {
      locos[selectedIDs[i]].speed = resumeSpeeds[i];
      doDCC(i);
    }
  } else {
    locos[selectedIDs[activeSlot]].speed = resumeSpeeds[activeSlot];
    doDCC(activeSlot);
  }
//  activateSlot(activeSlot);
}
/*
 *************************************************************************************************************************
   "PowerON" button
 *************************************************************************************************************************
*/
void powerONButton()
{
  PowerState = 1;
  sendCMD(F("<1>"));
}
/*
 *************************************************************************************************************************
   "PowerOFF" button
 *************************************************************************************************************************
*/
void powerOFFButton()
{
  PowerState = 0;
  sendCMD(F("<0>"));
}
/*
 *************************************************************************************************************************
   Process Menu Page
 *************************************************************************************************************************
*/
void menuPage(uint8_t button)
{
  LocosReturnPage = MenuPage;
  AccReturnPage = MenuPage;
  ProgReturnPage = MenuPage;
  RoutesReturnPage = MenuPage;
  #if defined NO_ACCESSORIES
    if (button >= 5 && button <=6) return;
  #endif
  #if defined NO_PROGRAMMING
    if (button == 7) return;
  #endif
  #if defined NO_WIFI
    if (button == 9) return;
  #endif
  if (button != 0) initPage(button);
}
/*
 *************************************************************************************************************************
   Process Credits Page
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
   Process Information Page
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
   Process Accessory Images Page
 *************************************************************************************************************************
*/
#if !defined NO_ACCESSORIES
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
  }  
#endif
  /*
 *************************************************************************************************************************
   Process Tips Page
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
