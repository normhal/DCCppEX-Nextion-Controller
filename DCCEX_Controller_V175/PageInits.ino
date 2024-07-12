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
  if(!PowerState) nextionSetValue("Power",0);
  if(PowerState)  nextionSetValue("Power",1);
  wait(50);
  nextionCommand(("Sig.pic=" + String(wifiImage)).c_str());
#if defined WIFI
  if(client.connected())
  {
    nextionCommand("P2.pic=258");
    lastCSPic = 258; 
  }else
  {
    nextionCommand("P2.pic=259");
    lastCSPic = 259; 
  }
#endif
//  setHeadingDetails(activeSlot);
  switch(page)
  {
    //******************************************************************************************************************************
    case ThrottlePage:
    {  
      wait(50);
      populateSlots();
      activateSlot(activeSlot);
      break;
    }
    //******************************************************************************************************************************
    case LocoEditPage:
    {
      nextionSetText("AD", String(readLocoAddress(editingID)));
      if(readLocoAddress(editingID) == 0)
      {
        nextionSetText("RN","");
        nextionSetText("LT","");
      }else{
        nextionSetText("RN", readEEPROMName(locoNameBase + (editingID * (locoNameLen))));      //Get Loco Name from Integrated EEPROM
        nextionSetText("LT", readEEPROMName(locoTypeBase + (editingID * (locoTypeLen))));      //Get Loco Type from Integrated EEPROM
      }
      #if defined (LONG_NAMES)
        nextionSetText("FName", longLocoNames[selectedIDs[thNum][editingID]]);
      #endif
      nextionSetText("LN", String(readLocoRNum(editingID)));
      loadFunctions(LocoEditPage, editingID);
      break;
    }
    //******************************************************************************************************************************
    case LocosPage:
    {
      backupID = selectedIDs[thNum][activeSlot];       //Used by "Cancel" button
      locosDrawPage(locoStartID);
      break;
    }
    //****************************************************************************************************************************** 
    case AccPage:                                                                                                                   
    {
      if (accStartID == 0) nextionCommand("PageDn.pic=PAGEDNGREYED");
      accDrawPage(accStartID);
      break;
    }
    //******************************************************************************************************************************
    case AccEditPage: 
    {
      nextionSetText("ID", String(editingID + accIDBase));
      nextionSetText("A", String(readAccAddress(editingID)));
      if(readAccAddress(editingID) == 0) nextionSetText("N",""); 
      else nextionSetText("N", readEEPROMName(accNameBase + (editingID * (accNameLen))));
      if(readAccAddress(editingID) != 0)
      {
        nextionCommand(("ImageA.pic=" + String(readAccImage(editingID))).c_str());
        nextionCommand(("ImageB.pic=" + String((readAccImage(editingID))+1)).c_str());    //Add 1 for the partner image
        nextionCommand(("Test.pic=" + String(readAccImage(editingID))).c_str());
        nextionCommand(("Test.pic2=" + String((readAccImage(editingID))+1)).c_str());    //Add 1 for the partner image
        nextionSetValue("DCC",1);
      }else{
        nextionCommand("ImageA.pic=23");   //+ String(GREYED_BUTTON)).c_str());  //23
        nextionCommand("ImageB.pic=23");  //+ String(GREYED_BUTTON)).c_str());
        nextionCommand("Test.pic=23");  //" + String(GREYED_BUTTON)).c_str());
        nextionCommand("Test.pic2=23");  //" + String(GREYED_BUTTON)).c_str());
        nextionSetValue("DCC",1);                                       //Default to DCC
      }
      eMode = 0;
      nextionSetValue("Edit",0);
      break;
    }
    //******************************************************************************************************************************
    case RoutesPage:
    {
      if (routeStartID == 0) nextionCommand("PageDn.pic=PAGEDNGREYED");
      accSelectMode = false;
      eMode = 0;
      routeDrawPage(routeStartID);
      break;
    }
    //******************************************************************************************************************************
    case ProgramPage:
    {
      nextionSetValue("Main", 0);
      nextionSetValue("Prog", 0);
      nextionSetValue("Read", 0);
      nextionSetValue("Write", 0);
      nextionCommand("ADText.pco=" + String(GREY));
      nextionCommand("LName.pco=" + String(GREY));
      nextionCommand("AD.pco=" + String(GREY));
      nextionCommand("AD.bco=" + String(GREY));
      nextionSetText("AD", String(readLocoAddress(selectedIDs[thNum][activeSlot])));
      #if defined (LONG_NAMES)
        nextionSetText("LName", String(longLocoNames[selectedIDs[thNum][activeSlot]]));
      #endif
      nextionSetText("CVN", "");
      nextionSetText("CVV", "");
      nextionSetText("S0", "Select Main or Prog First");
      nextionSetText("S1", "");
      nextionSetText("S2", "");
      nextionSetText("S3", "");
      progMode = 0;
      break;
    }
    //******************************************************************************************************************************
    case ConfigPage:                                                                                //Done
    {
      nextionCommand("RN.val=" + String(readEEPROMByte(eeRNumEnabled)));
      nextionCommand("Stop.val=" + String(readEEPROMByte(eeLocoStopAll)));
      nextionCommand("PU.val=" + String(readEEPROMByte(eePUState)));
      nextionCommand("DCC.val=" + String(readEEPROMByte(eeDCCRefresh)));       //",String(readEEPROMByte(ee
      nextionCommand("Join.val=" + String(readEEPROMByte(eeJoinMode)));
//      nextionCommand("EX.pic=270");
      nextionSetText("n0", String(readEEPROMByte(eeThreshold)));
      nextionSetText("n1", String(readEEPROMByte(eeREIncrement)));
      nextionSetText("n2", String(readEEPROMByte(eeAccDelay)));
      nextionSetText("ExTex", "DCCEX Roster Not Supported");
      break;
    }
    //******************************************************************************************************************************
    case WiFiPage:                                                                            //In Progress
    { 
      #if defined WIFI
        nextionCommand("WiFiEN.val=" + String(WiFiEnabled));
        nextionSetText("SSID", ssid);                     //String(ssid));
        nextionSetText("Password", password);             //password);
        nextionSetText("HostIP", host);                   //host);
        nextionSetText("Port", String(port));             //String(port)); 
        if(WiFiEnabled == 0) 
        {
          nextionSetText("Progress","");
          nextionSetText("WiFiState", "WiFi Disconnected...");
          return;
        }
        nextionSetText("RSSI", String(WiFi.RSSI()));
        nextionSetText("Retries", String(readEEPROMByte(eeWiFiRetries)));
        nextionSetText("n3", String(readEEPROMByte(eeWiFiSeconds)));
        nextionSetText("Progress", "");
        if(WiFi.status() == WL_CONNECTED)
        {
          nextionSetText("WiFiState", wifiStatus);
        }else
        {
          nextionSetText("WiFiState", "WiFi Disconnected - Retry");
        }
      #endif
      break;
    }
    //******************************************************************************************************************************
    case SelectionPage:
    {
      Console.print("List Name Passed: ");
      Console.println(listName);
      fillSelection(listName);
      break;
    }
    //******************************************************************************************************************************
    case FunctionEditPage:
    {
      if(readLocoAddress(editingID) != 0)
      {
        nextionSetText("Na", readEEPROMName(locoNameBase + (editingID * (locoNameLen))));
        nextionSetText("Nb", readEEPROMName(locoTypeBase + (editingID * (locoTypeLen))));
      }else
      {
        nextionSetText("Na", "");
        nextionSetText("Nb", "");
      }
      nextionSetText("AD", String(readLocoAddress(editingID)));                                               //Load Address
      uint8_t funcNum = (readEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2)));
      nextionSetText("F", String(funcNum & 0x7F));   //Load Function Number
      uint8_t funcImage = 118;
      if(funcNum != 127) funcImage = readEEPROMByte((locoFuncBase + (editingID * 20)) + ((g_fSlot*2)+1));                      //Retrieve Image Number
      nextionSetValue("n1", funcImage);                                                               //Load hidden Image Number
      if((funcNum & 0x80) == 128) 
      {
        nextionSetValue("Type", 1);
        fType = 1;
      }else
      {
        nextionSetValue("Type", 0);
        fType = 0;
      }
      nextionCommand(("ImageA.pic=" + String(funcImage)).c_str());                                         //Load ImageA
      nextionCommand(("Test.pic=" + String(funcImage)).c_str());                                           //Load Test Image
      nextionCommand(("ImageB.pic=" + String(funcImage + 1)).c_str());                                     //Load ImageB
      break;
    }

    //******************************************************************************************************************************
    case UpdatePage:                                                                           //Still to Follow
    {
      nextionSetText("uSSID", ussid);
      nextionSetText("uPW", upassword);
      nextionSetText("TFTIP", tftHost);
      nextionSetText("TFTPort", String(tftPort));
      nextionSetText("TFTName", url);
      nextionSetValue("OTA", 0);
      nextionSetValue("Upload", 0);
      break;
    }
    //******************************************************************************************************************************
    case MenuPage:
    {
      switch(thNum)
      {
        case(0):
          nextionCommand("TH1.pic=" + String(TH1_BUTTON_ON));     //TH1_BUTTON_ON
          nextionCommand("TH2.pic=" + String(TH2_BUTTON_OFF));     //TH2_BUTTON_OFF
          nextionCommand("TH3.pic=" + String(TH3_BUTTON_OFF));     //TH3_BUTTON_OFF
          nextionCommand("TH4.pic=" + String(TH4_BUTTON_OFF));     //TH4_BUTTON_OFF
          break;
        case(1):
          nextionCommand("TH1.pic=" + String(TH1_BUTTON_OFF));
          nextionCommand("TH2.pic=" + String(TH2_BUTTON_ON));
          nextionCommand("TH3.pic=" + String(TH3_BUTTON_OFF));
          nextionCommand("TH4.pic=" + String(TH4_BUTTON_OFF));
          break;
        case(2):
          nextionCommand("TH1.pic=" + String(TH1_BUTTON_OFF));
          nextionCommand("TH2.pic=" + String(TH2_BUTTON_OFF));
          nextionCommand("TH3.pic=" + String(TH3_BUTTON_ON));
          nextionCommand("TH4.pic=" + String(TH4_BUTTON_OFF));
          break;
        case(3):
          nextionCommand("TH1.pic=" + String(TH1_BUTTON_OFF));
          nextionCommand("TH2.pic=" + String(TH2_BUTTON_OFF));
          nextionCommand("TH3.pic=" + String(TH3_BUTTON_OFF));
          nextionCommand("TH4.pic=" + String(TH4_BUTTON_ON));
          break;
        default:
          Console.println("Invalid Throttle Number");
      } 
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
  nextionPage = page;                       //Save new Page reference
  nextionCommand("page " + String(page));   
}
/*
 *************************************************************************************************************************
 * Set PAGEUP and PAGEDN Buttons
 *************************************************************************************************************************
*/
void setPageButtons(uint8_t startID, uint8_t rowsPerPage, uint8_t numRows)
{
  #if !defined NO_PAGING
    if (startID == 0)
    {
      nextionCommand("PageUp.pic=24");    // + String(PAGEUP)).c_str());     //24
      nextionCommand("PageUp.pic2=25");   // + String(PAGEUPON)).c_str());  //25
      nextionCommand(F("PageDn.pic=29"));    // + String(PAGEDNGREYED)).c_str());   //29
      nextionCommand(F("PageDn.pic2=29"));    // + String(PAGEDNGREYED)).c_str());    //29
    }else{   
      nextionCommand("PageDn.pic=27");      // + String(PAGEDN)).c_str());     //27
      nextionCommand("PageDn.pic2=28");     // + String(PAGEDNON)).c_str());  //28
    }
    if (startID < (numRows - rowsPerPage))
    {
      nextionCommand("PageUp.pic=24");   // + String(PAGEUP)).c_str());    //24
      nextionCommand("PageUp.pic2=25");   // + String(PAGEUPON)).c_str());  //25
    }else{
      nextionCommand("PageUp.pic=26");     //+ String(PAGEUPGREYED)).c_str());   //26
      nextionCommand("PageUp.pic2=26");    //+ String(PAGEUPGREYED)).c_str());  //26
    }
  #endif  
}
