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
 * Process Accessory Page
 *************************************************************************************************************************
 */
void accPage(uint8_t button)
{
  #if !defined NO_ACCESSORIES
  switch(button)
  {
    case DoneButton:
    {
      if(eMode == 2)        //Route Select Mode Active
      {
        for(uint8_t i = 0; i<accessoriesPerRoute; i++)
        {
          writeEEPROMByte((routeListBase + (editingID*accessoriesPerRoute) + i), routeList[i][0]);
          writeEEPROMByte((routeStateBase + (editingID*accessoriesPerRoute) + i), routeList[i][1]);
        }
        eMode = 0;
        nextionCommand("Select.pic=144");  //SELECT_OFF = 144
      }
      initPage(AccReturnPage);
      break;
    }
    case RoutesButton:
      RoutesReturnPage = AccPage;
      eMode = 0;
      nextionCommand("Select.pic=144");  //SELECT_OFF = 144
      initPage(RoutesPage);
      break;
    case SelectButton:                      //This button sets "Select" state Off and deletes any route in progress
    {
      if(accSelectMode == false)                  //Select State currently OFF and Button Pressed, so turn if ON
      {
        accSelectMode = true;
        nextionCommand("Select.pic=145");      //SELECT_ON = 145
      }else{                                      //Select State currently ON and Button Pressed, so turn if OFF
        accSelectMode = false;                    //Only turn Route Select Mode OFF
        nextionCommand("Select.pic=144");      //SELECT_OFF = 144
        for(uint8_t i = 0; i < accessoriesPerPage; i++)       //Reset all the YELLOW marked Accessories
        {
          String nSlot = ("n" + String(i) + ".bco=" + String(GREY));
          String aSlot = ("a" + String(i) + ".bco=" + String(GREY));
          nextionCommand(nSlot.c_str());
          nextionCommand(aSlot.c_str());
        }
        for(uint8_t i=0; i < accessoriesPerRoute; i++)    //Clear the accumulated list of Accessories
        {
          routeList[i][0] = 255;
          routeList[i][1] = 0;
        }
      }
      break;
    }
    case EditButton:                      //Is this needed? Mode set by Dual State Button
    {
      switch(eMode)
      {
        case (0):
          eMode = 1;
          nextionSetValue("Edit",1);
          break;
        case (1):
          eMode = 0;
          nextionSetValue("Edit",0);
          break;
        case (2):
          nextionSetValue("Edit",0);
          break;       
        default:
          break;
      }
      break;
    }
    case PageUpButton:
    {
      if(accStartID < (numAccs - accessoriesPerPage))
      {
        accStartID = accStartID + accessoriesPerPage;
        nextionCommand("PageDn.pic=27");    // + String(PAGEDN));    //27
      }else{
        nextionCommand("PageUp.pic=26");      // + String(PAGEUPGREYED)); //26
      }
      accDrawPage(accStartID);
      break;
    }
    case PageDownButton:
    {
      if (accStartID >= accessoriesPerPage) 
      {
        accStartID = accStartID - (accessoriesPerPage);
        nextionCommand("PageUp.pic=24");    // + String(PAGEUP));  ///24
      }else{
        nextionCommand("PageDn.pic=29");      // + String(PAGEDNGREYED));    //29
      }
      accDrawPage(accStartID);
      break;
    }
    default:                              //Accessory Buttons get processed here - buttons 50 to 61
    {
      if((button >= SlotCStart && button < (SlotCStart + accessoriesPerPage)) || (button >= SlotBStart && button < (SlotBStart + accessoriesPerPage)))     //
      {
        uint8_t accSlot;
        if(button >= SlotCStart && button < (SlotCStart + accessoriesPerPage)) accSlot = button - SlotCStart;    //Convert button value to Slot number
        if(button >= SlotBStart && button < (SlotBStart + accessoriesPerPage)) accSlot = button - SlotBStart;    //Convert button value to Slot number
        switch (eMode)
        {
          case (0):           //Normal Accessory Operation Mode
          {
            if(readAccImage(accStartID + accSlot) != BLANK)
            {
              accStates[accStartID + accSlot] = !accStates[accStartID + accSlot];                 //Toggle the stored State
              String iSlot = "i" + String(accSlot);
              String nextionString = "";
              if(accStates[accStartID + accSlot] == false)
              {
                nextionString = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot));      // + accStates[accStartID + accSlot]));
              }else{
                nextionString = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot) + 1);              
              }
              nextionCommand(nextionString.c_str());
              doDCCAcc(accStartID+accSlot);
            }
            break;
          }
          case (1):         //Edit Mode active
          {
            editingID = accStartID + accSlot;          //The result is the actual ID of the Slot selected
            initPage(AccEditPage);
            break;
          }
          case (2):       //Select Mode Active
          {
            if(button >= SlotCStart && button < (SlotCStart + accessoriesPerPage))     //These are the Accessory HotSpots
            {
              if(selectedAccs[accStartID + accSlot] != (accStartID + accSlot))
              {
                String nSlot = ("n" + String(accSlot) + ".bco=" + String(YELLOW));   //Change the colour to Yellow
                String aSlot = ("a" + String(accSlot) + ".bco=" + String(YELLOW));   //Change the colour to Yellow
                nextionCommand(nSlot.c_str());
                nextionCommand(aSlot.c_str());
                selectedAccs[accStartID + accSlot] = (accStartID + accSlot);    //set this AccID as Selected]
                if (accCount < accessoriesPerRoute)
                {
                  routeList[accCount][0] = (accStartID + accSlot);              //Route element ID
                  routeList[accCount][1] = accStates[accStartID + accSlot];     //Route element state
                  accCount = accCount + 1;  
                }
                break;
              }else
              {
                String nSlot = ("n" + String(accSlot) + ".bco=" + String(GREY));
                String aSlot = ("a" + String(accSlot) + ".bco=" + String(GREY));
                nextionCommand(nSlot.c_str());
                nextionCommand(aSlot.c_str());
                routeList[accCount][0] = BLANK;
                if(accCount != 0) accCount = accCount - 1;
                selectedAccs[accStartID + accSlot] = BLANK;    //set this AccID as Deselected
                break;
              }
            }
            if(button >= SlotBStart && button < (SlotBStart + accessoriesPerPage))     //These are the Accessory Images buttons ONLY processed in Select Mode
            {
              if(selectedAccs[accStartID + accSlot] != (accStartID + accSlot))          //Allow State changes if NOT selected
              { 
                accStates[accStartID + accSlot] = !accStates[accStartID + accSlot];    
                String iSlot = "i" + String(accSlot);
                String nextionString = "";
                if(accStates[accStartID + accSlot] == false)
                {
                  nextionString = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot));      // + accStates[accStartID + accSlot]));
                }else{
                  nextionString = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot) + 1);              
                }
                nextionCommand(nextionString.c_str());
                break;
              }
              break;
            }
          }
          case (3):
          {
           break;
          }
        }
      }
      break;
    }
  }
  #endif
}

