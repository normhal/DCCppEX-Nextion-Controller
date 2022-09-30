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
 * Process Routes Page
 * Notes:
 * A Route is a  6-element 2D array containing up to 6 accessory IDs and associated "qualifier"
 * - Each Route has its own ID ranging from 0 to 55 (56 routes)
 * - Each of the 6 Elements of the array (ID) has an accessory ID and an associated "qualifier" 
 *   byte - containing the desired "State" and the Accessory "Type" - current implementation is "State" only.
 * - the "Execution" of a single route involves stepping through the array from element 0 to the last (up to 6)
 *   and sending the command for that accessory to the Command Station
 * - The delay between execution "steps" is a programmable value
 * 
 *************************************************************************************************************************
*/
void routesPage(uint8_t button)
{
  switch(button)
  {
    case DoneButton:
      #if defined (ESP32) || (ESP8266)
        EEPROM.commit();
      #endif
      initPage(originPage);       //Return to either Throttle or Acc Page
      break;
    case AccButton:
      #if defined (ESP32) || (ESP8266)
        EEPROM.commit();
      #endif
      initPage(AccPage);
      break;
    case ThrottleButton:
      #if defined (ESP32) || (ESP8266)
        EEPROM.commit();
      #endif
      initPage(ThrottlePage);
      break;
    case EditButton:
    {
      if (myNextion.getComponentValue("Edit") == true)
        {
          editMode = true;
          eMode = 1;
        }else{
          editMode = false;
          eMode = 0;
        }
      break;
    }
    case PageUpButton:
    {
      if (routeStartID < (numRoutes - rowsPerNextionRoutePage)){
        routeStartID = routeStartID + rowsPerNextionRoutePage;     
        myNextion.sendCommand(("PageUp.pic=" + String(PageUpGreyed)).c_str());
        myNextion.sendCommand(("PageDn.pic=" + String(PageDn)).c_str());
      }else break;
      routeDrawPage(routeStartID);
      break;
    }
    case PageDownButton:
    {
      if (routeStartID >= rowsPerNextionRoutePage){
        routeStartID = routeStartID - (rowsPerNextionRoutePage);
        myNextion.sendCommand(("PageUp.pic=" + String(PageUp)).c_str());
        myNextion.sendCommand(("PageDn.pic=" + String(PageDnGreyed)).c_str());
      }
      else break;
      routeDrawPage(routeStartID);
      break;
    }
    default:
    {
      uint8_t slotNum = button-10;                  //Save the Nextion Slot being edited
      if(editMode == 1)
      {
        editingID = routeStartID + slotNum;         //Determine the action Route ID
        editMode = 2;                               //Change mode for AccPage processing
        eMode = 2;
        for(uint8_t i = 0; i < numAccs; i++) selectedAccs[i] = 255;             //Ensure no Accessories are flagged as Selected
        for(uint8_t i = 0; i < accessoriesPerRoute; i++) routeList[i][0]=255;   //Clear Route List for now
        accCount = 0;
        originPage = RoutesPage;
        initPage(AccPage);
        myNextion.setComponentValue("Select", 1);   //Turn on "Select" Mode on AccPage
        break;                                      //Leave the rest up to AccPage     
      }
      // EXECUTE THE ROUTE ID ***************************************************************************************
      //Got here to Execute the selected Route ID
      uint8_t selectedRoute = routeStartID + slotNum;
      uint16_t eeRouteAddress = routeListBase + (selectedRoute * accessoriesPerRoute);
      uint16_t eeRouteStates = routeStateBase + (selectedRoute * accessoriesPerRoute);
      for(uint8_t r = 0; r < accessoriesPerRoute; r++)
      {
        uint8_t accID = readEEPROMByte(eeRouteAddress + (r));
        uint16_t accAddr = readAccAddress(accID);
        if(accID != 255)
        {
          String CMD = "<a " + String(accAddr) + " " + String(readEEPROMByte(eeRouteStates + (r))) + ">";
          sendCMD(CMD);
          accStates[accID] = readEEPROMByte(eeRouteStates + r);
          delay((readEEPROMByte(eeAccDelay))*10);       //Values are in 10ms increments
        }
      }
      break;
    }
  }
}
/*
 *************************************************************************************************************************
 * Draw a Page full of Routes
 * Displaying a Page of Routes involves the Following:
 * - Starting with the current routeStartID
 * - Updating the Route "Button" with the Route's Name
 *   - if the Name is Blank, move onto the next Route ID
 * - Reading the contents of that Route ID from EEPROM
 * - Filling the Route's details into the Route Detail field
 * - repeating the the remaining 5 slots.
 *************************************************************************************************************************
*/
void routeDrawPage(uint8_t routeStartID)
{
  for(uint8_t r = 0; r < rowsPerNextionRoutePage; r++)       //Prepare to draw "rowsPerNextionRoutePage" rows
  {
    String nameSlot = "r" + String(r);                   //route Slot
    String detailSlot1 = "l" + String(r);
    String detailSlot2 = "t" + String(r);
    uint16_t eeRouteAddress = routeListBase + ((routeStartID + r) * accessoriesPerRoute);
    uint16_t eeRouteStates = routeStateBase + ((routeStartID + r) * accessoriesPerRoute);
    myNextion.setComponentText(nameSlot, "R" + String(routeStartID + r + 1));       //Insert the Route Number on Nextion Button
    String routeDetail1 = "";                                                        // Start with a blank  Detail String
    String routeDetail2 = "";                                                        // Start with a blank  Detail String
    uint8_t entryNum = 0;
    for(uint8_t d = 0; d < accessoriesPerRoute; d++)
    {
      if(readEEPROMByte(eeRouteAddress + d) != 255)
      {
      uint8_t accID = readEEPROMByte(eeRouteAddress + d);
      uint16_t accAddr = readAccAddress(accID);
      if(entryNum<3) routeDetail1 = (routeDetail1 + String(accAddr) + ":" + String(readEEPROMByte(eeRouteStates + d)) + "   ");
      else routeDetail2 = (routeDetail2 + String(accAddr) + ":" + String(readEEPROMByte(eeRouteStates + d)) + "   ");
      entryNum++;
      }
    }
    myNextion.setComponentText(detailSlot1, String(routeDetail1));
    myNextion.setComponentText(detailSlot2, String(routeDetail2));
  }
  setPageButtons(routeStartID, rowsPerNextionRoutePage, numRoutes);
}
