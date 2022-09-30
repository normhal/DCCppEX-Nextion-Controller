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
  originPage = ThrottlePage;
  switch(button)                  
  {
    case AccButton:
      originPage = ThrottlePage;          
      initPage(AccPage);
      break;
    case RosterButton:
      #if defined AVR_UNO
        locoStartID = 0;
      #endif
      initPage(RosterPage);
      break;
    case ProgramButton:
      initPage(ProgramPage);
      break;
    case EditButton:
      editingID = selectedIDs[activeSlot];
      initPage(LocoEditPage);
      break;
    case ForwardButton:
    {
      if(readLocoAddress(selectedIDs[activeSlot]) == 0) return;
      dir = 1;
      changeDir(dir);
      break;
    }
    case ReverseButton:
    {
      if(readLocoAddress(selectedIDs[activeSlot]) == 0) return;
      dir = 0;
      changeDir(dir);
      break;
    }
    case SliderEvent:
    {
      wait(NEX_WAIT);
      if(readLocoAddress(selectedIDs[activeSlot]) == 0) return;
      String tempStr = nextionGetText("T");
      if(tempStr != "")
      {
        encoderPos = tempStr.toInt();
        oldEncPos = encoderPos;
        locos[selectedIDs[activeSlot]].speed = encoderPos;
        doDCC(activeSlot);
      }
      break;
    }
    default:                                                  
    {
      if(button >=230 && button <= 239)      //These are the 10 Tabs
      {
        if(activeSlot == (button - 230)) break;
        deActivateSlot(activeSlot);
        activeSlot = (button - 230);        //Determine Activated Tab 0 to 9
        activateSlot(activeSlot);
        break;
      }
      if(button>=20 && button <=29)         //These are the Function Slots
      {
        fSlot=button-20;
        uint8_t funcNum = readEEPROMByte(locoFuncBase + (selectedIDs[activeSlot]*fBlockSize) +(fSlot*2));    //retrieve the actual function number from its EEPROM slot
        uint8_t funcImg = readEEPROMByte(locoFuncBase + (selectedIDs[activeSlot]*fBlockSize) +(fSlot*2)+1);
          nextionCommand(("s" + String(fSlot) + ".pic=" + String(toggleFunction(funcNum & 0x7F, funcImg))).c_str());
          if((funcNum & 0x80) != 0)     //fType == PULSE)
          {
            currentMillis = millis();
            while((millis() - currentMillis) < (readEEPROMByte(eeFunctionPulse))*100) 
            { }
            while(nextionGetText("T1").toInt() != 0) //Will be non-zero if button still pressed
            { }
          nextionCommand(("s" + String(fSlot) + ".pic=" + String(toggleFunction(funcNum & 0x7F, funcImg))).c_str());
          }
        break;
      }
    }
  }
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
  #if !defined SAVE_MEMORY
    if(i != activeSlot)
    {
  #endif
      nextionSetText("t" + String(i), readEEPROMName(locoNameBase + (selectedIDs[i] * (locoNameLen))));         //Road Name
      nextionSetText("t" + String(i+10), readEEPROMName(locoTypeBase + (selectedIDs[i]* (locoNameLen))));       //Loco Type
      if(readEEPROMByte(eeRNumEnabled) == 0)
      {
        nextionSetText("t" + String(i+20), String(readLocoAddress(selectedIDs[i])));
      }else{
        nextionSetText("t" + String(i+20), String(readLocoRNum(selectedIDs[i])));
      }
      nextionCommand("t" + String(i + 20) + ".pco=" + String(WHITE));
  #if !defined SAVE_MEMORY
    }
  #endif
  }
}
/*
 ***************************************************************************************************************
 * Set Loco active
 ***************************************************************************************************************
*/
void activateSlot(uint8_t slot)
{
  setLocoDetails(slot);
  #if defined DISPLAY_TAB_DETAILS_GREY_BG
    nextionCommand("t" + String(slot) + ".pco=" + String(BLACK));        //Font Colour
    nextionCommand("t" + String(slot + 10) + ".pco=" + String(BLACK));   //Font Colour
    nextionCommand("t" + String(slot) + ".bco=" + String(GREY));        //Road Name Field background Colour
    nextionCommand("t" + String(slot + 10) + ".bco=" + String(GREY));   //Loco Type Field background Colour
    nextionCommand("t" + String(slot + 20) + ".bco=" + String(GREY));   //Address/Road Number Field background Colour
  #elif defined DISPLAY_TAB_DETAILS_YELLOW_BG
    nextionCommand("t" + String(slot) + ".pco=" + String(BLACK));        //Font Colour
    nextionCommand("t" + String(slot + 10) + ".pco=" + String(BLACK));   //Font Colour
    nextionCommand("t" + String(slot) + ".bco=" + String(YELLOW));        //Road Name Field background Colour
    nextionCommand("t" + String(slot + 10) + ".bco=" + String(YELLOW));   //Loco Type Field background Colour
    nextionCommand("t" + String(slot + 20) + ".bco=" + String(YELLOW));   //Address/Road Number Field background Colour
  #else
    nextionCommand("t" + String(slot) + ".pco=" + String(GREY));        //Font Colour
    nextionCommand("t" + String(slot + 10) + ".pco=" + String(GREY));   //Font Colour
    nextionCommand("t" + String(slot) + ".bco=" + String(GREY));        //Road Name Field background Colour
    nextionCommand("t" + String(slot + 10) + ".bco=" + String(GREY));   //Loco Type Field background Colour
    nextionCommand("t" + String(slot + 20) + ".bco=" + String(GREY));   //Address/Road Number Field background Colour
  #endif
  nextionSetText("t" + String(slot), readEEPROMName(locoNameBase + (selectedIDs[slot] * (locoNameLen))));         //Road Name
  nextionSetText("t" + String(slot+10), readEEPROMName(locoTypeBase + (selectedIDs[slot]* (locoNameLen))));       //Loco Type
  if(readEEPROMByte(eeRNumEnabled) == 0)
  {
    nextionSetText("t" + String(slot+20), String(readLocoAddress(selectedIDs[slot])));
  }else{
    nextionSetText("t" + String(slot+20), String(readLocoRNum(selectedIDs[slot])));
  }
  nextionCommand("t" + String(slot + 20) + ".pco=" + String(BLACK));   //Font Colour
  updateNextionThrottle(locos[selectedIDs[slot]].speed);
  nextionSetValue(F("ForRev"), (locos[selectedIDs[slot]].dir));
  nextionSetText("AD", String(readLocoAddress(selectedIDs[slot])));
  loadFunctions(ThrottlePage, selectedIDs[slot]);
}
/*
 ***************************************************************************************************************
 * Deactivate Loco Slot after having been Active
 ***************************************************************************************************************
*/
void deActivateSlot(uint8_t slot)
{
    nextionCommand("t" + String(slot) + ".bco=" + String(DARK_GREY));
    nextionCommand("t" + String(slot + 10) + ".bco=" + String(DARK_GREY)); 
    nextionCommand("t" + String(slot + 20) + ".bco=" + String(DARK_GREY)); 
    nextionCommand("t" + String(slot) + ".pco=" + String(BLACK)); 
    nextionCommand("t" + String(slot + 10) + ".pco=" + String(BLACK));
    nextionCommand("t" + String(slot + 20) + ".pco=" + String(WHITE));
}

