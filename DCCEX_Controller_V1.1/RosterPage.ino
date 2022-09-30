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
 * Process Roster Page
 *************************************************************************************************************************
 */
void rosterPage(uint8_t button)
{
  button = message.toInt();
  switch(button)
  {
    case CancelButton:
    {
      selectedIDs[activeSlot] = backupID;
      initPage(ThrottlePage);                       //Always return to the Throttle page
      break;
    }
    case SelectButton:
    {
      if(readLocoAddress(selectedRosterID) == 0) return; 
      selectedIDs[activeSlot] = selectedRosterID;   //selectedRosterID = Actual LocoID Being worked on
      uint8_t c = 0;
      for (uint16_t s = eeSelIDs; s < (eeSelIDs+numLocoSlots); s++)   //Save the updated SelectedID List
      {
        writeEEPROMByte(s, selectedIDs[c]);
        c++;
      }
      #if defined ESP
        EEPROM.commit();
      #endif
      initPage(ThrottlePage);
      break;
    }
    case EditButton:
    {
      originPage = RosterPage;                        //originPage will be either MenuPage or ThrottlePage
      editingID = selectedRosterID;                 //selectedRosterID set up previously
      initPage(LocoEditPage);
      break;
    }
    case PageUpButton:                              //Increase locoStartID by 10
    {
      if (locoStartID < (numLocos - rowsPerNextionRosterPage)) 
      {
        locoStartID = locoStartID + rowsPerNextionRosterPage;     
      }
      rosterDrawPage(locoStartID);
      break;
    }
    case PageDownButton:                            //Decrease locoStartID by 10
    {
      if (locoStartID >= rowsPerNextionRosterPage) 
      {
        locoStartID = locoStartID - (rowsPerNextionRosterPage);
      }
      rosterDrawPage(locoStartID);
      break;
    }
    default:                                        //Anything else is not an action, but must be dealt with
    {
      if(button >=10 && button <20)                 //In this case the button number is the Roster Slot number (range 0 to 9)
      {
        button = button-10;                         //Convert to be the actual slot number
        selectedRosterID = locoStartID + button;    //This identifies the actual Roster array element
      }
      break;
    }
  }
}
/*
 *************************************************************************************************************************
 * Draw a Page Full of Locos 
 *************************************************************************************************************************
*/
void rosterDrawPage(uint8_t startID)
{
  for(uint8_t r = 0; r < rowsPerNextionRosterPage; r++)
  {
    if (readLocoAddress(startID+r)!=0){
      nextionSetText("a"+String(r), String(readLocoAddress(startID+r)));
      nextionSetText("r"+String(r), String(readLocoRNum(startID+r)));
      #if defined ESP
        nextionSetText("n" + String(r), readEEPROMName(locoNameBase + ((startID+r) * (locoNameLen))));
        nextionSetText("t" + String(r), readEEPROMName(locoTypeBase + ((startID+r) * (locoTypeLen))));
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        nextionSetText("n" + String(r), readEEPROMName(locoNameBase + ((startID+r) * (locoNameLen))));
        nextionSetText("t" + String(r), readEEPROMName(locoTypeBase + ((startID+r) * (locoTypeLen))));
      #endif

      #if defined AVR_UNO && defined USE_NEXTION_EEPROM
        nextionCommand(("repo n" + String(r) + ".txt," + String((startID+r) * (locoNameLen))).c_str());
        nextionCommand(("repo t" + String(r) + ".txt," + String((startID+r) * (locoTypeLen))).c_str());
      #endif
    }else{
      nextionSetText("a"+String(r),"");
      nextionSetText("r"+String(r),"");
      nextionSetText("n"+String(r),"");
      #if defined ESP
        nextionSetText("t"+String(r),"");
      #endif
    }
  }
  setPageButtons(startID, rowsPerNextionRosterPage, numLocos);
}
