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
/* Copyright Norman Halland 2022
 * Header File for DCCEX_Throttle
*/
#ifndef DCCEXTHROTTLE_H
  #define DCCEXTHROTTLE_H

  #define Version "V1.1"
  const uint8_t EEPROMCODE = 0xA5;
  
  #include <EEPROM.h>

  //**************************************************************************************
  #if defined (ESP32) || (ESP8266)
    #define ESP
  #endif

  #if defined (ARDUINO_AVR_LEONARDO)
    #define AVR_UNO
  #elif defined (ARDUINO_AVR_UNO)
    #define AVR_UNO
  #elif defined (ARDUINO_AVR_PRO)
    #define AVR_UNO
  #elif defined (ARDUINO_AVR_NANO)
    #define AVR_UNO
  #endif  

  #if defined ESP
    #define NEX_WAIT 20
  #elif defined AVR_UNO
    #define NEX_WAIT 20
  #endif

  #include "Config.h"

  //**************************************************************************************
  #if defined (ESP32)
    #include <HardwareSerial.h>
    HardwareSerial nextion(2);
    #if defined WIFI
      #include <WiFi.h>
      WiFiClient client;
    #endif
    //Note: ESP32 Serial2 TX and RX pins by default are: 
    // Pin GPIO16 for TX (Blue wire)
    // Pin GPIO17 for RX (Yellow wire)
    const int buttonPin = 13;     // The GPIO Pin number for the encoder reverse button
    static int pinA = 5;          // The first hardware interrupt pin is digital pin 5
    static int pinB = 4;          // The second hardware interrupt pin is digital pin 4
    const uint16_t eepromEnd = 4095;
    #define numLocos 50                             //number of locos supported
    const uint8_t numFSlots = 10;                   //Function Slots per Loco
    #define numAccs 96                              //Number of accessories supported
    #define numRoutes 48
    #define numLocoSlots 10

//    #define locoNameLen 18
    #define accNameLen 9

  #elif defined (ESP8266)
    #define USE_SOFTWARE_SERIAL
    #include <SoftwareSerial.h>
    #include <ESP8266WiFi.h>
    SoftwareSerial nextion(4, 14); //D2??, D5// Nextion TX (Blue) to GPIO4 and RX (Yellow) to GPIO14 of ESP8266
    WiFiClient client;
    const int buttonPin = 5;       // Arduino 5 // the Arduino Pin number for the encoder reverse button
    static int pinA = 13;          // The first hardware interrupt pin
    static int pinB = 12;          // The second hardware interrupt pin

    const uint16_t eepromEnd = 4095;
    #define numLocos 50                             //number of locos supported
    const uint8_t numFSlots = 10;                   //Slots per Loco
    #define numAccs 96                              //Number of accessories supported
    #define numRoutes 48
    #define numLocoSlots 10

//    #define locoNameLen 18
    #define accNameLen 9

  #elif defined (AVR_UNO)                             // Pin assignments for UNO
    #include <SoftwareSerial.h>
    SoftwareSerial nextion(4, 5); // Nextion TX to pin 4 and RX to pin 5 of Arduino UNO
    const int buttonPin = 8;      // the Arduino Pin number for the encoder reverse button
    static int pinA = 3;          // the first hardware interrupt pin is digital pin 2
    static int pinB = 2;          // the second hardware interrupt pin is digital pin 3

//    #define locoNameLen 18
    #define accNameLen 9

    #if !defined USE_NEXTION_EEPROM
      #define eepromEnd 1023              //Integrated Arduino UNO EEPROM
      #define numLocos 10                 //Multiples of 10
      const uint8_t numFSlots = 10;       //Function Slots per loco  - always 10 for this version
      #define numAccs 36                  //Multiples of 12
      #define numRoutes 6                 //Multiples of 6
      #define numLocoSlots 10             //Always 10 for this version
    #endif
    
    #if defined USE_NEXTION_EEPROM
      #define eepromEnd 1023              //Integrated Arduino UNO EEPROM
      #define numLocos 10                 //Multiples of 10
      const uint8_t numFSlots = 10;       //Function Slots per loco  - always 10 for this version
      #define numAccs 48                  //Multiples of 12
      #define numRoutes 24                //Multiples of 6
      #define numLocoSlots 10             //Always 10 for this version
    #endif
    
