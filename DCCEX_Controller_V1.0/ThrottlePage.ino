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
  switch(button)                  //
  {
    case AccButton:           //
      initPage(AccPage);
      break;
    case RosterButton:
      initPage(RosterPage);
      break;
    case RoutesButton:
      initPage(RoutesPage);
      break;
    case EditButton:
      editingID = selectedIDs[activeSlot];
      eMode = 0;
      initPage(LocoEditPage);
      break;
    case ForwardButton:
    {
      if(readLocoAddress(selectedIDs[activeSlot]) == 0) return;
      dir = 1;
      locos[selectedIDs[activeSlot]].dir = dir;
      if (encoderPos >= readEEPROMByte(eeThreshold))
      {
        locos[selectedIDs[activeSlot]].speed = 0;
        encoderPos = 0;
        oldEncPos = 0;
      }
      doDCC();
      break;
    }
    case ReverseButton:
    {
      if(readLocoAddress(selectedIDs[activeSlot]) == 0) return;
      dir = 0;
      locos[selectedIDs[activeSlot]].dir = dir;
      if (encoderPos >= readEEPROMByte(eeThreshold))
      {
        locos[selectedIDs[activeSlot]].speed = 0;
        encoderPos = 0;
        oldEncPos = 0;
      }
      doDCC();
      break;
    }
    case SliderButton:
    {
      if(readLocoAddress(selectedIDs[activeSlot]) == 0) return;
      encoderPos = myNextion.getComponentValue("throttle");
      oldEncPos = encoderPos;
      locos[selectedIDs[activeSlot]].speed = encoderPos;
      doDCC();
      break;
    }
    default:                                                  
    {
      if(button >=230 && button <= 239)      //These are the 10 Tabs
      {
        activeSlot = (button - 230);        //Determine Activated Tab 0 to 9
        setLocoTabs(activeSlot);
        refreshThrottleValues(activeSlot);
        break;
      }
      if(button>=20 && button <=29)
      {
        fSlot=button-20;
        uint8_t funcNum = readEEPROMByte(locoFuncBase + (selectedIDs[activeSlot]*fBlockSize) +(fSlot*2));    //retrieve the actual function number from its EEPROM slot
        uint8_t funcImg = readEEPROMByte(locoFuncBase + (selectedIDs[activeSlot]*fBlockSize) +(fSlot*2)+1);
        myNextion.sendCommand(("s" + String(fSlot) + ".pic=" + String(toggleFunction(funcNum & 0x7F, funcImg))).c_str());
        if((funcNum & 0x80) != 0)
        {
          currentMillis = millis();
          while((millis() - currentMillis) < (readEEPROMByte(eeFunctionPulse))*100) 
          { }
          while(myNextion.getNextionValue("tch0") != 0) //Will be non-zero if button still pressed
          {}
          myNextion.sendCommand(("s" + String(fSlot) + ".pic=" + String(toggleFunction(funcNum & 0x7F, funcImg))).c_str());
          break;
        }
      }
    }
  }
}
/*
 ***************************************************************************************************************
 * Refresh the Throttle Page Loco fields with Current Settings (and Functions) 
 ***************************************************************************************************************
*/
void refreshThrottleValues(uint8_t activeSlot) 
{
  if(readLocoAddress(selectedIDs[activeSlot]) == 0)
  {
    myNextion.setComponentText("N", "Loco Slot Vacant");
    return;
  }
  encoderPos = locos[selectedIDs[activeSlot]].speed;
  oldEncPos = encoderPos;
  loadFunctions(ThrottlePage, selectedIDs[activeSlot]);
  setActiveSlot(activeSlot);
  for(uint8_t i = 0; i <numLocoSlots ; i++)
  {
    if(readEEPROMByte(eeRNumEnabled) == 0)
    {
      myNextion.setComponentValue("n" + String(i), readLocoAddress(selectedIDs[i]));  //Update Nextion Loco Address
    }else{
      myNextion.setComponentValue("n" + String(i), readLocoRNum(selectedIDs[i]));  //Update Nextion Loco Address
    }
  }
}
/*
 ***************************************************************************************************************
 * Setup Active Loco
 ***************************************************************************************************************
*/
void setActiveSlot(uint8_t activeSlot)
{
  #if defined ESP
    myNextion.setComponentText("N", readEEPROMName(locoNameBase + (selectedIDs[activeSlot] * (locoNameLen+1))));
  #endif
  #if defined AVR_UNO && !defined USE_NEXTION_EEPROM
    myNextion.setComponentText("N", readEEPROMName(locoNameBase + (selectedIDs[activeSlot] * (locoNameLen+1))));
  #endif
  #if defined AVR_UNO && defined USE_NEXTION_EEPROM
    myNextion.sendCommand(("repo N.txt," + String(selectedIDs[activeSlot] * (locoNameLen+1))).c_str());
  #endif
  myNextion.setComponentValue("speedVal", (locos[selectedIDs[activeSlot]].speed));
  myNextion.setComponentValue("throttle", (locos[selectedIDs[activeSlot]].speed));
  myNextion.setComponentValue("ForRev", (locos[selectedIDs[activeSlot]].dir));
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
        myNextion.setComponentValue(("n" + String(fSlot)), fNum);               //Function Number into Function Slot
        myNextion.sendCommand(("n" + String(fSlot)+".pco=0").c_str());           //Set colour to Black
        myNextion.sendCommand(("c" + String(fSlot)+".pco=0").c_str());           //Set colour to Black
        myNextion.setComponentText("c" + String(fSlot), "F");                    //Ensure "F" is displayed
      } //Both Pages
      String nextionCommand = ("s" + String(fSlot) + ".pic=" + String(iNum + (functions[locoID][fSlot]))); //Function Image Slot
      myNextion.sendCommand(nextionCommand.c_str());
    }
    else    //fNum==255
    {
      if (Page == LocoEditPage)
      {
        myNextion.sendCommand(("c" + String(fSlot)+".pco=65535").c_str());      //Colour to White
        myNextion.sendCommand(("n" + String(fSlot)+".pco=50712").c_str());      //Colour to Grey
        myNextion.setComponentText("c" + String(fSlot), "F"); 
        myNextion.sendCommand(("s" + String(fSlot) + ".pic=" + String(GreyedButton)).c_str());   //load greyed Function Image
      }else{
       myNextion.sendCommand(("s" + String(fSlot) + ".pic=" + String(BLANK)).c_str());   //load blank Function Image
      }
    }
  }
}
/*
 ***************************************************************************************************************
 * Setup Active Loco Tab and Refresh Page
 ***************************************************************************************************************
*/
void setLocoTabs(uint8_t tabNumber)
{
  if(tabNumber >=0 && tabNumber <=4)    //Lefthand Tabs
  {
    myNextion.sendCommand(("Left.pic=" + String(tabBase + tabNumber)).c_str());
    myNextion.sendCommand(("Right.pic=" + String(RightNone)).c_str());
  }
  if(tabNumber >=5 && tabNumber <=9)    //Righthand Tabs
  {
    myNextion.sendCommand(("Right.pic=" + String(tabBase + tabNumber)).c_str());
    myNextion.sendCommand(("Left.pic=" + String(LeftNone)).c_str());
  }  
}
/*
 ***************************************************************************************************************
 * Function to Toggle a Function's state
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
