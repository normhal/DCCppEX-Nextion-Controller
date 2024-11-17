/*
  *********************************************************************
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
  *   GNU General Public License for more details.
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
      nextionSetText("S3","Timeout waiting for CV Data");
    }
  }
  if (message != "")
  {
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
         initPage(MenuPage);
        break;
    #if defined WIFI
      case WiFiRetryButton:
        if (WiFiEnabled == 1)
        {
          wifiImage = WIFI_X;
          csImage = CS_X;
          nextionCommand("Sig.pic=" + String(wifiImage));
          nextionCommand("P2.pic=" + String(csImage));
          if (WiFi.status() != WL_CONNECTED) 
          {
            connectWiFi(retries);
          }
          if (!client.connect(host.c_str(), port))
          {
            wifiProgress = "DCCEX Connection Failed";
            csImage = CS_X;
            nextionSetText("Progress", wifiProgress);
          } else {
            wifiProgress = "DCCEX Connection Successful";
            csImage = CS_G;
            nextionSetText("WiFiState", wifiProgress);
          }
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
            case SelectionPage:
              selectionPage(button);
              break;
            case AccEditPage:
              accEditPage(button);
              break;
            case WiFiPage:
              #if defined WIFI
                wifiPage(button);
              #endif
              break;
            case FunctionEditPage:
              functionEditPage(button);
              break;
            case UpdatePage:
              nextionUpload(button);
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
    nextionCommand("P2.pic=" + String(CS_W));
    for (uint8_t i = 0; i < numLocoSlots; i++)
    {
      resumeSpeeds[i] = locos[selectedIDs[thNum][i]].speed;
      locos[selectedIDs[thNum][i]].speed = 0;
      doDCC(i);
    }
    activateSlot(activeSlot[thNum]);
  } else {
    resumeSpeeds[activeSlot[thNum]] = locos[selectedIDs[thNum][activeSlot[thNum]]].speed;
    locos[selectedIDs[thNum][activeSlot[thNum]]].speed = 0;
    doDCC(activeSlot[thNum]);
    activateSlot(activeSlot[thNum]);
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
    nextionCommand("P2.pic=" + String(CS_W));
    for (uint8_t i = 0; i < numLocoSlots; i++)
    {
      locos[selectedIDs[thNum][i]].speed = resumeSpeeds[i];
      doDCC(i);
    }
    activateSlot(activeSlot[thNum]);
  } else {
    locos[selectedIDs[thNum][activeSlot[thNum]]].speed = resumeSpeeds[activeSlot[thNum]];
    doDCC(activeSlot[thNum]);
    activateSlot(activeSlot[thNum]);
  }
/*
#if defined WIFI
  if(client.connected()) nextionCommand("P2.pic=258"); else nextionCommand("P2.pic=259");
#endif
*/
}
/*
 *************************************************************************************************************************
   "PowerON" button
 *************************************************************************************************************************
*/
void powerONButton()
{
  PowerState = ON;      //1;
  if(readEEPROMByte(eeJoinMode)) sendCMD("<1 JOIN>");
  else sendCMD("<1 MAIN>");
  
}
/*
 *************************************************************************************************************************
   "PowerOFF" button
 *************************************************************************************************************************
*/
void powerOFFButton()
{
  PowerState = OFF;
  sendCMD("<0>");
}
/*
 *************************************************************************************************************************
   Process Menu Page
 *************************************************************************************************************************
*/
void menuPage(uint8_t button)
{
  nextionPage = MenuPage;
  LocosReturnPage = MenuPage;
  AccReturnPage = MenuPage;
  ProgReturnPage = MenuPage;
  RoutesReturnPage = MenuPage;
  #if !defined WIFI
    if (button == 9) return;
  #endif
  if (button == SaveButton)
  {
    saveSelected();
    return;
  }
  if (button == RestoreButton)
  {
    restoreSelected();
    return;
  }
  if(button >=20 && button <= 23)
  {
    button = button - 20;
    thNum = button;
    nextionCommand("TH" + String(button + 1) + ".pic=" + String(TH1_BUTTON_ON + (button * 2)));
    wait(50);
    initPage(ThrottlePage);
    return;
  }
  if (button != 0)
  {
    initPage(button);
  }
}
/*
 *************************************************************************************************************************
   Process Accessory Images Page
 *************************************************************************************************************************
*/
void accImagesPage(uint8_t button)
{
  switch (button)
  {
    case Done_Press:
      initPage(AccEditPage);
      break;
    case Cancel_Press:
      initPage(AccEditPage);
      break;
  }
}  
/*
 ***********************************************************************************************************************
 * Process Nextion Update
 ***********************************************************************************************************************
*/
/*
void updatePage(button)
{
  initPage(UpdatePage);
}
*/