#else                           //Mega etc
  #error "UnTested Processor!!!"
#endif  

  
  const uint8_t fBlockSize = (numFSlots * 2);       //Slots x 2 bytes each - Function and Image number

/*
*******************************************************************************************************************************
* Defines and Variables for Loco Details
*******************************************************************************************************************************
*/
  struct locomotive {
	uint8_t dir = 1;
	uint8_t speed;
};
typedef struct locomotive loco;

  loco locos[numLocos];
  
  bool functions[numLocos][numFSlots];    //Array of function slots and the state of the function which occupies that slot
                                         //ie a specific function state is addressed - functions[locoID][fSlot]. 
                                         //The array has numLocos rows, and numFSlots columns
 
  //THE 6 OPTIONS BELOW SHOULD BE CHANGED VIA THE CONTROLLER CONFIG PAGE ONLY
  //CHANGING THEM HERE AFTER THE FIRST BOOT WILL HAVE NO EFFECT AS
  //THEY ARE USED ONLY ONCE TO INITIALIZE EEPROM
  
  const uint8_t ReverseThreshold = 30;       //Speed after which loco will stop when direction is changed
  const uint8_t REAccAmount = 5;             //Change this to your preferred acceleration rate of the Rotary Encoder
  const bool DCCRefresh = false;
  bool PowerUpState  = true;                  //Change to "false" to Start up withPower OFF
  uint8_t wifiEnabled = 0;                    //WiFi disabled by default to allow Credentials to be configured
  const uint8_t WiFiRetries = 20;
  const uint8_t AccDelay = 10;                //Number of 10ms intervals between Route Accessory activations
  #define FunctionPulse 5
  bool LocoStopAll = true;

/*
 **************************************************************************************************************************************
 * Function Prototypes
 **************************************************************************************************************************************
 */

void buttonScheduler();
void stopButton();
void powerONButton();
void powerOFFButton();
void menuPage(uint8_t);
void creditsPage(uint8_t);
void infoPage(uint8_t);
void accImagesPage(uint8_t);
void displayCredits();
void setLocoTabs(uint8_t);
void populateSlots();

void setPageButtons(uint8_t, uint8_t, uint8_t);
void acc_Page(uint8_t);
void accDrawPage(uint8_t);

void accEditPage(uint8_t);

int connectWiFi(uint8_t);
void sendCMD(String);
void receiveCMD();
void wait(uint16_t);
void parse(char *com);
void doDCC(uint8_t);
void doDCCAcc(uint8_t);
void doDCCfunction04();
void doDCCfunction58();
void doDCCfunction912();
void doDCCfunction1320();
void doDCCfunction2128();
uint8_t toggleFunction(uint8_t, uint8_t);

void configPage(uint8_t);

void initEEPROM();
uint16_t readLocoAddress(uint8_t);
uint16_t readAccAddress(uint8_t);
uint8_t readAccImage(uint8_t);
String readEEPROMName(uint16_t);  
String readLocoName(uint8_t);
uint16_t readEEPROMAddr(uint16_t);
uint8_t readEEPROMByte(uint16_t);
void readAccName(uint8_t, uint8_t);
void writeLocoName(uint8_t, const String &Name);
void writeAccAddress(uint8_t, uint16_t);
void writeLocoAddress(uint8_t, uint16_t);
void writeEEPROMByte(uint16_t, uint8_t);
void writeEEPROMName(uint16_t, const String &Name);
void writeEEPROMAddr(uint16_t, uint16_t);

void initPage(uint8_t);
void activatePage(uint8_t);

void programPage(uint8_t);

void rosterPage(uint8_t);
void rosterDrawPage(uint8_t);

void encoderInterrupt();
void checkREButton();
void PinA();
void PinB();

void routesPage(uint8_t);
void routeDrawPage(uint8_t);
void statusPage(uint8_t);

