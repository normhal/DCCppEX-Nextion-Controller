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
#if defined HARD_CODED_VALUES
  console.println("Loading Values");
  /************************************************************************************************************
  * Load Locos and Functions
  /************************************************************************************************************/
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
    #if defined HCL_ALL10
      if(i <10) selectedIDs[i] = i;
    #else
      if(hcLocos[i].FavSlot != "") 
      {
        selectedIDs[hcLocos[i].FavSlot.toInt()] = i;
      }
    #endif
  }
  console.println("All Locos and Functions Loaded");
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
  console.println("Accessories Loaded");
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
  console.println("Routes Loaded");
#endif   //END OF HARD_CODED_VALUES
}
