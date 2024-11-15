//Throttle Page
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
 * Process Throttle Page
 *************************************************************************************************************************
*/
void throttlePage(uint8_t button)
{
//  Console.printf("Throttle Page: %d\n\r", thNum);
  if(message.startsWith("TH"))
  {
    encoderPos = (message.substring(2)).toInt();
    return;
  }
  if(message.startsWith("GL"))          //Guest Loco
  {
    guestAddress = (message.substring(2)).toInt();
    nextionSetText("LName", "Guest Loco");
    nextionCommand("LName.bco=" + String(YELLOW));
    guestActive = true;
    encoderPos = 0;
    return;
  }
  switch(button)                  
  {
    case AccButton:
      nextionCommand("AD.bco=" + String(GREY));
      guestActive = false;        
      AccReturnPage = ThrottlePage;
      initPage(AccPage);
      break;
    case LocosButton:
      nextionCommand("AD.bco=" + String(GREY));
      guestActive = false;        
      LocosReturnPage = ThrottlePage;
      initPage(RosterPage);
      break;
    case ProgramButton:
      nextionCommand("AD.bco=" + String(GREY));
      guestActive = false;  
      ProgReturnPage = ThrottlePage;      
      initPage(ProgramPage);
      break;
    case EditButtonOn:                          //This is Actually the "Address" text
    {
      editingID = selectedIDs[thNum][activeSlot[thNum]];
      if(editingID != 255)
//      if((editingID != 255) && (readLocoAddress(editingID) != 0))      
      {
        nextionCommand("AD.bco=" + String(GREY));
        guestActive = false;
//        editingID = selectedIDs[thNum][activeSlot[thNum]];
        LocoEditReturnPage = ThrottlePage;
        initPage(LocoEditPage);
      }
      break;
    }
    case DirectionButton:
    {
      changeDirection();
      break;
    }
    default:                                                  
    {
      if((button >= TabSlotStart) && (button < (TabSlotStart + locosPerPage)))      //Process the Pressed Tab
      {
        if(guestActive == true)
        {
          nextionCommand("AD.bco=" + String(GREY));
          guestActive = false;
          initPage(ThrottlePage);
          break;
        }
        if(activeSlot[thNum] == (button - TabSlotStart)) break;
        deActivateSlot(activeSlot[thNum]);
        activeSlot[thNum] = (button - TabSlotStart);        //Determine Activated Tab 0 to 9
        activateSlot(activeSlot[thNum]);
        break;
      }
      if(selectedIDs[thNum][activeSlot[thNum]] != 255)        
      {
        if(button >= FunctionSlotStart && button < (FunctionSlotStart + functionsPerPage))         //Process the Function Slots
        {
          if(guestActive == true)
          {
            nextionCommand("AD.bco=" + String(GREY));
            guestActive = false;
            initPage(ThrottlePage);
            break;
          }
          g_fSlot = button - FunctionSlotStart;
          uint8_t funcNum = readEEPROMByte(locoFuncBase + (selectedIDs[thNum][activeSlot[thNum]]*fBlockSize) +(g_fSlot*2));    //retrieve the actual function number from its EEPROM slot
          if(funcNum == 127) break;
          uint8_t funcImg = readEEPROMByte(locoFuncBase + (selectedIDs[thNum][activeSlot[thNum]]*fBlockSize) +(g_fSlot*2)+1);
          nextionCommand(("s" + String(g_fSlot) + ".pic=" + String(toggleFunction(funcNum & 0x7F, funcImg))).c_str());
          break;
        }
        if(button >= FunctionReleaseStart && button < (FunctionReleaseStart + functionsPerPage))
        {
          uint8_t funcNum = readEEPROMByte(locoFuncBase + (selectedIDs[thNum][activeSlot[thNum]]*fBlockSize) +(g_fSlot*2));    //retrieve the actual function number from its EEPROM slot
          if((funcNum & 0x80) != 0)     //fType == PULSE)
          {
            uint8_t funcImg = readEEPROMByte(locoFuncBase + (selectedIDs[thNum][activeSlot[thNum]]*fBlockSize) +(g_fSlot*2)+1);
            nextionCommand(("s" + String(g_fSlot) + ".pic=" + String(toggleFunction(funcNum & 0x7F, funcImg))).c_str());
          }
        }
        break;
      }
    }
  }
}
/*
 ***************************************************************************************************************
 * Send the New Guest Speed to the Command Station
 ****************************************************************************************************************
*/
void setGuest()
{
  String dccppCMD = "<t 0 " + String(guestAddress) + " " + String(encoderPos) + " " + String(guestDir) + ">"; 
  sendCMD(dccppCMD);
}
/*
 ***************************************************************************************************************
 * Reset Speed to 0 if above Threshold
 ****************************************************************************************************************
*/
void checkThreshold()
{
  if (encoderPos >= readEEPROMByte(eeThreshold)) encoderPos = 0;
  updateNextionThrottle(encoderPos);
}
/*
 ***************************************************************************************************************
 * Populate Throttle Page Loco fields with Current Settings (and Functions) 
 ***************************************************************************************************************
*/
void populateSlots()
{  
  for(uint8_t i = 0; i <numLocoSlots ; i++)
  {
    if(i != activeSlot[thNum])
    {
      if(selectedIDs[thNum][i] != 255)
      {
        nextionSetText("n" + String(i),"");                                                                                   //Blank out if unused
        nextionSetText("t" + String(i), readEEPROMName(locoTypeBase + (selectedIDs[thNum][i]* (locoNameLen))));                      //Loco Type
        nextionSetText("n" + String(i), readEEPROMName(locoNameBase + (selectedIDs[thNum][i] * (locoNameLen))));                     //Road Name
        if((readLocoAddress(selectedIDs[thNum][i])) == 0)
        {
          nextionSetText("v" + String(i),"");
          nextionSetText("t" + String(i),"");
          nextionSetText("n" + String(i),"");
        }else
        {
          if(readEEPROMByte(eeRNumEnabled) == 0)
          {
            nextionSetText("v" + String(i), String(readLocoAddress(selectedIDs[thNum][i])));
          }else
          {
            nextionSetText("v" + String(i), String(readLocoRNum(selectedIDs[thNum][i])));
          }
          nextionCommand("n" + String(i) + ".bco=" + String(THGREY));
          nextionCommand("t" + String(i) + ".bco=" + String(THGREY)); 
          nextionCommand("v" + String(i) + ".bco=" + String(THGREY)); 
          nextionCommand("v" + String(i) + ".pco=" + String(WHITE));
        }
      }else
      {
        nextionSetText("v" + String(i),"");
        nextionSetText("t" + String(i),"");
        nextionSetText("n" + String(i),"");
      }
    }
  }
}
/*
 ***************************************************************************************************************
 * Set Loco active
 ***************************************************************************************************************
*/
void activateSlot(uint8_t slot)
{
  if(nextionPage == ThrottlePage)
  {
    #if defined DISPLAY_TAB_DETAILS_GREY_BG
      nextionCommand("n" + String(slot) + ".pco=" + String(BLACK));  //Font Colour
      nextionCommand("t" + String(slot) + ".pco=" + String(BLACK));  //Font Colour
      nextionCommand("n" + String(slot) + ".bco=" + String(GREY));   //Road Name Field background Colour
      nextionCommand("t" + String(slot) + ".bco=" + String(GREY));   //Loco Type Field background Colour
      nextionCommand("v" + String(slot) + ".bco=" + String(GREY));   //Address/Road Number Field background Colour
    #elif defined DISPLAY_TAB_DETAILS_YELLOW_BG
      nextionCommand("n" + String(slot) + ".pco=" + String(BLACK));  //Font Colour
      nextionCommand("t" + String(slot) + ".pco=" + String(BLACK));  //Font Colour
      nextionCommand("n" + String(slot) + ".bco=" + String(YELLOW)); //Road Name Field background Colour
      nextionCommand("t" + String(slot) + ".bco=" + String(YELLOW)); //Loco Type Field background Colour
      nextionCommand("v" + String(slot) + ".bco=" + String(YELLOW)); //Address/Road Number Field background Colour
    #else
      nextionCommand("n" + String(slot) + ".pco=" + String(GREY));   //Font Colour
      nextionCommand("t" + String(slot) + ".pco=" + String(GREY));   //Font Colour
      nextionCommand("n" + String(slot) + ".bco=" + String(GREY));   //Road Name Field background Colour
      nextionCommand("t" + String(slot) + ".bco=" + String(GREY));   //Loco Type Field background Colour
      nextionCommand("v" + String(slot) + ".bco=" + String(GREY));   //Address/Road Number Field background Colour
    #endif
    if((readLocoAddress(selectedIDs[thNum][slot])) == 0 || selectedIDs[thNum][slot] == 255)
    {
      nextionSetText("v" + String(slot),"");
      nextionSetText("t" + String(slot),"");
      nextionSetText("n" + String(slot),"");
      for(uint8_t l_fSlot=0; l_fSlot <numFSlots; l_fSlot++) 
      {
        nextionCommand(("s" + String(l_fSlot) + ".pic=" + String(BLANK)).c_str());   //load blank Function Image
      }
      nextionSetText("AD", String(0));
    }else
    { 
      if(selectedIDs[thNum][slot] != 255)
      {
        #if defined (LONG_NAMES)        
          nextionSetText("LName", longLocoNames[selectedIDs[thNum][slot]]);
        #endif
        nextionSetText("n" + String(slot), readEEPROMName(locoNameBase + (selectedIDs[thNum][slot] * (locoNameLen))));         //Road Name
        nextionSetText("t" + String(slot), readEEPROMName(locoTypeBase + (selectedIDs[thNum][slot]* (locoNameLen))));       //Loco Type  
        if(readEEPROMByte(eeRNumEnabled) == 0)
        {
          nextionSetText("v" + String(slot), String(readLocoAddress(selectedIDs[thNum][slot])));
        }else{
          nextionSetText("v" + String(slot), String(readLocoRNum(selectedIDs[thNum][slot])));
        }
        nextionCommand("v" + String(slot) + ".pco=" + String(BLACK));   //Font Colour
        updateNextionThrottle(locos[selectedIDs[thNum][slot]].speed);
        if(locos[selectedIDs[thNum][slot]].dir == 1) nextionCommand("FR.pic=8");
        else nextionCommand("FR.pic=9");
        nextionSetText("AD", String(readLocoAddress(selectedIDs[thNum][slot])));
        loadFunctions(ThrottlePage, selectedIDs[thNum][slot]);
      }    
    }
  }
}
/*
 ***************************************************************************************************************
 * Deactivate Loco Slot after having been Active
 ***************************************************************************************************************
*/
void deActivateSlot(uint8_t slot)
{
    nextionCommand("n" + String(slot) + ".bco=" + String(THGREY));
    nextionCommand("t" + String(slot) + ".bco=" + String(THGREY)); 
    nextionCommand("v" + String(slot) + ".bco=" + String(THGREY)); 
    nextionCommand("n" + String(slot) + ".pco=" + String(BLACK)); 
    nextionCommand("t" + String(slot) + ".pco=" + String(BLACK));
    nextionCommand("v" + String(slot) + ".pco=" + String(WHITE));
}