void throttlePage(uint8_t);
void refreshThrottleValues(uint8_t);
void setActiveSlot(uint8_t);
void activateSlot(uint8_t);
void deActivateSlot(uint8_t);
void setLocoDetails(uint8_t);
void updateSlotAddresses();
void loadFunctions(uint8_t, uint8_t);
void changeDir(uint8_t);

String nextionGetText(String);
void nextionSetText(String, String);
void nextionCommand(String);
String nextionGetValue(String);
void nextionSetValue(String, uint8_t);
void updateNextionThrottle(uint8_t);

String nextionReceive();

void updateCredentials();
/*
 **********************************************************************************************************
 * EEPROM Names, Addresses and Memory Map
 * Also refer to EEPROM.ino file for detailed breakdown
 * 
 * ARDUINO and ESP EEPROM MEMORY MAP
 * 
 * Loco and Accessory Names are stored on the Enhanced Nextion Models
 * 
 * These are the EEPROM and Emulated EEPROM assignments
 * 
 * numLocos defines the number of addresses, loco names, and function groups
 * - 10 for AVR devices using Basic Nextion
 * - 20 for AVR devices (1k Arduino EEPROM + 1k Enhanced Nextion EEPROM available)
 * - up to 50 for ESP devices (4k total emulated EEPROM available)
 * numFSlots defines the number of function slots for each loco
 * - 10 for All devices
 * numAccs defines the number of Accessories supported
 * - 36 for AVR using Basic Nextion
 * - 48 for AVR using Enhanced Nextion
 * - 96 for ESP using either Basic or Enhanced Nextion
 * numRoutes defines the number of Routes supported
 * - 6 for AVR using Basic Nextion
 * - 12 for AVR using Enhanced Nextion
 * - 48 for ESP using either Basic or Enhanced Nextion
 * 
 * AVR devices using Basic Nextion use 1K of integrated EEPROM as follows:
 * 
 * 10 Loco Addresses of 2 bytes each                      =  20 - Addresses 0 to 39    - Base Address = 0
 * 10 Loco Names of 21 bytes each                         = 210
 * 10 function slots using 2 bytes for each slot 20x20    = 200 - Addresses 40 to 439  - Base Address = 40
 * 36 Accessory Addresses using 2 bytes each              =  72 - addresses 440 to 536 - Base Address = 440
 * 36 Accessory Names of 9 bytes each                     = 324
 * 36 Accessory Image numbers                             =  36 - addresses 536 to 584 - Base Address = 536
 * 36 Accessory Types (and optional State Planned)        =  36 - addresses 584 to 632 - Base Address = 584
 * 6 Route Contents of 12 bytes each                      =  72 - addresses 632 to 776 - Base Address = 632
 * Configuration details reside from 920 - 1024           =  54 - addresses 920 to 1024- Base Address = 920
 *                                                         1024 bytes
 *                                                         
 * AVR devices using Enhanced Nextion use 1K of integrated EEPROM as follows:
 *                                                         
 * 20 Loco Addresses of 2 bytes each                      =  40 - Addresses 0 to 39    - Base Address = 0
 * 10 function slots using 2 bytes for each slot 20x20    = 400 - Addresses 40 to 439  - Base Address = 40
 * 48 Accessory Addresses using 2 bytes each              =  96 - addresses 440 to 536 - Base Address = 440
 * 48 Accessory Image numbers                             =  48 - addresses 536 to 584 - Base Address = 536
 * 48 Accessory Types (with optional State Planned)       =  48 - addresses 584 to 632 - Base Address = 584
 * 24 Route Contents of 12 bytes each                     = 288 - addresses 632 to 919 - Base Address = 632
 * Configuration details reside from 920 - 1024           =  74 - addresses 920 to 1024- Base Address = 1024 - 74
 *                                                          994 bytes
 *                                                          
 * ESP Devices using either Nextion model get 4096 bytes Emulated EEPROM allocated as follows:-
 * 
 * 50 Loco Names of 21 bytes each                         =  950 - Addresses 0 to 949     - Base Address = 0
 * 50 Loco Road Numbers of 2 bytes each                   =  100 - Addresses 950 to 1049  - Base Address = 950
 * 50 Loco Addresses of 2 bytes each                      =  100 - Addresses 1050 to 1149 - Base Address = 1050
 * 10 Function Slots using 2 bytes for each loco          = 1000 - Addresses 1150 to 2149 - Base Address = 1150
 * 96 Accessory Names     -  9 bytes each                 =  864 - Addresses 2150 to 3014 - Base Address = 2150
 * 96 Accessory Addresses using 2 bytes each              =  192 - Addresses 3014 to 3206 - Base Address = 3014
 * 96 Accessory Image numbers                             =   96 - Addresses 3206 to 3302 - Base Address = 3206
 * 96 Accessory Type (with optional State Planned)        =   96 - Addresses 3302 to 3398 - Base Address = 3302
 * 48 Route Contents of 12 bytes each (6xIDs + 6xStates)  =  576 - Addresses 3398 to 3974 - Base Address = 3398
 *                                                          3974 bytes Total
 * Bytes remaining for Configuration values                  122                                                          
 *                                                          4096
 * ID Numbers are assigned as follows: THESE ARE NOT USED AS SUCH AT THE MOMENT - PLANNED for future
 * 
 * - Accessories use from 0 to 127 (128)        - 0 to 47     - 48 for now               Base Number  = 0
 * - Locos from 128 to 199 (72)                 - 128 to 147  - 20 for now               Base Number  = 128
 * - Routes from 200 to 255 (56)                - 200 to 211  - 12 for now               Base Number  = 200
 * 
 * For the sake of code simplicity, ID numbers internally always start at 0
 * Any communication with the base station where ID number is needed adds the relevant baseID to the ID
 * 
 * To Calculate the Relative position within the appropriate EEPROM block 
 * 
 * ID numbers are first multiplied by their block size, then added to their respective Base Address to 
 * determine their physical address
 * 
 * The Base addresses which are used by the ESP and AVR with Basic Nextion are as follows:
 * locoNameBase   = 0
 * locoRNumBase   = locoNameBase + (numLocos * 19)
 * locoAddrBase   = locoRNumBase + (numLocos * 2)
 * locoFuncBase   = locoAddrBase + (numLocos * 2)
 * accNameBase    = locoFuncBase + (numSlots * 2 * numLocos)
 * accAddrBase    = accNameBase + (numAccs * 9)
 * accImageBase   = accAddrBase + (numAccs * 2)
 * accTypeBase    = accImageBase + numAccs
 * routeListBase  = accTypeBase + numAccs                     //These are the Accessory IDs in the Route
 * routeStateBase  = routeListBase + (numRoutes * routeLen/2) //These are the States for the IDs in the Route
 * configBase     = routeListBase + (numRoutes * routeLen)    //- not used
 * 
 * eg. The DCC Address of Loco ID 7 would be located at: 7(ID) x 2(bytes per address) + 0(locoBaseAddress) = Address 0014
 *     The DCC Address of Accessory ID 10 would be located at 10(ID) x 2(bytes per address) + 440(locoBaseAddress) = Address 0460
 *                                                     
 * Nextion EEPROM is used to store Loco and Accessory Names for AVR devices using Enhanced Nextion Displays only
 * 
 * Loco Names:
 * - 20 names of 20 characters each (21 total)         = 420 uint8_ts   Range is from 0 to 420    Base Address is:   0
 *   - 20 for Name, 1 for null terminator
 * Accessory Names
 * - 48 names of 8 characters each (9  total)          = 432 uint8_ts   Range is from 420 to 852  Base Address is: 420
 *   - 8 for name, 1 for null terminator
 *  
*/
//EEPROM Memory Assignments
#define locoNameLen 9
#define locoTypeLen 9
#define rNumLen 2
#define accNameLen 9
#define addrLen 2
#define slotLen 2       //function slots
#define routeLen 12     //2 bytes per object - ID and State