/*
 ***************************************************************************************************************
 * Define the 10 Function details and images on the displayed Page - either Throttle or Edit for Loco id
 * Current States of functions are NOT restored in EEPROM
 ***************************************************************************************************************
*/
void loadFunctions(uint8_t Page, uint8_t locoID)
{
  for(uint8_t fSlot=0; fSlot<10; fSlot++)       //10 function slots representing 2 uint8_ts per group and on a page - never more:-)
  {
    uint8_t fNum = readEEPROMByte((locoFuncBase + (locoID * 20)) + (fSlot*2));         // 20 bytes needed for 10 slots on a page
    fNum = fNum & 0x7F;                         //Remove the pulse indicator bit
    uint8_t iNum = readEEPROMByte((locoFuncBase + (locoID * 20)) + (fSlot*2)+1);
    if (fNum <= 30)        //Maximum function number supported at this time
    {
      if (Page == LocoEditPage)
      {
        nextionSetValue(("n" + String(fSlot)), fNum);               //Function Number into Function Slot
        nextionCommand(("n" + String(fSlot)+".pco=0").c_str());           //Set colour to Black
        nextionCommand(("c" + String(fSlot)+".pco=0").c_str());           //Set colour to Black
        nextionSetText("c" + String(fSlot), "F");                    //Ensure "F" is displayed
      } //Both Pages
      String nextionString = ("s" + String(fSlot) + ".pic=" + String(iNum + (functions[locoID][fSlot]))); //Function Image Slot
      nextionCommand(nextionString.c_str());
    }
    else    //fNum== 127
    {
      if (Page == LocoEditPage)
      {
        nextionCommand(("c" + String(fSlot)+".pco=65535").c_str());      //Colour to White
        nextionCommand(("n" + String(fSlot)+".pco=50712").c_str());      //Colour to Grey
        nextionSetText("c" + String(fSlot), "F"); 
        nextionCommand(("s" + String(fSlot) + ".pic=" + String(GREYED_BUTTON)).c_str());   //load greyed Function Image
      }else{
       nextionCommand(("s" + String(fSlot) + ".pic=" + String(BLANK)).c_str());   //load blank Function Image
      }
    }
  }
}
/*
 ***************************************************************************************************************
 * Function to Toggle a Function's state
 * For the active LocoID
 ***************************************************************************************************************
 */
