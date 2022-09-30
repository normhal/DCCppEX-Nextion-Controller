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
 * Process Configuration Page
 *************************************************************************************************************************
*/
void configPage(uint8_t button)
{
  switch (button)
  {
    case WiFiButton:
      initPage(StatusPage);
      break;
    case SaveButton:
    {
      uint8_t c = 0;
      for (uint16_t s = eeSelIDs; s < (eeSelIDs+numLocoSlots); s++)   //numLocos = 10
      {
        writeEEPROMByte(s, selectedIDs[c]);
        c++;
      }
      writeEEPROMByte(eeActiveSlot,activeSlot);
      if(myNextion.getComponentValue("bt0") == 0) writeEEPROMByte(eeRNumEnabled, 0);
      else writeEEPROMByte(eeRNumEnabled, 1);
      if(myNextion.getComponentValue("bt1") == 0) writeEEPROMByte(eePUState, 0);
      else writeEEPROMByte(eePUState, 1);
      if(myNextion.getComponentValue("bt2") == 0) writeEEPROMByte(eeDCCRefresh, 0);
      else writeEEPROMByte(eeDCCRefresh, 1);
      writeEEPROMByte(eeThreshold, myNextion.getComponentValue("n0"));
      writeEEPROMByte(eeREIncrement, myNextion.getComponentValue("n1"));
      writeEEPROMByte(eeFunctionPulse, myNextion.getComponentValue("n2"));
      writeEEPROMByte(eeAccDelay, myNextion.getComponentValue("n3"));
      #if defined ESP
        EEPROM.commit();
      #endif
    }
    case DoneButton:
    {
      initPage(MenuPage);
      break;
    }
    case LoadButton:
      myNextion.sendCommand(("Load.pic=" + String(BUTTON_ON)).c_str());
      #if defined HARD_CODED_LOCOS
        for(uint8_t i = 0; i < numLocoDefs; i++)
        {
          #if defined ESP
            writeEEPROMName((locoNameBase + (i * (locoNameLen+1))), String((userLocoNames[i]) + '\0'));
          #endif
          #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
            writeEEPROMName((locoNameBase + (i * (locoNameLen+1))), String((userLocoNames[i]) + '\0'));
          #endif
          #if defined AVR_UNO && defined USE_NEXTION_EEPROM
            myNextion.sendCommand(("wepo \"" + userLocoNames[i] + "\"," + String(locoNameBase + (i * (locoNameLen+1)))).c_str());
            delay(5);
          #endif
          writeLocoAddress(i, userLocoAddrs[i]);
          writeLocoRNum(i, userLocoRNums[i]);
        }
      #endif
      #if defined HARD_CODED_ACCS
        for(uint8_t a = 0; a < numAccDefs; a++)                             //"a" is equivalent to "ID" in this instance
        {
          writeEEPROMName(accNameBase + (a*(accNameLen+1)), userAccNames[a] + '\0');
          writeAccAddress(a, userAccAddrs[a]);
          writeEEPROMByte(accImageBase + a, userAccImages[a]); 
          writeEEPROMByte(accTypeBase + a, userAccTypes[a]);               //default type 0 for now
        }
      #endif
      #if defined HARD_CODED_ROUTES
        for(uint8_t r = 0; r < numRouteDefs; r++)      //number of Routes to initialize
        {
          for(uint8_t i = 0; i < accessoriesPerRoute; i++)
          {
            writeEEPROMByte((routeListBase + (r * accessoriesPerRoute)+i), userRouteIDs[(r*accessoriesPerRoute)+i]);
            writeEEPROMByte((routeStateBase + (r * accessoriesPerRoute)+i), userRouteStates[(r*accessoriesPerRoute)+i]);
          }
        }
      #endif
      break;
    default:
      break;
  }
}
