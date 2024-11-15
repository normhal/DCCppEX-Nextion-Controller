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
 * Function Edit Page
 *************************************************************************************************************************
*/
void functionEditPage(uint8_t button)
{
  if(message.startsWith("FF"))
  {
    writeEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2), message.substring(2).toInt());
    return;
  }  
  if(message.startsWith("FI"))
  {
    writeEEPROMByte((locoFuncBase + (editingID * 20)) + ((g_fSlot*2)+1), message.substring(2).toInt());
    return;
  }
  switch(button)
  {
    case Done_Press:
    {
      if(deleteMode == 1)
      {
        writeEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2), 255);
      }
      deleteMode = 0;
      nextionSetValue("D", 0);
      initPage(LocoEditPage);
      break;
    }
    case Cancel_Press:
    {
      deleteMode = 0;
      nextionSetValue("D", 0);      //Turn off Delete Button
      initPage(LocoEditPage);
      break;
    }
    case Func_Latched:
    {
      uint8_t funcNumber = readEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2));
      funcNumber = (funcNumber & 0x7f);
      writeEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2), funcNumber);
      fType = LATCH;
      break;
    }
    case Func_Pulsed:
    {
      uint8_t funcNumber = readEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2));
      funcNumber = (funcNumber | 0x80);
      writeEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2), funcNumber);
      fType = PULSE;
      break;
    }
    case DELETE_OFF:
      deleteMode = 0;
      break;
    case DELETE_ON:
      deleteMode = 1;
      break;
    case TestButtonOn:
    {
      uint8_t funcNumber = readEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2));
      uint8_t funcImage = readEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2)+1);
      uint8_t newImage = toggleFunction((funcNumber & 0x7f), funcImage); 
//      Serial.print("Selected Image: ");
//      Serial.println(newImage);
      nextionCommand("Test.pic=" + String(newImage));
      break;
    }
    case TestButtonOff:
    {
      if(fType == PULSE)
      {
        uint8_t funcNumber = readEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2));
        uint8_t funcImage = readEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2)+1);
        uint8_t newImage = toggleFunction((funcNumber & 0x7f), funcImage);
        nextionCommand("Test.pic=" + String(newImage));
      }
      break;
    }
    default:
      break;
  }
}
