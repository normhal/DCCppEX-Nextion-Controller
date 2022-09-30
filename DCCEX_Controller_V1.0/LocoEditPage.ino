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
      if(myNextion.getComponentValue("A") == 0)
      {
        myNextion.setComponentText("Info", "Address must NOT be Zero");
        break;
      }
      writeLocoAddress(editingID, myNextion.getComponentValue("A"));
      writeLocoRNum(editingID,myNextion.getComponentValue("RN"));
//      delay(10);
      #if defined ESP
        writeEEPROMName((locoNameBase + (editingID * (locoNameLen+1))), String(myNextion.getComponentText("N", NEXWAIT) + '\0'));
        EEPROM.commit();
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        writeEEPROMName((locoNameBase + (editingID * (locoNameLen+1))), String(myNextion.getComponentText("N", NEXWAIT) + '\0'));
      #endif
      #if defined AVR_UNO && defined USE_NEXTION_EEPROM
        myNextion.sendCommand(("wepo N.txt," + String(locoNameBase + (editingID * (locoNameLen+1)))).c_str());
        delay(5);
      #endif
      eMode = 0;              //Turn off Edit Mode
      initPage(originPage);
      break;
    }
    case DeleteButton:
      if(eMode == 1)
      {
        writeLocoAddress(editingID, 0);
        Serial.println(F("Check Point 2"));
        writeLocoRNum(editingID, 0);
        writeEEPROMName(editingID, "");
        for(uint8_t i=0; i<10; i++)
        {
          writeEEPROMByte((locoFuncBase + (editingID*20)) + (i*2), 255);
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
      if (myNextion.getComponentValue("Edit") == true)
      {
        eMode = 1;
      }else{
        eMode = 0;
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
          writeLocoAddress(editingID, myNextion.getComponentValue("A"));    //Save before Leaving Page
          writeLocoRNum(editingID,myNextion.getComponentValue("RN"));       //Save before Leaving Page
      #if defined ESP
        writeEEPROMName((locoNameBase + (editingID * (locoNameLen+1))), String(myNextion.getComponentText("N", NEXWAIT) + '\0'));
        EEPROM.commit();
      #endif
      #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
        writeEEPROMName((locoNameBase + (editingID * (locoNameLen+1))), String(myNextion.getComponentText("N", NEXWAIT) + '\0'));
      #endif
      #if defined AVR_UNO && defined USE_NEXTION_EEPROM
        myNextion.sendCommand(("wepo N.txt," + String(locoNameBase + (editingID * (locoNameLen+1)))).c_str());
        delay(5);
      #endif
          initPage(FunctionEditPage);
          return;
        }
      }
    }
  }
}
