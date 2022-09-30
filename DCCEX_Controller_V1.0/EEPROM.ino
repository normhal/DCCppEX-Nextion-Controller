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
 * EEPROM Read and Write Routines
 * 
 * ARDUINO UNO EEPROM MEMORY MAP with ENHANCED NEXTION
 * ---------------------------------------------------
 * 20 Loco Addresses of 2 uint8_ts each                  =  40 - Memory Addresses 0 to 39
 *    Names are stored in the Nextion
 * 10 functions using 2 uint8_ts for each loco 20x20     = 400 - Memory Addresses 40 to 439
 * 48 Accessories using 2 uint8_ts for each address      = 144 - Memory Addresses 440 to 584
 *    and 1 uint8_t for each image  3 x 48 = 144            
 * 20 Loco Road Numbers of 2 bytes each                  =  40 bytes
 * 12 Routes
 *   - Route detail = 6 x ID and state pairs = 12 * 12   = 144 bytes
 * eeLocoRNumBase   @             //20 Bytes
 * eeLocoAddrBase   @             //20 Bytes
 *
 * Configuration details need from 950 - 1024            =  74 uint8_ts
 * 
 * Total ARDUINO EEPROM USED                               842
 *                                                      
 * Configuration Block Details                                                     
 * 
 * eeSelIDs         @             //10 Bytes
 * eeRNumEnabled    @             //1 Byte
 * eeActiveSlot     @             //1 Byte
 * eeThreshhold     @             //1 Byte
 * eeREincrement    @             //1 Byte
 * eePUState        @             //1 Byte
 * eeDCCRefresh     @             //1 Byte
 * eeAccDelay       @             //1 Byte
 * eeInitCheck      @1023
 * 

  //Initialize every Road Number to 0
  for (int i = locoRNumBase; i < (locoRNumBase + (numLocos * 2)); i++) writeEEPROMByte(i, 0);  //the *2 is  because of a two byte address

  //Initialize every Loco Address to 0
  for (int i = locoAddrBase; i < (locoAddrBase + (numLocos * 2)); i++) writeEEPROMAddr(i, 0);  //the *2 is  because of a two byte address
 * 
 * 
 * UNO WITH AN ENHANCED NEXTION - EEPROM MEMORY MAP for LOCO and ACCESSORY NAMES ONLY
 * 
 * ----------------------------------------------------------------------------------
 * Loco Names:
 * - 20 names of 18 characters each (19 total)         = 380 uint8_ts   Range is from 0 to 379    Base Address is:   0
 *   - 18 bytes for Name, 1 for terminating null
 * Accessory Names:
 * - 48 names of 8 characters each (9  total)          = 432 uint8_ts   Range is from 380 to 812  Base Address is: 380
 *   - 8 for name, 1 for terminating null
 * 
 * Total NEXTION EEPROM Used                             812 bytes
 * 
 * ARDUINO UNO EEPROM MEMORY MAP with BASIC NEXTION
 * ------------------------------------------------
 * 10 Loco Addresses of 2 uint8_ts each                  =  20 bytes - Memory Addresses 0 to 19
 * 10 Loco Names of 18 characters each (19 total)        = 190 bytes - Memory Addresses 20 to 209 
 * 10 functions using 2 uint8_ts for each loco 10x20     = 200 bytes - Memory Addresses 210 to 409
 * 36 Accessories using 2 uint8_ts for each address      = 108 bytes - Memory Addresses 410 to 518
 *    and 1 uint8_t for each image  3 x 36 = 108            
 * 36 Accessory Names of 8 characters each (9 total)     = 324 bytes - Memory Addresses 518 to 841
 * 10 Loco Road Numbers of 2 bytes each                  =  20 bytes - Memory Addresses 842 to 861
 * 6 Routes
 *   - Route detail = 6 x ID and state pairs = 6 * 12    =  72 bytes - Memory Addresses 862 to 933
 * Configuration details need from 950 - 1024            =  74 bytes - Memory Addresses 940 to 1023
 * 
 * Total ARDUINO EEPROM USED                              1008 bytes
 *                                                      
 * Configuration Block Details                                                     
 * 
 * eeActiveSlot     @1000
 * eeThreshhold     @1001
 * eeREincrement    @1002
 * eePUState        @1003
 * eeDCCRefresh     @1004
 * 10 x eeSelIDs    @1005, 1006, 1007
 * eeInitCheck      @1023
 * 
 * ESP32/8266 EMULATED EEPROM MEMORY MAP
 * -------------------------------------
 * - 4095 bytes in size
 * LocoNames * 50
 * Accessory Names * 96
 *  
 ***********************************************************************************************************************************
 * Routine to Initialize EEPROM - Integrated or Emulated
 ***********************************************************************************************************************************
