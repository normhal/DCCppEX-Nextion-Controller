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
 *************************************************************************************************************************
 * Page Initializations
 *************************************************************************************************************************
*/
void initPage(uint8_t page)
{
  activatePage(page);
  if(!PowerState) myNextion.setComponentValue("Power",0);
  if(PowerState)  myNextion.setComponentValue("Power",1);
  myNextion.sendCommand(("Sig.pic=" + String(wifiImage)).c_str());
  switch (page)
  {
    //******************************************************************************************************************************
    case ThrottlePage:
    {  
      setLocoTabs(activeSlot);
      refreshThrottleValues(activeSlot);
      break;
    }
    //******************************************************************************************************************************
    case LocoEditPage:
    {
      myNextion.setComponentValue("A", readLocoAddress(editingID));
      #if defined ESP
        if(readLocoAddress(editingID) == 0) myNextion.setComponentText("N","");
        else myNextion.setComponentText("N", readEEPROMName(locoNameBase + (editingID * (locoNameLen+1))));      //Get Loco Name from Integrated EEPROM
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        if(readLocoAddress(editingID) == 0) myNextion.setComponentText("N","");
        else myNextion.setComponentText("N", readEEPROMName(locoNameBase + (editingID * (locoNameLen+1))));      //Get Loco Name from Integrated EEPROM
      #endif
      #if defined AVR_UNO && defined USE_NEXTION_EEPROM
        myNextion.sendCommand(("repo N.txt," + String((editingID)*(locoNameLen+1))).c_str());               //Loco Name stored in Nextion EEPROM
      #endif
      myNextion.setComponentText("ID", String(editingID));
      myNextion.setComponentValue("RN", readLocoRNum(editingID));
      if(eMode == 1) myNextion.setComponentValue("Edit", true);
      else myNextion.setComponentValue("Edit", false);                
      loadFunctions(LocoEditPage, editingID);
      break;
    }
    //******************************************************************************************************************************
    case RosterPage:
    {
      backupID = selectedIDs[activeSlot];       //Used by "Cancel" button
      rosterDrawPage(locoStartID);
      break;
    }
    //****************************************************************************************************************************** 
    case AccPage:                                                                                                                   
    {
      if (accStartID == 0) myNextion.sendCommand("PageDn.pic=PageDnGreyed");
      accDrawPage(accStartID);
      break;
    }
    //******************************************************************************************************************************
    case AccEditPage: 
    {
      myNextion.setComponentText("ID", String(editingID + accIDBase));
      #if defined ESP
        myNextion.setComponentText("N", readEEPROMName(accNameBase + (editingID * (accNameLen+1))));
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        myNextion.setComponentText("N", readEEPROMName(accNameBase + (editingID * (accNameLen+1))));
      #endif
      #if defined AVR_UNO && defined USE_NEXTION_EEPROM
        myNextion.sendCommand(("repo N.txt," + String(accNameBase + (editingID * (accNameLen+1)))).c_str());
      #endif
      myNextion.setComponentValue("A", readAccAddress(editingID));
      if(readAccAddress(editingID) != 0)
      {
        myNextion.sendCommand(("ImageA.pic=" + String(readAccImage(editingID))).c_str());
        myNextion.sendCommand(("ImageB.pic=" + String((readAccImage(editingID))+1)).c_str());    //Add 1 for the partner image
        myNextion.sendCommand(("Test.pic=" + String(readAccImage(editingID))).c_str());
        myNextion.sendCommand(("Test.pic2=" + String((readAccImage(editingID))+1)).c_str());    //Add 1 for the partner image
        myNextion.setComponentValue("DCC",1);
      }else{
        myNextion.sendCommand(("ImageA.pic=" + String(GreyedButton)).c_str());
        myNextion.sendCommand(("ImageB.pic=" + String(GreyedButton)).c_str());
        myNextion.sendCommand(("Test.pic=" + String(GreyedButton)).c_str());
        myNextion.sendCommand(("Test.pic2=" + String(GreyedButton)).c_str());
        myNextion.setComponentValue("DCC",1);                                       //Default to DCC
      }
      accEditMode = false;
      break;
    }
    //******************************************************************************************************************************
    case RoutesPage:
    {
      if (routeStartID == 0) myNextion.sendCommand("PageDn.pic=PageDnGreyed");
      editMode = false;
      eMode = 0;
      routeDrawPage(routeStartID);
      break;
    }
    //******************************************************************************************************************************
    case ProgramPage:
    {
      myNextion.setComponentValue("Main", 0);
      myNextion.setComponentValue("Prog", 0);
      progMode = 0;
      break;
    }
    //******************************************************************************************************************************
    case ConfigPage:                                                                                //Done
    {
      String nCmd = "bt2.val=" + String(readEEPROMByte(eeDCCRefresh));
      myNextion.sendCommand(nCmd.c_str());       //",String(readEEPROMByte(ee
      nCmd = "bt1.val=" + String(readEEPROMByte(eePUState));
      myNextion.sendCommand(nCmd.c_str());
      nCmd = "bt0.val=" + String(readEEPROMByte(eeRNumEnabled));
      myNextion.sendCommand(nCmd.c_str());
      myNextion.setComponentValue("n0", readEEPROMByte(eeThreshold));
      myNextion.setComponentValue("n1", readEEPROMByte(eeREIncrement));
      myNextion.setComponentValue("n2", readEEPROMByte(eeFunctionPulse));
      myNextion.setComponentValue("n3", readEEPROMByte(eeAccDelay));
      break;
    }
    //******************************************************************************************************************************
    case StatusPage:                                                                            //In Progress
    {  
      #if defined WIFI
        myNextion.setComponentValue("WiFi", readEEPROMByte(eewifiEnabled));
        wifiEnabled = readEEPROMByte(eewifiEnabled);
        myNextion.setComponentText("SSID", readEEPROMName(eeSSID));         //String(ssid));
        myNextion.setComponentText("Password", readEEPROMName(eePWD));      //password);
        myNextion.setComponentText("HostIP", readEEPROMName(eeIPAddr));     //host);
        myNextion.setComponentText("Port", String(readEEPROMAddr(eePort)));       //String(port)); 
        
        ssid = readEEPROMName(eeSSID);
        password = readEEPROMName(eePWD);
        host = readEEPROMName(eeIPAddr);
        port = readEEPROMAddr(eePort);

        if(wifiEnabled == 0) 
        {
          wifiStatus = "WiFi Disabled...";
          myNextion.setComponentText("Progress", "WiFi Disconnected...");
        }
        myNextion.setComponentText("RSSI", String(WiFi.RSSI()).c_str());
        myNextion.setComponentText("Retries", String(readEEPROMByte(eeWiFiRetries)).c_str());
        myNextion.setComponentText("WiFiState", wifiStatus);
      #endif
      break;
    }
    //******************************************************************************************************************************
    case FunctionEditPage:
    {
      myNextion.setComponentText("ID", String(editingID));                                                        //Load ID
      #if defined ESP
        myNextion.setComponentText("N", readEEPROMName(locoNameBase + (editingID * (locoNameLen+1))));
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        myNextion.setComponentText("N", readEEPROMName(locoNameBase + (editingID * (locoNameLen+1))));
      #endif
      #if defined AVR_UNO && defined USE_NEXTION_EEPROM
        myNextion.sendCommand(("repo N.txt," + String(locoNameBase + (editingID * (locoNameLen+1)))).c_str());    //Load Name
      #endif
      myNextion.setComponentValue("A", readLocoAddress(editingID));                                               //Load Address
      uint8_t funcNum = (readEEPROMByte((locoFuncBase + (editingID * 20)) + (fSlot*2)));
      myNextion.setComponentValue("F", funcNum & 0x7F);   //Load Function Number
      uint8_t funcImage = readEEPROMByte((locoFuncBase + (editingID * 20)) + ((fSlot*2)+1));                      //Retrieve Image Number
      myNextion.setComponentValue("n1", funcImage);                                                               //Load hidden Image Number
      if((funcNum & 0x80) == 128) myNextion.setComponentValue("Type", 1);
      else myNextion.setComponentValue("Type", 0);
      myNextion.sendCommand(("ImageA.pic=" + String(funcImage)).c_str());                                         //Load ImageA
      myNextion.sendCommand(("Test.pic=" + String(funcImage)).c_str());                                           //Load Test Image
      myNextion.sendCommand(("ImageB.pic=" + String(funcImage + 1)).c_str());                                     //Load ImageB
    }
    //******************************************************************************************************************************
    case CreditsPage:                                                                           //Still to Follow
    {
      break;
    }
    //******************************************************************************************************************************
    case InfoPage:
    {
      myNextion.setComponentText("Version", Version);
      break;
    }  
    //******************************************************************************************************************************
    case TipsPage:                                                                           //Still to Follow
    {
      break;
    }
  }
}
/*
 *************************************************************************************************************************
 * Activate a Nextion Page
 *************************************************************************************************************************
*/