/*
 ***************************************************************************************************************
 * Define the 10 Function details and images on the displayed Page - either Throttle or Edit for Loco id
 * Current States of functions are NOT restored in EEPROM
 ***************************************************************************************************************
*/
void loadFunctions(uint8_t Page, uint8_t locoID)
{
  for(uint8_t l_fSlot=0; l_fSlot<numFSlots; l_fSlot++)       //10 function slots representing 2 uint8_ts per group and on a page - never more:-)
  {
    uint8_t fNum = readEEPROMByte((locoFuncBase + (locoID * 20)) + (l_fSlot*2));         // 20 bytes needed for 10 slots on a page
    fNum = fNum & 0x7F;                         //Remove the pulse indicator bit
    uint8_t iNum = readEEPROMByte((locoFuncBase + (locoID * 20)) + (l_fSlot*2)+1);
    if (fNum <= 68)        //Maximum function number supported at this time
    {
      if (Page == LocoEditPage)
      {
        nextionSetValue(("n" + String(l_fSlot)), fNum);               //Function Number into Function Slot
        nextionCommand(("n" + String(l_fSlot)+".pco=0").c_str());           //Set colour to Black
        nextionCommand(("c" + String(l_fSlot)+".pco=0").c_str());           //Set colour to Black
        nextionSetText("c" + String(l_fSlot), "F");                    //Ensure "F" is displayed
      } //Both Pages
      String nextionString = ("s" + String(l_fSlot) + ".pic=" + String(iNum + (functions[locoID][l_fSlot]))); //Function Image Slot
      nextionCommand(nextionString.c_str());
    }
    else    //fNum == 127
    {
      if (Page == LocoEditPage)
      {
        nextionCommand(("c" + String(l_fSlot)+".pco=" + WHITE).c_str());      //Colour to White
        nextionCommand(("n" + String(l_fSlot)+".pco="+ GREY).c_str());      //Colour to Grey
        nextionSetText("c" + String(l_fSlot), "F"); 
        nextionCommand(("s" + String(l_fSlot) + ".pic=" + String(GREYED_BUTTON)).c_str());   //load greyed Function Image
      }else
      {
        nextionCommand(("s" + String(l_fSlot) + ".pic=" + String(BLANK)).c_str());   //load blank Function Image
      }
    } 
  }
}
/*
 ***************************************************************************************************************
 * Function to Toggle a Function's state
 * For the active LocoID
 * Returns the toggled image number and toggles the function state
 ***************************************************************************************************************
 */