*/
void initEEPROM()
{
  uint8_t j = 0;
  for (uint16_t i = eeSelIDs; i < (eeSelIDs + numLocoSlots); i++)
  {
    writeEEPROMByte(i ,j);  //Initialize the Selected IDs with incrementing values from 0 to numLocoSlots
    j++;
  }
  writeEEPROMByte(eeActiveSlot, activeSlot);
  writeEEPROMByte(eeThreshold, ReverseThreshold);
  writeEEPROMByte(eeREIncrement, REAccAmount);
  writeEEPROMByte(eePUState, PowerUpState);
  writeEEPROMByte(eeDCCRefresh, DCCRefresh);
  writeEEPROMByte(eewifiEnabled, wifiEnabled);                //WiFi default
  writeEEPROMByte(eeAccDelay, AccDelay);
  writeEEPROMByte(eeFunctionPulse, FunctionPulse);

  //Initialize every Road Number to 0
  for (int i = locoRNumBase; i < (locoRNumBase + (numLocos * 2)); i++) writeEEPROMByte(i, 0);  //the *2 is  because of a two byte address

  //Initialize every Loco Address to 0
  for (int i = locoAddrBase; i < (locoAddrBase + (numLocos * 2)); i++) writeEEPROMAddr(i, 0);  //the *2 is  because of a two byte address

  //Initialize all functions to 255 and images to "Blank" (10)
  for(int i = locoFuncBase; i < accAddrBase; i = i+2){
    writeEEPROMByte(i, 255);
    writeEEPROMByte(i+1, BLANK);
  }

  //Now for the Accessory Addresses to 0 and Images to "Blank" (10)
  for(int i = accAddrBase; i < (accImageBase); i++) writeEEPROMByte(i, 0);
  for(int i = accImageBase; i < (routeListBase); i++) writeEEPROMByte(i, BLANK);
  
  writeEEPROMByte(eepromEnd, EEPROMCODE);   //Indicate that EEPROM has now been initialized
  #if defined ESP
    Serial.println(F("Committing EEPROM Write..."));
    #if defined WIFI
      writeEEPROMByte(eeWiFiRetries, WiFiRetries);
      writeEEPROMName(eeSSID, ssid.c_str());
      writeEEPROMName(eePWD, password.c_str());
      writeEEPROMName(eeIPAddr, host.c_str());
      writeEEPROMAddr(eePort, port);   //2 byte Int
    #endif
    EEPROM.commit();
  #endif
  Serial.print(F("EEPROM Initialized, Check Byte Written and Read Back: "));
  Serial.println(readEEPROMByte(eepromEnd));
}
/*
 **************************************************************************************************************************************
 * EEPROM Read Routines - for non-Nextion EEPROM
 *  Address of Item to be read is calculated as follows:
 *  locoAddrBase + (locoID * addrLen) eg. 7th Loco will have an ID of 6 (       = 6 * 2  = 12 + locoAddrBase   = 0+12     =   12
 *  locoNameBase + (locoID * (locoNameLen+1)) ESP Only
 *  accAddrBase  + (accID * addrLen) eg. 20th Accessory will have an ID of 19  = 19 * 2 = 38 + accAddrBase    = 440+38   =  478
 *  accNameBase  + (accID * (accNameLen+1)  ESP Only 
 *  routeAddrBase + (routeID * routeLen)eg. 5th Route will have an ID of 4       = 4 * 12 = 48 + routeAddrBase  = ...+48   =  648
 * 
 **************************************************************************************************************************************
*/
uint16_t readLocoAddress(uint8_t locoID)
{
  uint16_t eeAddress = (locoAddrBase + (locoID * addrLen));
  uint8_t uint8_t1 = EEPROM.read(eeAddress);
  uint8_t uint8_t2 = EEPROM.read(eeAddress + 1);
  return (uint8_t1 << 8) + uint8_t2;
}
/*
 **************************************************************************************************************************************
*/
uint16_t readLocoRNum(uint8_t locoID)
{
  uint16_t eeAddress = (locoRNumBase + (locoID * addrLen));
  uint8_t uint8_t1 = EEPROM.read(eeAddress);
  uint8_t uint8_t2 = EEPROM.read(eeAddress + 1);
  return (uint8_t1 << 8) + uint8_t2;
}
/*
 **************************************************************************************************************************************
 * Read Accessory Address from EEPROM
 * Accessory Addresses are stored in Arduino/ESP EEPROM from address 440(accAddrBase) to 536 - 2 bytes each
 **************************************************************************************************************************************
*/
uint16_t readAccAddress(uint8_t accID)
{
  uint16_t eeAddress = (accAddrBase + (accID * 2));
  uint8_t uint8_t1 = EEPROM.read(eeAddress);
  uint8_t uint8_t2 = EEPROM.read(eeAddress + 1);
  return (uint8_t1 << 8) + uint8_t2;
}
/*
 **************************************************************************************************************************************
 * Read Accessory Image number form EEPROM
 * Accessory Images are stored in Arduino/ESP EEPROM from address 536 to 584 - 1 byte each
 **************************************************************************************************************************************
*/
uint8_t readAccImage(uint8_t accID)
{
  uint16_t eeAddress = (accImageBase + accID);
  uint8_t uint8_t1 = EEPROM.read(eeAddress);
  return (uint8_t1);
}
/*
 ***********************************************************************************************************************************
 * Read a String from Arduino or ESP EEPROM
 * Last Byte is null character
 * Some Names are stored at random locations
 ***********************************************************************************************************************************
*/
String readEEPROMName(uint16_t eeAddress)
{
  char data [locoNameLen+1];
  for (int i = 0; i < (locoNameLen+1); i++)
  {
    data[i] = EEPROM.read(eeAddress + 1 + i);
    if(data[i] == '\0') break;
  }
  return String(data);
}
/*
 ***********************************************************************************************************************************
 * Read 2 bytes from a specific EEPROM location
 ***********************************************************************************************************************************
*/
uint16_t readEEPROMAddr(uint16_t eeAddress)
{
  uint8_t uint8_t1 = EEPROM.read(eeAddress);
  uint8_t uint8_t2 = EEPROM.read(eeAddress + 1);
  return (uint8_t1 << 8) + uint8_t2;
}
/*
 ***********************************************************************************************************************************
 * Read 1 byte from a specific EEPROM location
 ***********************************************************************************************************************************
*/
uint8_t readEEPROMByte(uint16_t eeAddress)
{
  uint8_t eeData = EEPROM.read(eeAddress);
  return eeData;
}
/*
 ***********************************************************************************************************************************
 * EEPROM Read Routines - for Nextion integrated EEPROM
 * 
 * readAccName
 * 
 *  - The passed ID is used to calculate the address to be read
 *  - the passed Slot is used to determine which slot the Name must be moved into. (0 to 11)
 *  - for the sake of consistency, slot numbers or Names, Addresses and Images will always be 0 to 11
 * 
 * Nextion Address Slots range from t0 to t11
 * Nextion Name Slots range from t20 to t31
 * Nextion Image Slots range from t40 to t51
 ***********************************************************************************************************************************
*/
void readAccName(uint8_t accID, uint8_t accSlot)
{
  int eeAddress = (accID * eeAccBlockSize);
  myNextion.sendCommand(("repo n" + String(accSlot) + ".txt," + String(eeAddress)).c_str());
}

