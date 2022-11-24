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
  #if !defined NO_PROGRAMMING
    switch (button)
    {
      case DoneButton:
        initPage(ProgReturnPage);
        break;
      case CancelButton:
        initPage(ProgReturnPage);
        break;
      case MainButton:
        progMode = progMain;
        nextionSetText(F("Status"), "");
        break;
      case ProgButton:
        progMode = progProg;
        nextionSetText(F("Status"), "");
        break;
      case ReadButton:
      {
        if(progMode == progProg)
        {
          sendCMD("<R " + String(g_cvNumber) + " 5 6>");
          nextionSetText(F("Status"), F("Waiting for CV to be Read"));
          break;
        }
        nextionSetText(F("Status"), F("Read not supported on Main"));
        break;
      }
      case WriteButton:
      {
        if(progMode == progMain)
        {
          nextionSetText(F("Status"), F("Programming Values Sent"));
          sendCMD("<w " + String(g_programAddress) + " " + String(g_cvNumber) + " " + String(g_cvValue) + ">");
          break;        
        }
        if(progMode == progProg)
        {
          nextionSetText(F("Status"), F("Programming Values Sent"));
          sendCMD("<W " + String(g_cvNumber) + " " + String(g_cvValue) + ">");
        break;        
        }
      case Program_AD_Press:
        nextionDataType = PROGRAM_ADDRESS;
        break;
      case CVN_Press:
        nextionDataType = CV_NUMBER;
        break;
      case CVV_Press:
        nextionDataType = CV_VALUE;
        break;
      }
      default:
        break;
    }
  #endif
}
