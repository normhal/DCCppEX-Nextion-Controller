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
 * Loco Edit Page Routines
 *************************************************************************************************************************
 ***************************************************
 * Process Loco Edit Page
 *************************************************
*/
void locoEditPage(uint8_t button)
{
  button = message.toInt();
  switch(button)
  {
    case CancelButton:
    {
      initPage(ThrottlePage);
      break;
    }
    case DoneButton:
    {
      eMode = 0;              //Turn off Edit Mode
      nextionCommand("Edit.pic=164");           // + String(EDIT_OFF)); 164
      wait(5);                                  //Wait for Nextion
      String tempStr = nextionGetText("RA");
      if(tempStr == "0") break;
      writeLocoAddress(editingID, tempStr.toInt());
      wait(5);                                  //Wait for Nextion
      tempStr = nextionGetText("RRN");
      writeLocoRNum(editingID, tempStr.toInt());
      #if defined ESP
        tempStr = nextionGetText("RN");
        writeEEPROMName((locoNameBase + (editingID * (locoNameLen))), tempStr);
        tempStr = nextionGetText("RT");
        writeEEPROMName((locoTypeBase + (editingID * (locoTypeLen))), tempStr);
        EEPROM.commit();
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        tempStr = nextionGetText("RN");
        writeEEPROMName((locoNameBase + (editingID * (locoNameLen))), tempStr);
        tempStr = nextionGetText("RT");
        writeEEPROMName((locoTypeBase + (editingID * (locoTypeLen))), tempStr);
      #endif
      #if defined AVR_UNO && defined USE_NEXTION_EEPROM
        nextionCommand(("wepo Na.txt," + String(locoNameBase + (editingID * (locoNameLen)))).c_str());
        wait(5);
        nextionCommand(("wepo Nb.txt," + String(locoTypeBase + (editingID * (locoTypeLen)))).c_str());
        wait(5);
      #endif
      initPage(originPage);
      break;
    }
    case DeleteButton:
      if(eMode == 1)
      {
        writeLocoAddress(editingID, 0);
        writeLocoRNum(editingID, 0);
        writeEEPROMName(editingID, "");
        for(uint8_t i=0; i<10; i++)
        {
          writeEEPROMByte((locoFuncBase + (editingID*20)) + (i*2), 127);
          writeEEPROMByte((locoFuncBase + (editingID*20)) + ((i*2)+1), BLANK);
        }
      }
      eMode = 0;              //Turn off Edit Mode
      initPage(originPage);
      break;
    case ProgramButton:
    {
      originPage = LocoEditPage;
      initPage(ProgramPage);
    }
    case EditButton:                      //
    {
      if(eMode == false)
      {
        eMode = 1;
        nextionCommand("Edit.pic=165"); // + String(EDIT_ON));
      }else{
        eMode = 0;
        nextionCommand("Edit.pic=164"); // + String(EDIT_OFF));
      }
      break;
    }
    default:
    {
      if(button >= 240 && button < 250)        //This comes from the IMAGE Slot which was pressed (200 to 209)
      {
        fSlot = button - 240;
        if(eMode == 1)
        {
          initPage(FunctionEditPage);
          return;
        }
      }
    }
  }
}