uint8_t toggleFunction(uint8_t funcNum, uint8_t funcImg)
{
  #if defined DCCPP
    if (funcNum == 0)
    {
      if (bitRead(LocoFN0to4[selectedIDs[thNum][activeSlot[thNum]]], 4) == 0)     //if function off
      {
      funcImg = funcImg + 1;  
      bitWrite(LocoFN0to4[selectedIDs[thNum][activeSlot[thNum]]], 4, 1);        //set function on
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=1;
    }
    else
    {
      bitWrite(LocoFN0to4[selectedIDs[thNum][activeSlot[thNum]]], 4, 0);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=0;
    }
    doDCCfunction04();
  }
  if (funcNum >= 1 && funcNum <= 4)
  {
    if (bitRead(LocoFN0to4[selectedIDs[thNum][activeSlot[thNum]]], funcNum-1) == 0)
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN0to4[selectedIDs[thNum][activeSlot[thNum]]], funcNum-1, 1); 
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=1;
    }
    else
    {
      bitWrite(LocoFN0to4[selectedIDs[thNum][activeSlot[thNum]]], funcNum-1, 0);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=0;
    }
    doDCCfunction04();
  }
  if (funcNum >= 5 && funcNum <= 8)
  {
    if (bitRead(LocoFN5to8[selectedIDs[thNum][activeSlot[thNum]]], funcNum-5) == 0 ) 
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN5to8[selectedIDs[thNum][activeSlot[thNum]]], funcNum-5, 1);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN5to8[selectedIDs[thNum][activeSlot[thNum]]], funcNum-5, 0);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=0;
    }
    doDCCfunction58();
  }
  if (funcNum >= 9 && funcNum <= 12)
  {
    if (bitRead(LocoFN9to12[selectedIDs[thNum][activeSlot[thNum]]], funcNum-9) == 0 ) 
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN9to12[selectedIDs[thNum][activeSlot[thNum]]], funcNum-9, 1);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN9to12[selectedIDs[thNum][activeSlot[thNum]]], funcNum-9, 0);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=0;
    }
    doDCCfunction912();
  }
  if (funcNum >= 13 && funcNum <= 20)
  {
    if (bitRead(LocoFN13to20[selectedIDs[thNum][activeSlot[thNum]]], funcNum-13) == 0 )
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN13to20[selectedIDs[thNum][activeSlot[thNum]]], funcNum-13, 1);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN13to20[selectedIDs[thNum][activeSlot[thNum]]], funcNum-13, 0);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=0;
    }
    doDCCfunction1320();                                                        //Set up Command to Command Station
  }
  if (funcNum >= 21 && funcNum <= 28)
  {
    if (bitRead(LocoFN21to28[selectedIDs[thNum][activeSlot[thNum]]], funcNum-21) == 0 )         //Function 21 is bit 0 of LocoFN21to28
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN21to28[selectedIDs[thNum][activeSlot[thNum]]], funcNum-21, 1);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN21to28[selectedIDs[thNum][activeSlot[thNum]]], funcNum-21, 0);
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot]=0;
    }
    doDCCfunction2128();                                                      //Set up Command to Command Station
    }
    return(funcImg);
  #endif

  #if !defined DCCPP
    if(functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot] == 0)
    {
      funcImg = funcImg + 1;
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot] = 1;
    }
    else
    {
      functions[selectedIDs[thNum][activeSlot[thNum]]][g_fSlot] = 0;
    }
    doDCCfunctions(funcNum);
    return(funcImg);
  #endif
}
/*
 **********************************************************************************************************
 * Change Loco Direction
 **********************************************************************************************************
*/
void changeDirection()
{
  uint8_t currentSlot;
  if(guestActive == false) currentSlot = activeSlot[thNum];
  else currentSlot = numLocoSlots;
  if(selectedIDs[thNum][currentSlot] != 255)
  { 
    if(encoderPos >= readEEPROMByte(eeThreshold)) 
    {
      locos[selectedIDs[thNum][currentSlot]].speed = 0;
      encoderPos = 0;
      oldEncPos = 0;
    }
    if(locos[selectedIDs[thNum][currentSlot]].dir == 1)       //Currently Forward
    {
      locos[selectedIDs[thNum][currentSlot]].dir = 0;         //Set to Reverse
      nextionCommand("FR.pic=9");
    }else
    {                                                         //Currently Reverse
      locos[selectedIDs[thNum][currentSlot]].dir = 1;         //Set to Forward
      nextionCommand("FR.pic=8");
    }
    doDCC(currentSlot);
  }
}
//************************************************************************************************************  