/*
 *************************************************************************************************************************
 * Draw a Page Full of Accessories
 *************************************************************************************************************************
*/
void accDrawPage(uint8_t accStartID) 
{
  #if !defined NO_ACCESSORIES
    uint8_t slotNum = 0;                                                                               //Slot being work with
    for(uint8_t accID = accStartID; accID < (accStartID + accessoriesPerPage); accID++)                      //Name, Address and Image must be updated
    {
      String aSlot = "a"+String(slotNum);                                                        //(a)slotNum=Nextion Address Slot index
      String iSlot = "i"+String(slotNum);                                                        //(i)slotNum=Nextion Image Slot index
      String nSlot = "n"+String(slotNum);                                                        //(i)slotNum=Nextion Image Slot index
      if (readAccAddress(accID)!=0)                                                                    //Read from local EEPROM >0 means occupied
      { 
        #if defined ESP
          nextionSetText("n" + String(slotNum), readEEPROMName(accNameBase + (accID * (accNameLen))));
        #endif
        #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
          nextionSetText("n" + String(slotNum), readEEPROMName(accNameBase + (accID * (accNameLen))));
        #endif
        #if defined AVR_UNO && defined USE_NEXTION_EEPROM   
          nextionCommand(("repo n" + String(slotNum) + ".txt," + String(accNameBase + (accID * (accNameLen)))).c_str());  //Transfer the Accessory Name from Nextion EEPROM
        #endif
        nextionSetText(aSlot, String(readAccAddress(accID)));                             //Set the Accessory Address
        nextionCommand((iSlot + ".pic=" + String(readAccImage(accID) + accStates[accID])).c_str());               //Set the Accessory Image with State
      }else
      {
        nextionSetText(aSlot,"");
        nextionSetText(nSlot,"");
        nextionCommand(iSlot + ".pic=" + String(BLANK));
      }
      slotNum++;                                                                                  //Increment the Nextion address Slot ID
    }                                                                                             //Note: accStartID retains its initial Value
    setPageButtons(accStartID, accessoriesPerPage, numAccs);
    if(eMode == 0) nextionSetValue(F("Edit"),0);
  #endif
}
