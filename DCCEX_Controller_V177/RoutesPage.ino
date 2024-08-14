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
    case Done_Press:
      accSelectMode = false;
      eMode = 0;
      if(!EEPROM.commit()) Console.println("EEPROM.commit Failed");
      initPage(RoutesReturnPage);       //Return to either Throttle or Acc Page
      break;
    case AccButton:
//        if(!EEPROM.commit()) Console.println("EEPROM.commit Failed");    //nkh?
        AccReturnPage = RoutesPage;
        initPage(AccPage);
        break;
      case ThrottleButton:
        if(!EEPROM.commit()) Console.println("EEPROM.commit Failed");
        initPage(ThrottlePage);
        break;
      case EditButtonOn:                      //Is this needed? Mode set by Dual State Button
      {
        if(eMode == false)
        {
          accSelectMode = true;
          eMode = 1;
          nextionCommand("Edit.pic=165");    // + String(EDIT_ON));    //165
        }else{
          accSelectMode = false;
          eMode = 0;
          nextionCommand("Edit.pic=164");    // + String(EDIT_OFF));   //164
        }
        break;
      }
      case PageUpButton:
      {
        if (routeStartID < (numRoutes - rowsPerNextionRoutePage))
        {
          routeStartID = routeStartID + rowsPerNextionRoutePage;     
          nextionCommand("PageUp.pic=26");   //+ String(PAGEUPGREYED)).c_str());     //26
          nextionCommand("PageDn.pic=27");    // + String(PAGEDN)).c_str());          //27
        }else break;
        routeDrawPage(routeStartID);
        break;
      }
      case PageDownButton:
      {
        if (routeStartID >= rowsPerNextionRoutePage){
          routeStartID = routeStartID - (rowsPerNextionRoutePage);
          nextionCommand("PageUp.pic=24");   // + String(PAGEUP)).c_str());         //24
          nextionCommand("PageDn.pic=29");   // + String(PAGEDNGREYED)).c_str());   //29
        }
        else break;
        routeDrawPage(routeStartID);
        break;
      }
      default:
      { 
        if(button < SlotBStart || (button > SlotBStart + routesPerPage)) break;           //filter out any false button values
        uint8_t slotNum = button - SlotBStart;                  //Save the Nextion Slot being edited
        if(eMode == 1)
        {
          editingID = routeStartID + slotNum;         //Determine the action Route ID
          accSelectMode = 2;                               //Change mode for AccPage processing
          eMode = 2;
          for(uint8_t i = 0; i < numAccs; i++) selectedAccs[i] = 255;             //Ensure no Accessories are flagged as Selected
          for(uint8_t i = 0; i < accessoriesPerRoute; i++) routeList[i][0]=255;   //Clear Route List for now
          accCount = 0;
          AccReturnPage = RoutesPage;
          initPage(AccPage);
          nextionCommand("Select.pic=145");  //SELECT_ON = 145
          break;                                      //Leave the rest up to AccPage     
        }
        // EXECUTE THE ROUTE ID ***************************************************************************************
        //Got here to Execute the selected Route ID
        uint8_t selectedRoute = routeStartID + slotNum;
        uint16_t eeRouteAddress = routeListBase + (selectedRoute * accessoriesPerRoute);
        uint16_t eeRouteStates = routeStateBase + (selectedRoute * accessoriesPerRoute);
        //Turn the Button "ON"
        nextionCommand("r" + String(slotNum) + ".pic=" + String(ROUTE_ON));    //pic "7"
        for(uint8_t r = 0; r < accessoriesPerRoute; r++)
        {
          uint8_t accID = readEEPROMByte(eeRouteAddress + (r));
          uint16_t accAddr = readAccAddress(accID);
          if(accID != 255)
          {
            String CMD = "<a " + String(accAddr) + " " + String(readEEPROMByte(eeRouteStates + (r))) + ">";
            sendCMD(CMD);
            accStates[accID] = readEEPROMByte(eeRouteStates + r);
            wait((readEEPROMByte(eeAccDelay))*10);       //Values are in 10ms increments
          }
        }
        nextionCommand("r" + String(slotNum) + ".pic=" + String(ROUTE_OFF));    //pic "6"
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
    String detailSlot1 = "k" + String(r);
    String detailSlot2 = "l" + String(r);
    uint16_t eeRouteAddress = routeListBase + ((routeStartID + r) * accessoriesPerRoute);
    uint16_t eeRouteStates = routeStateBase + ((routeStartID + r) * accessoriesPerRoute);
    nextionSetText(nameSlot, "R" + String(routeStartID + r + 1));       //Insert the Route Number on Nextion Button
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
    nextionSetText(detailSlot1, String(routeDetail1));
    nextionSetText(detailSlot2, String(routeDetail2));
  }
  setPageButtons(routeStartID, rowsPerNextionRoutePage, numRoutes);
}