#if defined ESP || !defined USE_NEXTION_EEPROM
  const uint16_t locoNameBase  = 0;
  const uint16_t locoTypeBase = locoNameBase + (numLocos * (locoNameLen));    //the +1 is for terminator
  const uint16_t locoRNumBase  = locoTypeBase + (numLocos * (locoTypeLen));   //the +2 is for 1 terminator and 1 filler
  const uint16_t locoAddrBase  = locoRNumBase + (numLocos * rNumLen);
  const uint16_t locoFuncBase  = locoAddrBase + (numLocos * addrLen); 
  const uint16_t accNameBase   = locoFuncBase + (numFSlots * slotLen * numLocos);
  const uint16_t accAddrBase   = accNameBase + (numAccs * (accNameLen));
  const uint16_t accImageBase  = accAddrBase + (numAccs * addrLen);
  const uint16_t accTypeBase   = accImageBase + numAccs;
  const uint16_t routeListBase  = accTypeBase + numAccs;
  const uint16_t routeStateBase = routeListBase + (numRoutes*routeLen/2);   //Divided by 2 because routeLen includes ID and State
  const uint16_t configBase    = routeStateBase + (numRoutes*routeLen/2);
  
//  const uint16_t routeStateBase = routeListBase + (numRoutes * routeLen/2);   //Divided by 2 because routeLen includes ID and State
//  const uint16_t configBase    = routeListBase + (numRoutes * routeLen);

