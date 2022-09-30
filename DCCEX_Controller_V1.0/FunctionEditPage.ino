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
  switch(button)
  {
    case DoneButton:
    {
      uint8_t funcNumber = myNextion.getComponentValue("F");
      if(myNextion.getComponentValue("Type") == 1) funcNumber = (funcNumber | 0x80);
      uint8_t funcImage = myNextion.getComponentValue("n1");
      writeEEPROMByte((locoFuncBase + (editingID * 20)) + (fSlot*2), funcNumber);        //Store the Function Number
      writeEEPROMByte((locoFuncBase + (editingID * 20)) + ((fSlot*2)+1), funcImage);     //Store the Image Number - each ID uses 20 bytes for functions
      initPage(LocoEditPage);
      break;
    }
    case CancelButton:
    {
      initPage(LocoEditPage);
      break;
    }
    case TestButtonOn:
    {
      myNextion.sendCommand(("Test.pic=" + String(toggleFunction(myNextion.getComponentValue("F"), 
      myNextion.getComponentValue("n1")))).c_str());
      if(myNextion.getComponentValue("Type") == 1)
      {
        currentMillis = millis();
        while((millis() - currentMillis) < (readEEPROMByte(eeFunctionPulse))*100) 
        { }
        while(myNextion.getNextionValue("tch0") != 0) //Will be non-zero if button still pressed
        {}
        myNextion.sendCommand(("Test.pic=" + String(toggleFunction(myNextion.getComponentValue("F"), 
        myNextion.getComponentValue("n1")))).c_str());
      }
    }
    default:
    {
      break;
    }  
  }
}
