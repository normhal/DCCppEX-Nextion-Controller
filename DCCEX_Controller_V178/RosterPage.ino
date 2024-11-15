// Roster Page
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
 * Process Locos Page
 *************************************************************************************************************************
 */
void rosterPage(uint8_t button)
{
  button = message.toInt();
  switch(button)
  {
    case Cancel_Press:
    {
      selectedIDs[thNum][activeSlot[thNum]] = backupID;
      initPage(LocosReturnPage);                       //Always return to the Throttle page
      break;
    }
    case SelectButton:
    {
      if(readLocoAddress(selectedLocoID) == 0) return; 
      selectedIDs[thNum][activeSlot[thNum]] = selectedLocoID;   //selectedLocoID = Actual LocoID Being worked on
      initPage(ThrottlePage);
      break;
    }
    case EditButtonOn:
    {
      LocoEditReturnPage = RosterPage;                        //originPage will be either MenuPage or ThrottlePage
      editingID = selectedLocoID;                 //selectedLocoID set up previously
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
      if(button >= SlotAStart && button < (SlotAStart + locosPerPage))                 //In this case the button number is the Locos Slot number (range 0 to 9)
      {
        button = button-SlotAStart;               //Convert to be the actual slot number
        selectedLocoID = locoStartID + button;    //This identifies the actual Locos array element
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
    nextionCommand("l" + String(r) + ".bco=50712");
    nextionCommand("n" + String(r) + ".bco=50712");
    nextionCommand("t" + String(r) + ".bco=50712");
    nextionCommand("r" + String(r) + ".bco=50712");
    nextionCommand("a" + String(r) + ".bco=50712");
    if (readLocoAddress(startID+r)!=0)
    {
      #if defined (LONG_NAMES)
        nextionSetText("l" + String(r), longLocoNames[startID+r]);
      #endif
      nextionSetText("n" + String(r), readEEPROMName(locoNameBase + ((startID+r) * (locoNameLen))));
      nextionSetText("t" + String(r), readEEPROMName(locoTypeBase + ((startID+r) * (locoTypeLen))));
      nextionSetText("r" + String(r), String(readLocoRNum(startID+r)));
      nextionSetText("a" + String(r), String(readLocoAddress(startID+r)));
    }else
    {
      nextionSetText("l" + String(r),"");
      nextionSetText("n" + String(r),"");
      nextionSetText("t" + String(r),""); 
      nextionSetText("r" + String(r),"");
      nextionSetText("a" + String(r),"");
    }
  }
  setPageButtons(startID, rowsPerNextionRosterPage, numLocos);
}
