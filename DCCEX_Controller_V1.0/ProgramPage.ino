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
 *********************************************************************************************************************
 * Process Programming Page
 * Programming Commands supported:
 * Main Line Programming:
 * - Write CV Byte to Loco <w ADDR CV VALUE>
 *   - Bit programming not yet supported
 * Program Track Programming:
 * - Read Loco Address  <R>
 * - Write Loco Address <W>
 * - Read CV Value      <R CV CALLBACKNUM CALLBACKSUB>
 * - Write CV Value     <W CV VALUE>
 * 
 *********************************************************************************************************************
*/
void programPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(LocoEditPage);
      break;
    case CancelButton:
      initPage(MenuPage);
      break;
    case MainButton:
      progMode = progMain;
      myNextion.setComponentText("Status", "");
      break;
    case ProgButton:
      progMode = progProg;
      myNextion.setComponentText("Status", "");
      break;
    case ReadButton:
    {
      if(progMode == progProg)
      {
        String progCMD = "<R " + String(myNextion.getComponentText("CV", NEXWAIT)) + " 5 6>";
        replyExpected = true;
        sendCMD(progCMD);
        myNextion.setComponentText("Status", "Waiting for CV to be Read");
        break;
      }
      myNextion.setComponentText("Status", "Read not supported on Main");
      break;
    }
    case WriteButton:
    {
      if(progMode == progMain)
      {
        uint16_t locoAddr = myNextion.getComponentValue("AD");
        String locoCV  = myNextion.getComponentText("CV", NEXWAIT);
        String cvValue = myNextion.getComponentText("Value", NEXWAIT);
        String progCMD = "<w " + String(locoAddr) + " " + String(locoCV) + " " + String(cvValue) + ">";
        myNextion.setComponentText("Status", "Programming Values Sent");
        sendCMD(progCMD);
        break;        
      }
      if(progMode == progProg)
      {
        uint16_t locoCV  = myNextion.getComponentValue("CV");
        uint8_t cvValue = myNextion.getComponentValue("Value");
        String progCMD = "<W " + String(locoCV) + " " + String(cvValue) + ">";
        myNextion.setComponentText("Status", "Programming Values Sent");
        sendCMD(progCMD);
      break;        
      }
    }
    default:
      break;
  }
}