void activatePage(uint8_t page)
{
  nextionPage = page;               //Save new Page reference
  myNextion.sendCommand(("page " + String(nextionPage)).c_str());
}
/*
 *************************************************************************************************************************
 * Set PageUp and PageDn Buttons
 *************************************************************************************************************************
 * 
*/
void setPageButtons(uint8_t startID, uint8_t rowsPerPage, uint8_t numRows)
{
  if (startID == 0)
  {
    myNextion.sendCommand(("PageUp.pic=" + String(PageUp)).c_str());
    myNextion.sendCommand(("PageUp.pic2=" + String(PageUpOn)).c_str());
    myNextion.sendCommand(("PageDn.pic=" + String(PageDnGreyed)).c_str());
    myNextion.sendCommand(("PageDn.pic2=" + String(PageDnGreyed)).c_str());
  }else{   
    myNextion.sendCommand(("PageDn.pic=" + String(PageDn)).c_str());
    myNextion.sendCommand(("PageDn.pic2=" + String(PageDnOn)).c_str());
  }
  if (startID < (numRows - rowsPerPage))
  {
    myNextion.sendCommand(("PageUp.pic=" + String(PageUp)).c_str());
    myNextion.sendCommand(("PageUp.pic2=" + String(PageUpOn)).c_str());
  }else{
    myNextion.sendCommand(("PageUp.pic=" + String(PageUpGreyed)).c_str());
    myNextion.sendCommand(("PageUp.pic2=" + String(PageUpGreyed)).c_str());
  }  
}