#elif defined AVR_UNO && defined USE_NEXTION_EEPROM
  
  //Nextion EEPROM Assignments
  const uint16_t locoNameBase  = 0;
  const uint16_t locoTypeBase  = locoNameBase + (numLocos * (locoNameLen));
  const uint16_t accNameBase   = locoTypeBase + (numLocos * (locoNameLen));
  
  //UNO Integrated EEPROM Assignments
  const uint16_t locoAddrBase  = 0;
  const uint16_t locoFuncBase  = locoAddrBase + (numLocos*addrLen);
  const uint16_t accAddrBase   = locoFuncBase +  (numLocos * numFSlots * slotLen);
  const uint16_t accImageBase  = accAddrBase + (numAccs * addrLen);
  const uint16_t accTypeBase   = accImageBase + numAccs;
  const uint16_t routeListBase  = accTypeBase + numAccs;
  const uint16_t routeStateBase = routeListBase + (numRoutes * routeLen/2);
  const uint16_t locoRNumBase = routeListBase + (numRoutes * routeLen);
//  const uint16_t configBase    = routeListBase + (numRoutes * routeLen);

#elif defined AVR_MEGA && defined USE_NEXTION_EEPROM
  const uint16_t locoNameBase  = 0;
  const uint16_t locoTypeBase  = locoNameBase + (numLocos * (locoNameLen));
  const uint16_t accNameBase   = locoTypeBase + (numLocos * (locoNameLen+2));

  //MEGA Integrated EEPROM Assignments
  const uint16_t locoAddrBase  = 0;
  const uint16_t locoFuncBase  = numLocos*addrLen;
  const uint16_t accAddrBase   = locoFuncBase +  (numLocos * numFSlots * slotLen);
  const uint16_t accImageBase  = accAddrBase + (numAccs * addrLen);
  const uint16_t accTypeBase   = accImageBase + numAccs;
  const uint16_t routeListBase  = accTypeBase + numAccs;
  const uint16_t routeStateBase = routeListBase + (numRoutes * routeLen/2);
//  const uint16_t configBase    = routeListBase + (numRoutes * routeLen);
#endif      

//ID Numbers
const uint8_t accIDBase         = 0;
const uint8_t locoIDBase        = 128;
const uint8_t routeIDBase       = 200;