uint8_t toggleFunction(uint8_t funcNum, uint8_t funcImg)
{
  if (funcNum == 0)
  {
    if (bitRead(LocoFN0to4[selectedIDs[activeSlot]], 4) == 0)     //if function off
    {
      funcImg = funcImg + 1;  
      bitWrite(LocoFN0to4[selectedIDs[activeSlot]], 4, 1);        //set function on
      functions[selectedIDs[activeSlot]][fSlot]=1;
    }
    else
    {
      bitWrite(LocoFN0to4[selectedIDs[activeSlot]], 4, 0);
      functions[selectedIDs[activeSlot]][fSlot]=0;
    }
    doDCCfunction04();
  }
  if (funcNum >= 1 && funcNum <= 4)
  {
    if (bitRead(LocoFN0to4[selectedIDs[activeSlot]], funcNum-1) == 0)
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN0to4[selectedIDs[activeSlot]], funcNum-1, 1); 
      functions[selectedIDs[activeSlot]][fSlot]=1;
    }
    else
    {
      bitWrite(LocoFN0to4[selectedIDs[activeSlot]], funcNum-1, 0);
      functions[selectedIDs[activeSlot]][fSlot]=0;
    }
    doDCCfunction04();
  }
  if (funcNum >= 5 && funcNum <= 8)
  {
    if (bitRead(LocoFN5to8[selectedIDs[activeSlot]], funcNum-5) == 0 ) 
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN5to8[selectedIDs[activeSlot]], funcNum-5, 1);
      functions[selectedIDs[activeSlot]][fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN5to8[selectedIDs[activeSlot]], funcNum-5, 0);
      functions[selectedIDs[activeSlot]][fSlot]=0;
    }
    doDCCfunction58();
  }
  if (funcNum >= 9 && funcNum <= 12)
  {
    if (bitRead(LocoFN9to12[selectedIDs[activeSlot]], funcNum-9) == 0 ) 
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN9to12[selectedIDs[activeSlot]], funcNum-9, 1);
      functions[selectedIDs[activeSlot]][fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN9to12[selectedIDs[activeSlot]], funcNum-9, 0);
      functions[selectedIDs[activeSlot]][fSlot]=0;
    }
    doDCCfunction912();
  }
  if (funcNum >= 13 && funcNum <= 20)
  {
    if (bitRead(LocoFN13to20[selectedIDs[activeSlot]], funcNum-13) == 0 )
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN13to20[selectedIDs[activeSlot]], funcNum-13, 1);
      functions[selectedIDs[activeSlot]][fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN13to20[selectedIDs[activeSlot]], funcNum-13, 0);
      functions[selectedIDs[activeSlot]][fSlot]=0;
    }
    doDCCfunction1320();                                                        //Set up Command to Command Station
  }
  if (funcNum >= 21 && funcNum <= 28)
  {
    if (bitRead(LocoFN21to28[selectedIDs[activeSlot]], funcNum-21) == 0 )         //Function 21 is bit 0 of LocoFN21to28
    {
      funcImg = funcImg + 1;
      bitWrite(LocoFN21to28[selectedIDs[activeSlot]], funcNum-21, 1);
      functions[selectedIDs[activeSlot]][fSlot]=1;
    }
    else 
    {
      bitWrite(LocoFN21to28[selectedIDs[activeSlot]], funcNum-21, 0);
      functions[selectedIDs[activeSlot]][fSlot]=0;
    }
    doDCCfunction2128();                                                      //Set up Command to Command Station
  }
  return(funcImg);
}
/*
 **********************************************************************************************************
 * Change Loco Direction
 **********************************************************************************************************
*/
void changeDir(uint8_t dir)
{
  locos[selectedIDs[activeSlot]].dir = dir;
  if (encoderPos >= readEEPROMByte(eeThreshold))
  {
    locos[selectedIDs[activeSlot]].speed = 0;
    encoderPos = 0;
    oldEncPos = 0;
  }
  doDCC(activeSlot);
}
/*
 **********************************************************************************************************
 * Set Loco Details at the top of each Page
 * This routine does NOT update the Tab details on the Throttle Page except for the Address...
 **********************************************************************************************************
*/
void setLocoDetails(uint8_t activeSlot)
{
  #if defined ESP
    nextionSetText("Na", readEEPROMName(locoNameBase + (selectedIDs[activeSlot] * (locoNameLen))));
    nextionSetText("Nb", readEEPROMName(locoTypeBase + (selectedIDs[activeSlot] * (locoTypeLen))));
  #endif
  #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
    nextionSetText("Na", readEEPROMName(locoNameBase + (selectedIDs[activeSlot] * (locoNameLen))));
    nextionSetText("Nb", readEEPROMName(locoTypeBase + (selectedIDs[activeSlot] * (locoTypeLen))));
  #endif
  #if defined AVR_UNO && defined USE_NEXTION_EEPROM
    nextionCommand(("repo Na.txt," + String(selectedIDs[activeSlot] * (locoNameLen))).c_str());
    nextionCommand(("repo Nb.txt," + String(selectedIDs[activeSlot] * (locoTypeLen))).c_str());
  #endif
  nextionSetText("Nc", String(readLocoRNum(selectedIDs[activeSlot])));
  nextionSetValue("S", locos[selectedIDs[activeSlot]].speed);
}
