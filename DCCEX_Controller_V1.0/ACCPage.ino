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
      }
      initPage(originPage);
      break;
    }
    case RoutesButton:
      originPage = AccPage;
      initPage(RoutesPage);
      break;
    case SelectButton:                      //Here for Routes
    {
      if (myNextion.getComponentValue("Select") == true)
        {
          accSelectMode = true;
          eMode = 2;
        }else{
          accSelectMode = false;
          eMode = 0;
          //Reset all the Selected Items
          for(uint8_t i = 0; i < accRowsPerPage; i++)
          {
            String nSlot = ("n" + String(i) + ".bco=" + String(GREY));
            String aSlot = ("a" + String(i) + ".bco=" + String(GREY));
            myNextion.sendCommand(nSlot.c_str());
            myNextion.sendCommand(aSlot.c_str());
          }
          for(uint8_t i=0; i < accessoriesPerRoute; i++)
          {
            routeList[i][0] = 255;
            routeList[i][1] = 0;
          }
        }
      break;
    }
    case EditButton:                      //Is this needed? Mode set by Dual State Button
    {
      if (myNextion.getComponentValue("Edit") == true)
        {
          accEditMode = true;
          eMode = 1;
        }else{
          accEditMode = false;
          eMode = 0;
        }
      break;
    }
    case PageUpButton:
    {
      if (accStartID < (numAccs - accRowsPerNextionPage))
      {
        accStartID = accStartID + accRowsPerNextionPage;
        myNextion.sendCommand(("PageDn.pic=" + String(PageDn)).c_str());
      }else{
        myNextion.sendCommand(("PageUp.pic=" + String(PageUpGreyed)).c_str());
      }
      accDrawPage(accStartID);
      break;
    }
    case PageDownButton:
    {
      if (accStartID >= accRowsPerNextionPage) 
      {
        accStartID = accStartID - (accRowsPerNextionPage);
        myNextion.sendCommand(("PageUp.pic=" + String(PageUp)).c_str());
      }else{
        myNextion.sendCommand(("PageDn.pic=" + String(PageDnGreyed)).c_str());
      }
      accDrawPage(accStartID);
      break;
    }
    default:                              //Accessory Buttons get processed here - buttons 10 to 21
    {
      if((button >=10 && button < 22) || (button >=50 && button < 62))     //
      {
        uint8_t accSlot;
        if(button >=10 && button < 22) accSlot = button - 10;    //Convert button value to Slot number
        if(button >=50 && button < 62) accSlot = button - 50;    //Convert button value to Slot number
        switch (eMode)
        {
          case (0):           //Normal Accessory Operation Mode
          {
            if(readAccImage(accStartID + accSlot) != BLANK)
            {
              accStates[accStartID + accSlot] = !accStates[accStartID + accSlot];                 //Toggle the stored State
              String iSlot = "i" + String(accSlot);
              String nextionCommand = "";
              if(accStates[accStartID + accSlot] == false)
              {
                nextionCommand = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot));      // + accStates[accStartID + accSlot]));
              }else{
                nextionCommand = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot) + 1);              
              }
              myNextion.sendCommand(nextionCommand.c_str());
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
            if(button >=10 && button < 22)     //These are the Accessory HotSpots
            {
              if(selectedAccs[accStartID + accSlot] != (accStartID + accSlot))
              {
                String nSlot = ("n" + String(accSlot) + ".bco=" + String(YELLOW));   //Change the colour to Yellow
                String aSlot = ("a" + String(accSlot) + ".bco=" + String(YELLOW));   //Change the colour to Yellow
                myNextion.sendCommand(nSlot.c_str());
                myNextion.sendCommand(aSlot.c_str());
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
                myNextion.sendCommand(nSlot.c_str());
                myNextion.sendCommand(aSlot.c_str());
                routeList[accCount][0] = BLANK;
                if(accCount != 0) accCount = accCount - 1;
                selectedAccs[accStartID + accSlot] = BLANK;    //set this AccID as Deselected
                break;
              }
            }
            if(button >=50 && button < 62)     //These are the Accessory Images buttons ONLY processed in Select Mode
            {
              if(selectedAccs[accStartID + accSlot] != (accStartID + accSlot))          //Allow State changes if NOT selected
              { 
                accStates[accStartID + accSlot] = !accStates[accStartID + accSlot];    
                String iSlot = "i" + String(accSlot);
                String nextionCommand = "";
                if(accStates[accStartID + accSlot] == false)
                {
                  nextionCommand = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot));      // + accStates[accStartID + accSlot]));
                }else{
                  nextionCommand = iSlot + ".pic=" + String(readAccImage(accStartID + accSlot) + 1);              
                }
                myNextion.sendCommand(nextionCommand.c_str());
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
}

/*
 *************************************************************************************************************************
 * Draw a Page Full of Accessories
 *************************************************************************************************************************
*/
void accDrawPage(uint8_t accStartID) 
{
  uint8_t slotNum = 0;                                                                               //Slot being work with
  for(uint8_t accID = accStartID; accID < (accStartID + accRowsPerNextionPage); accID++)                      //Name, Address and Image must be updated
  {
    String aSlot = "a"+String(slotNum);                                                        //(a)slotNum=Nextion Address Slot index
    String iSlot = "i"+String(slotNum);                                                        //(i)slotNum=Nextion Image Slot index
    String nSlot = "n"+String(slotNum);                                                        //(i)slotNum=Nextion Image Slot index
    if (readAccAddress(accID)!=0)                                                                    //Read from local EEPROM >0 means occupied
    { 
      #if defined ESP
        myNextion.setComponentText("n" + String(slotNum), readEEPROMName(accNameBase + (accID * (accNameLen+1))));
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        myNextion.setComponentText("n" + String(slotNum), readEEPROMName(accNameBase + (accID * (accNameLen+1))));
      #endif
      #if defined AVR_UNO && defined USE_NEXTION_EEPROM   
        myNextion.sendCommand(("repo n" + String(slotNum) + ".txt," + String(accNameBase + (accID * (accNameLen+1)))).c_str());  //Transfer the Accessory Name from Nextion EEPROM
      #endif
      myNextion.setComponentText(aSlot, String(readAccAddress(accID)));                             //Set the Accessory Address
      myNextion.sendCommand((iSlot + ".pic=" + String(readAccImage(accID) + accStates[accID])).c_str());               //Set the Accessory Image with State
    }else
    {
      myNextion.setComponentText(aSlot,"");
      myNextion.setComponentText(nSlot,"");
      String nextionCommand = iSlot + ".pic=" + String(BLANK);
      myNextion.sendCommand(nextionCommand.c_str());
    }
    slotNum++;                                                                                  //Increment the Nextion address Slot ID
  }                                                                                             //Note: accStartID retains its initial Value
  setPageButtons(accStartID, accRowsPerNextionPage, numAccs);
}
