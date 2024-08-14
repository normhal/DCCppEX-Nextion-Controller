/***********************************************************************
*                  
* COPYRIGHT (c) 2024 Norman Halland (NormHal@gmail.com)
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
 ***********************************************************************************************************
 * Optional Hard-Coded Loco Details
 * You can specify up to 10 Loco details which can be loaded into the Loco Inventory positions 1 to 10
 * To upload, press the "Load Hard Coded Details" button on the "Config" Page
 * 
 * Functions for each Loco can be added, edited or deleted via the Loco Edit Page
 * Designate a Favorite Loco with an "*" to initialize the Throttle Page and the Inventory
 ***********************************************************************************************************
*/
void loadHardCodedValues()
{
  Console.println("Loading Values");
  /************************************************************************************************************
  * Load Locos and Functions
  /************************************************************************************************************/
  thNum = 0;                                    //Reset Throttle Number to 0
  for(uint8_t i = 0; i < NUM_HCLOCOS; i++)
  { 
    writeEEPROMName((locoNameBase + (i * (locoNameLen))), ((hcLocos[i].RoadName) + '\0'));
    writeEEPROMName((locoTypeBase + (i * (locoTypeLen))), ((hcLocos[i].LocoType) + '\0'));
    writeLocoAddress(i, hcLocos[i].LocoAddress);
    writeLocoRNum(i, hcLocos[i].RoadNumber);
    for(uint8_t j = 0; j < numFSlots; j++)      //Number of configured function Slots (10 default)
    {
      writeEEPROMByte((locoFuncBase + (i * 20)) + (i*2), 127);    //Initialize all slots to inactive(127)
    }
    for(uint8_t j = 0; j < numFSlots; j++)      //Number of configured function Slots (10 default)
    {
      if(hcLocos[i].FSlot[j][2] != 0)              //only write details if Function Image is present
      {
        writeEEPROMByte((locoFuncBase + (i * 20)) + (hcLocos[i].FSlot[j][0]*2), (hcLocos[i].FSlot[j][1]) | (hcLocos[i].FSlot[j][3]));    //Write the Function number into the correct Slot and address

        writeEEPROMByte((locoFuncBase + (i * 20)) + (hcLocos[i].FSlot[j][0]*2) + 1, hcLocos[i].FSlot[j][2]);  //Write the Image Number into the adjacent Slot and address
      }
    }
  }
  saveSelected();
  Console.println("All Locos and Functions Loaded");
  /*************************************************************************************************************
  * Load Accessories
  /************************************************************************************************************/
  for(uint8_t i = 0; i < (NUM_HCACCS); i++)
  {
    writeEEPROMName(accNameBase + (i*(accNameLen)), hcAcc[i].AccName);
    writeAccAddress(i, hcAcc[i].AccAddress);
    writeEEPROMByte(accImageBase + i, hcAcc[i].AccImage);
    writeEEPROMByte(accTypeBase + i, hcAcc[i].AccType);               //default type 0 for now
  }
  Console.println("Accessories Loaded");
  /*************************************************************************************************************
  * Load Routes
  /************************************************************************************************************/
  for(uint8_t i = 0; i < (NUM_HCROUTES); i++)
  {
    for(uint8_t j = 0; j < accessoriesPerRoute; j++)
    {
      writeEEPROMByte((routeListBase + (i * accessoriesPerRoute) + j), hcRoute[i].AccIDs[j][0]); 
      writeEEPROMByte((routeStateBase + (i * accessoriesPerRoute) + j), hcRoute[i].AccIDs[j][1]);
    }
  }
  Console.println("Routes Loaded");
}