void writeAccAddress(uint8_t ID, uint16_t Address)
{ 
  uint16_t eeAddress = (accAddrBase+(ID * 2));
  uint8_t byte1 = Address >> 8;
  uint8_t byte2 = Address & 0xFF;
  #if defined (AVR_UNO)
    EEPROM.update(eeAddress, byte1);
    EEPROM.update(eeAddress + 1, byte2);
  #else
    EEPROM.write(eeAddress, byte1);
    EEPROM.write(eeAddress + 1, byte2);
  #endif
}
/*
 ***********************************************************************************************************************************
 * EEPROM Write Routines - for Arduino Integrated EEPROM
 * Loco Addresses are stored in Arduino/ESP EEPROM from address 0 to 39 - 2 bytes each
 ***********************************************************************************************************************************
*/
void writeLocoAddress(uint8_t locoID, uint16_t locoAddress)
{ 
  uint16_t eeAddress = (locoAddrBase+(locoID * 2));
  uint8_t byte1 = locoAddress >> 8;
  uint8_t byte2 = locoAddress & 0xFF;
  #if defined (AVR_UNO)
    EEPROM.update(eeAddress, byte1);
    EEPROM.update(eeAddress + 1, byte2);
  #else
    EEPROM.write(eeAddress, byte1);
    EEPROM.write(eeAddress + 1, byte2);
  #endif
}
void writeLocoRNum(uint8_t locoID, uint16_t locoRNum)
{ 
  uint16_t eeAddress = (locoRNumBase+(locoID * 2));
  uint8_t byte1 = locoRNum >> 8;
  uint8_t byte2 = locoRNum & 0xFF;
  #if defined (AVR_UNO)
    EEPROM.update(eeAddress, byte1);
    EEPROM.update(eeAddress + 1, byte2);
  #else
    EEPROM.write(eeAddress, byte1);
    EEPROM.write(eeAddress + 1, byte2);
  #endif
}
/*
 ***********************************************************************************************************************************
*/
void writeEEPROMByte(uint16_t eeAddress, uint8_t eeData)
{
  #if defined (AVR_UNO)
    EEPROM.update(eeAddress, eeData);  
  #else
    EEPROM.write(eeAddress, eeData);
  #endif
}
/*
 ***********************************************************************************************************************************
 * Write a String to Arduino or ESP EEPROM
 ***********************************************************************************************************************************
*/

void writeEEPROMName(uint16_t eeAddress, const String &Name)
{
  uint8_t len = Name.length();
  for (uint8_t i = 0; i < len; i++)
  {
  #if defined (AVR_UNO)
    EEPROM.update(eeAddress + 1 + i, Name[i]);
  #else
    EEPROM.write(eeAddress + 1 + i, Name[i]);
  #endif
  }
}

/*
 ***********************************************************************************************************************************
 * Write a 2 uint8_t "Address" to EEPROM
 ***********************************************************************************************************************************
*/
void writeEEPROMAddr(uint16_t eeAddress, uint16_t wordToWrite)
{ 
  uint8_t uint8_t1 = wordToWrite >> 8;
  uint8_t uint8_t2 = wordToWrite & 0xFF;
  #if defined (AVR_UNO)
    EEPROM.update(eeAddress, uint8_t1);
    EEPROM.update(eeAddress + 1, uint8_t2);
  #else
    EEPROM.write(eeAddress, uint8_t1);
    EEPROM.write(eeAddress + 1, uint8_t2);
  #endif
}