#define eeSSID        (eepromEnd - 79)      //1023 - 74 = 949  20 Max 20 characters //This overlaps the UNO's Route locations
#define eePWD         (eepromEnd - 59)      //1023 - 54 = 969  20 Max 20 characters
#define eeIPAddr      (eepromEnd - 39)      //1023 - 34 = 989  26 Max 16 characters xxx.xxx.xxx.xxx
#define eePort        (eepromEnd - 23)      //1023 - 18 = 1005  2 Max 2 uint8_ts
#define eeLocoStopAll (eepromEnd - 21)      //1023 - 79 = 944
#define eeFunctionPulse (eepromEnd - 20)    //1023 - 78 = 945
#define eeRNumEnabled (eepromEnd - 19)      //1023 - 77 = 946
#define eeAccDelay    (eepromEnd - 18)      //1023 - 76 = 947  Delay between Accessory Route Commands
#define eewifiEnabled (eepromEnd - 17)      //1023 - 75 = 948
#define eeSelIDs      (eepromEnd - 16)      //1023 - 16 = 1007  10 Selected IDs
#define eeThreshold   (eepromEnd - 6)       //1023 - 6  = 1017
#define eeREIncrement (eepromEnd - 5)       //1023 - 5  = 1018
#define eePUState     (eepromEnd - 4)       //1023 - 4  = 1019
#define eeDCCRefresh  (eepromEnd - 3)       //1023 - 3  = 1020
#define eeActiveSlot  (eepromEnd - 2)       //1023 - 2  = 1021
#define eeWiFiRetries (eepromEnd - 1)       //1023 - 1  = 1022
#define eeCheckByte   eepromEnd             //1023 - 0  = 1023


/*
 * eeSelIDs         @             //10 Bytes
 * eeLocoRNumBase   @             //20 Bytes
 * eeLocoAddrBase   @             //20 Bytes
 * eeRNumEnabled    @             //1 Byte
 * eeActiveSlot     @             //1 Byte
 * eeThreshhold     @             //1 Byte
 * eeREincrement    @             //1 Byte
 * eePUState        @             //1 Byte
 * eeDCCRefresh     @             //1 Byte
 * eeAccDelay       @             //1 Byte
 * eeInitCheck      @1023
*/


const uint8_t eeAddrLen = 2;
const uint8_t accRowsPerPage = 12;
const uint16_t accStartAddress = 440;   //locoAddressBlock(40) + funcDetailsBlock(400)

/*
 ********************************************************************************************************************************
 * Defines for The Nextion
 ********************************************************************************************************************************
*/
/*
 * Page numbers
*/
#define CoverPage 0
#define MenuPage 1
#define ThrottlePage 2
#define LocoEditPage 3
#define RosterPage 4
#define AccPage 5
#define RoutesPage 6
#define ProgramPage 7       //Programming Page
#define ConfigPage 8       //Configuration Page
#define StatusPage 9
#define AccEditPage 10
#define FNumbersPage 11      //Function Button Page
#define FImagesPage 12      //Function Image Page
#define AccImagesPage 13
#define FunctionEditPage 14
#define CreditsPage 15
#define InfoPage 16
#define TipsPage 17

/*
 ****************************************************************************************************
 * Nextion Button numbers
 * These numbers are what each Button sends to Identify itself 
 * and is Hard Coded into the Nextion HMI Touch or Release Event
 ****************************************************************************************************
 * Menu Page and Navigation Buttons
 **********************************
*/
#define RosterButton 201
#define AccButton 202
#define RoutesButton 203
#define ProgramButton 204
#define EditButton 206
#define SelectButton 207
#define DeleteButton 208
#define DoneButton 210
#define CancelButton 211
#define MenuButton 212
#define LoadButton 213
#define PageUpButton 214
#define PageDownButton 215

/*
 **********************************
 * Throttle Page Buttons
 **********************************
*/
#define PowerOFFButton 30
#define PowerONButton 31
#define StopButton 32
#define GoButton 33
#define Loco1Button 230
#define Loco2Button 231
#define Loco3Button 232
#define Loco4Button 233
#define Loco5Button 234
#define Loco6Button 235
#define Loco7Button 236
#define Loco8Button 237
#define Loco9Button 238
#define Loco10Button 239
#define ForwardButton 37
#define ReverseButton 36

#define WiFiButton 45           //To select WiFi Status Page
#define WiFiRetryButton 34      //WiFi Emblem Button
#define RetryButton 35
#define WiFiEnabledOFF 66    //WiFi Status Page Button
#define WiFiEnabledON 67    //WiFi Status Page Button

