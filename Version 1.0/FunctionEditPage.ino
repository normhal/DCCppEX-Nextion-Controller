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
      wait(NEX_WAIT);                                             //Wait for Nextion's sake...
      uint8_t funcNumber = nextionGetText("FN").toInt();    //hidden FN sends Function Number
      if(fType == PULSE) funcNumber = (funcNumber | 0x80);
      if(fType == LATCH) funcNumber = (funcNumber & 0x7f);
        wait(2);                                             //Wait for Nextion's sake...
      uint8_t funcImage = nextionGetText("FI").toInt();     //hidden FI sends Function Image Number
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
    case BUTTON_OFF:
    {
      fType = LATCH;
      break;
    }
    case BUTTON_ON:
    {
      fType = PULSE;
      break;
    }
    case TestButtonOn:
    {
      wait(20);
      uint8_t funcNumber = nextionGetText("FN").toInt();
      wait(20);
      uint8_t funcImage = nextionGetText("FI").toInt();

      uint8_t newImage = toggleFunction(funcNumber, funcImage);
      nextionCommand("Test.pic=" + String(newImage));
      if(fType == PULSE)
      {
        currentMillis = millis();
        while((millis() - currentMillis) < (readEEPROMByte(eeFunctionPulse))*100) 
        { }
        while(nextionGetText("T").toInt() != 0) //Will be non-zero if button still pressed
        { }
        uint8_t newImage = toggleFunction(funcNumber, funcImage);
        nextionCommand("Test.pic=" + String(newImage));
      }
      break;
    }
    default:
    {
      break;
    }  
  }
}
