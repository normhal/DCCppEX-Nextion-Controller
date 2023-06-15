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
#include "Config.h"
/*
 *************************************************************************************************************************
 * Process Configuration Page
 *************************************************************************************************************************
*/
void configPage(uint8_t button)
{
  switch (button)
  {
  #if !defined NO_WIFI
    case WiFiButton:
      #if defined ESP
        EEPROM.commit();
      #endif
      initPage(StatusPage);
      break;
    #endif
    case SaveButton:
    {
      saveSelected();
      break;
    }
    case DoneButton:
    {
      #if defined ESP
        EEPROM.commit();
      #endif
      initPage(MenuPage);
      break;
    }
    case RNUM_OFF:
    {
      writeEEPROMByte(eeRNumEnabled, 0);
      break;
    }
    case RNUM_ON:
    {
      writeEEPROMByte(eeRNumEnabled, 1);
      break;
    }
    case PU_OFF:
    {
      writeEEPROMByte(eePUState, 0);
      break;
    }
    case PU_ON:
    {
      writeEEPROMByte(eePUState, 1);
      break;
    }
    case DCC_OFF:
    {
      writeEEPROMByte(eeDCCRefresh, 0);
      break;
    }
    case DCC_ON:
    {
      writeEEPROMByte(eeDCCRefresh, 1);
      break;
    }
    case STOP_OFF:
    {
      writeEEPROMByte(eeLocoStopAll, 0);
      break;
    }
    case STOP_ON:
    {
      writeEEPROMByte(eeLocoStopAll, 1);
      break;
    }
    case LoadButton:    //Code to load pre-defined details
    {
      nextionCommand(("Load.pic=" + String(BUTTON_ON)).c_str());
      #if defined HARD_CODED_VALUES
      for(uint16_t i = 0; i < numLocoDefs; i++)
        {
          #if defined ESP
            writeEEPROMName((locoNameBase + (i * (locoNameLen))), String((userLocoNames[i]) + '\0'));
            writeEEPROMName((locoTypeBase + (i * (locoTypeLen))), String((userLocoTypes[i]) + '\0'));
          #endif
          #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
            writeEEPROMName((locoNameBase + (i * (locoNameLen))), String((userLocoNames[i]) + '\0'));
            writeEEPROMName((locoTypeBase + (i * (locoTypeLen))), String((userLocoTypes[i]) + '\0'));
          #endif
          #if defined AVR_UNO && defined USE_NEXTION_EEPROM
            nextionCommand(("wepo \"" + userLocoNames[i] + "\"," + String(locoNameBase + (i * (locoNameLen)))).c_str());
            delay(5);
            nextionCommand(("wepo \"" + userLocoTypes[i] + "\"," + String(locoTypeBase + (i * (locoTypeLen)))).c_str());
            delay(5);
          #endif
          writeLocoAddress(i, userLocoAddrs[i]);
          writeLocoRNum(i, userLocoRNums[i]);

          writeEEPROMByte((locoFuncBase + (i * 20)) +  0, userLocoFSlot1Num[i]);
          writeEEPROMByte((locoFuncBase + (i * 20)) +  1, userLocoFSlot1Img[i]);
          writeEEPROMByte((locoFuncBase + (i * 20)) +  2, userLocoFSlot2Num[i]);
          writeEEPROMByte((locoFuncBase + (i * 20)) +  3, userLocoFSlot2Img[i]);
        }
        for(uint16_t a = 0; a < numAccDefs; a++)                             //"a" is equivalent to "ID" in this instance
        {
          #if defined ESP
          writeEEPROMName(accNameBase + (a*(accNameLen)), userAccNames[a] + '\0');
          #endif
          #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
          writeEEPROMName(accNameBase + (a*(accNameLen)), userAccNames[a] + '\0');
          #endif
          #if defined AVR_UNO && defined USE_NEXTION_EEPROM
            nextionCommand(("wepo \"" + userAccNames[a] + "\"," + String(accNameBase + (a * (accNameLen)))).c_str());
            delay(5);
          #endif
          writeAccAddress(a, userAccAddrs[a]);
          writeEEPROMByte(accImageBase + a, userAccImages[a]); 
          writeEEPROMByte(accTypeBase + a, userAccTypes[a]);               //default type 0 for now
        }
        for(uint8_t r = 0; r < numRouteDefs; r++)      //number of Routes to initialize
        {
          for(uint8_t i = 0; i < accessoriesPerRoute; i++)
          {
            writeEEPROMByte((routeListBase + (r * accessoriesPerRoute)+i), userRouteIDs[(r*accessoriesPerRoute)+i]);
            writeEEPROMByte((routeStateBase + (r * accessoriesPerRoute)+i), userRouteStates[(r*accessoriesPerRoute)+i]);
          }
        }
      #endif
      nextionSetText("keybdA.b241",NEW_LEFT_CHAR);
      wait(10);
      nextionSetText("keybdA.b243",NEW_RIGHT_CHAR);
      break;
    }
    case Th_Press:
      nextionDataType = THRESHOLD_VALUE;
      break;
    case RE_Accel_Press:
      nextionDataType = RE_ACCEL_VALUE;
      break;
    case Acc_Cmd_Delay:
      nextionDataType = ACC_CMD_GAP;
      break;
    case Func_Pulse_Len:
      nextionDataType = FUNCTION_PULSE_LENGTH;
      break;
    case Read_Timeout:
      nextionDataType = READ_TIMEOUT;
      break;
    default:
      break;
  }
}