#define SliderPress 41
#define ThrottleButton 42   //Used on Routes Page
#define SliderMove 41
#define SliderEvent 41

/*
 ****************************************************************************************************
 * Edit Page - Buttons, defines and Constants
 * 200 to 209 - Function Slot Number
*/
#define TestButtonOff 47
#define TestButtonOn 46
/*
 ****************************************************************************************************
 * Nextion Object Image Numbers
 * IMPORTANT:
 * These numbers correspond DIRECTLY with the position of the Nextion HMI Images
 * DO NOT MODIFY!!!
 ****************************************************************************************************
*/
//WiFi Status Images
#define WIFI_OFF 30
#define WIFI_0 31
#define WIFI_1 32
#define WIFI_2 33
#define WIFI_3 34
#define WIFI_4 35
#define WIFI_5 36
#define WIFI_6 37

uint8_t wifiImage = WIFI_OFF;

//Throttle Page Tab Images
#define Left0 10
#define Left1 11
#define Left2 12
#define Left3 13
#define Left4 14
#define Right0 15
#define Right1 16
#define Right2 17
#define Right3 18
#define Right4 19
#define LeftNone 20
#define RightNone 21

#define tabBase 10

//General Image Numbers

#define BLANK 22
#define GREYED_BUTTON 23
#define QMARK 119
#define PAGEUP 24
#define PAGEUPON 25
#define PAGEUPGREYED 26
#define PAGEDN 27
#define PAGEDNON 28
#define PAGEDNGREYED 29
#define BUTTON_OFF 132
#define BUTTON_ON 133
const char SELECT_OFF = 144;
const char SELECT_ON = 145;
#define EDIT_OFF 164
#define EDIT_ON 165
#define LATCH 0
#define PULSE 1
#define RNUM_OFF 140
#define RNUM_ON 141
#define PU_OFF 142
#define PU_ON 143
#define DCC_OFF 146
#define DCC_ON 147
#define STOP_OFF 148
#define STOP_ON 149
#define ROUTE_OFF 244
#define ROUTE_ON 245
#define DELETE_OFF 19
#define DELETE_ON 18

/*
 ****************************************************************************************************
 * Roster Page - Buttons, defines and Constants
 * 
*/
const uint8_t rowsPerNextionRosterPage = 10;         //Nextion's design

/*
 ****************************************************************************************************
 * Accessory Images Page - Buttons, defines and constants
 * 
*/

/*
 ****************************************************************************************************
 * Accessory Page - Buttons, defines, and constants
 *
*/
bool accEditMode = false;
bool accSelectMode = false;
const uint8_t accRowsPerNextionPage = 12;
const uint8_t eeAccBlockSize = 3;       //2 uint8_t Address and 1 uint8_t Image

bool funcEditMode = false;

/*
 ****************************************************************************************************
 * Routes Page - Buttons, defines and Constants
 * 
*/
const uint8_t rowsPerNextionRoutePage = 6;         //Nextion's design
#define accessoriesPerRoute 6
uint8_t routeStartID = 0;
uint8_t routeList[accessoriesPerRoute][2];
uint8_t accCount = 0;

/*
 ****************************************************************************************************
 * Programming Page - Buttons, defines and Constants
 * 
*/
#define MainButton 57
#define ProgButton 58
#define ReadButton 59
#define WriteButton 60
bool replyExpected = false;            //reply from Command Station Expected
uint8_t progMode = 0;                      //progMain or progProg
const uint8_t progMain = 1;            //Programming on the Main Track
const uint8_t progProg = 2;            //Programming on the Programming Track

/*
 *****************************************************************************************************
 * Config Page - Buttons, defines and Constants
 * 
*/
#define ConfigButton 44         //To Select Config Page
#define SaveButton 39
/*
 *****************************************************************************************************
 * Accessory Edit and Accessory Images Page Specific Buttons
 * 
*/

#define DCCButton 61
#define VPinButton 62
#define ServoButton 63
#define LCNButton 64
#define ImageButton 65

/*
 ****************************************************************************************************
 * Convenient Names used by Multiple routines in the Throttle Code
 * 
*/
#define  MAX_COMMAND_LENGTH         30 //Maximum length of a string received from the Base Station

#define fGroup4 222           // Don't change - mask to generate higher function numbers
#define fGroup5 223           // Don't change - mask to generate higher function numbers
#define incAmount 5           // change this to your preferred acceleration rate of the Rotary Encoder

/*
 * Slot Names for Loco Selection
*/
#define slot1 0
#define slot2 1
#define slot3 2
#define slot4 3
#define slot5 4
#define slot6 5
#define slot7 6
#define slot8 7
#define slot9 8
#define slot10 9
/*
 * Colours
*/
const uint16_t BLACK = 0;
const uint16_t WHITE = 65535;
const uint16_t GREY = 50712;
const uint16_t DARK_GREY = 25388;
const uint16_t YELLOW = 65504;
/*
************************************************************************************************************************
* 
***********************************************************************************************************************
*/

uint8_t accStates[numAccs];
uint8_t selectedAccs[numAccs];  //for Routes...
/*
 * Global Variables used by Throttle
*/

//static char commandString[MAX_COMMAND_LENGTH+1];   //MAX_COMMAND_LENGTH+1];
char commandString[MAX_COMMAND_LENGTH+1];   //MAX_COMMAND_LENGTH+1];

bool  deleteMode = 0;
uint8_t eMode = 0;
String message;
String wifiStatus;
String wifiProgress;
int key ;
uint8_t fSlot;
uint8_t editImage;
uint8_t funcNum;

unsigned long lastTime;
unsigned long elapsedTime;
unsigned long DCCtime;
unsigned long DCCcurrentTime;
bool lastA = false;
unsigned long number = 0;
unsigned int lowest = 0;
unsigned int highest = 126; //126;
bool rcvCMD = false;
//bool editMode  = false;

uint8_t activeSlot;
uint8_t previousSlot;
uint8_t selectedIDs[numLocoSlots];
uint8_t resumeSpeeds[numLocoSlots];
uint8_t backupID;
//uint8_t safeID;
uint8_t originPage;
uint8_t prevPage;
uint8_t editingID;
uint8_t selectedRosterID;
uint8_t locoStartID = 0;
uint8_t accStartID = 0;
uint8_t routesStartID = 0;
uint8_t programID;
uint8_t nextID;               //Next available ID

uint8_t LocoFN0to4[numLocos] = {128};   //{128, 128, 128, 128, 128, 128, 128, 128, 128, 128};
uint8_t LocoFN5to8[numLocos] = {176};   //, 176, 176, 176, 176, 176, 176, 176, 176, 176};
uint8_t LocoFN9to12[numLocos] = {160};  //, 160, 160, 160, 160, 160,160, 160, 160, 160};
uint8_t LocoFN13to20[numLocos] = {0};   //, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t LocoFN21to28[numLocos] = {0};   //, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t id;                                   //index into arrays
boolean DCCflag = 0;
boolean ledPin_state;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = millis();
const long interval = 300;
unsigned int saveAddress = 0;
uint8_t aFlag = 0;
uint8_t bFlag = 0;
uint8_t encoderPos = 0;
uint8_t oldEncPos = 0;
uint8_t reading = 0;
uint8_t old_pos = 0;  //encoderPos;
boolean buttonState = 0;
boolean directionFlag = 0;
unsigned long interruptTime;
uint8_t button;
uint8_t buttonPos;
boolean dir = 1;    // 0 for backwards, 1 for forwards
uint8_t functionSlot = 127;

uint8_t fType = 0;  //Function type being edited
String changeButton = "";
int nextionPage = 0;      //Current Page displayed on Nextion
boolean PowerState = 1;
boolean Processed = 0;
int TOAddress = 0;
int TOSubAddress = 0;
boolean TOState = 0;
boolean REFlag = 0;
String dccppCMD = "";      //String sent to DCC++ Base Station

#endif
