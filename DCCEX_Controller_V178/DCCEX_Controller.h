/***********************************************************************
*                  
* COPYRIGHT (c) 2022, 2023, 2024 Norman Halland (NormHal@gmail.com)
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
 * Header File for DCCEX_Controller
*/
#ifndef DCCEXCONTROLLER_H
  #define DCCEXCONTROLLER_H

  #define Version "V1.7.8"
  uint8_t EEPROMCODE = 178;
  String TFT_SUFFIX = ".tft";
  
  #include <EEPROM.h>
  #include "NextionButtons.h"

  //**************************************************************************************
  
  #if defined (ARDUINO_RASPBERRY_PI_PICO_W)
    #define PICO_W
  #endif
  
  #if defined (ARDUINO_RASPBERRY_PI_PICO)
    #define PICO
  #endif
  
  #include "Config.h"

  #if defined ENABLE_OTA
    #include <ArduinoOTA.h>
  #endif
  #if defined NEXTION_UPLOAD
    #include <NextionUpload.h>
  #endif
  
  //**************************************************************************************
  #if defined (ESP32)
    #define Console Serial
//    #include <HardwareSerial.h>
    #define nextion Serial2     //HardwareSerial nextion(2);
    #if defined WIFI
      #include <WiFi.h>
      WiFiClient client;
    #endif
    //Note: ESP32 Serial2 TX and RX pins by default are: 
    // Pin GPIO16 for TX (Blue wire)
    // Pin GPIO17 for RX (Yellow wire)
    #define REButtonPin 13        // The GPIO Pin number for the encoder reverse button
    static int pinA = 5;          // The first hardware interrupt pin is digital pin 5
    static int pinB = 4;          // The second hardware interrupt pin is digital pin 4
    #define eepromSize 4095
    #define eepromEnd (eepromSize - 1)
    #define numLocos 50                             //number of locos supported
    const uint8_t numFSlots = 10;                   //Function Slots per Loco
    #define numAccs 96                              //Number of accessories supported
    #define numRoutes 48
    #define numLocoSlots 10

    #define accNameLen 9

  #elif defined (ESP8266)
    #include <SoftwareSerial.h>
    #define nextion Serial
    SoftwareSerial Console(4, 14); //NodeMCU D2, NodeMCU D5// TX to GPIO4 and RX to GPIO14 of ESP8266
    #if defined WIFI
      #include <ESP8266WiFi.h>
      WiFiClient client;
    #endif
    #define REButtonPin 5       // Arduino 5 // the Arduino Pin number for the encoder reverse button
    static int pinA = 13;          // The first hardware interrupt pin
    static int pinB = 12;          // The second hardware interrupt pin

    #define eepromSize 4095
    #define eepromEnd (eepromSize - 2)
    #define numLocos 50                             //number of locos supported
    const uint8_t numFSlots = 10;                   //Slots per Loco
    #define numAccs 96                              //Number of accessories supported
    #define numRoutes 48
    #define numLocoSlots 10

    #define accNameLen 9
    
  #elif defined (PICO) || defined (PICO_W)
    #define Console Serial
    #define nextion Serial2
    #if defined WIFI
      #include <WiFi.h>
      WiFiClient client;
    #endif

    #define REButtonPin 13     // The GPIO Pin number for the encoder reverse button
    static int pinA = 5;          // The first hardware interrupt pin is digital pin 5
    static int pinB = 4;          // The second hardware interrupt pin is digital pin 4
    #define eepromSize 4095
    #define eepromEnd (eepromSize - 2)
    #define numLocos 50                             //number of locos supported
    const uint8_t numFSlots = 10;                   //Function Slots per Loco
    #define numAccs 96                              //Number of accessories supported
    #define numRoutes 48
    #define numLocoSlots 10

    #define accNameLen 9

  #elif defined PICO
    #if defined WIFI
      #undef WIFI
    #endif

    #define REButtonPin 13     // The GPIO Pin number for the encoder reverse button
    static int pinA = 5;          // The first hardware interrupt pin is digital pin 5
    static int pinB = 4;          // The second hardware interrupt pin is digital pin 4
    #define eepromSize 4095
    #define eepromEnd (eepromSize-2)
    #define numLocos 50                             //number of locos supported
    const uint8_t numFSlots = 10;                   //Function Slots per Loco
    #define numAccs 96                              //Number of accessories supported
    #define numRoutes 48
    #define numLocoSlots 10

    #define accNameLen 9

  #else                           //Mega etc
    #error "UnTested Processor!!!"
  #endif  

  bool updated = false;           //Flag for the Nextion having been updated 
  
  const uint8_t fBlockSize = (numFSlots * 2);       //Slots x 2 bytes each - Function and Image number

  loco locos[numLocos];

  enum List {
    SSIDs = 1,
    uSSIDs,
    TFTs,
    IPs
  };
  enum List listName;
  
  #define NA 127           //Value for an unassigned Function and Image

  #define ON  1
  #define OFF 0
  

  //THE 6 OPTIONS BELOW SHOULD BE CHANGED VIA THE CONTROLLER CONFIG PAGE ONLY
  //CHANGING THEM HERE AFTER THE FIRST BOOT WILL HAVE NO EFFECT AS
  //THEY ARE USED ONLY ONCE TO INITIALIZE EEPROM
  
  const uint8_t ReverseThreshold = 30;       //Speed after which loco will stop when direction is changed
  const uint8_t REAccAmount = 5;             //Change this to your preferred acceleration rate of the Rotary Encoder
  const bool DCCRefresh = false;
  uint8_t PowerUpState  = ON;                  //Change to "false" to Start up withPower OFF
  uint8_t WiFiEnabled = 1;                    //WiFi disabled by default to allow Credentials to be configured
  uint8_t WiFiRetries = 40;
  const uint8_t AccDelay = 10;                //Number of 10ms intervals between Route Accessory activations
  const uint8_t FunctionPulse = 5;
  uint8_t LocoStopAll = true;

  uint8_t REpinA;
  uint8_t lastREpinA;
  unsigned long lastButtonPress = 0;

  const uint8_t tCount = 4;

/*
  **************************************************************************************************************************************
  * Function Prototypes for all functions
  **************************************************************************************************************************************
  */
  void buttonScheduler();
  void goButton();
  void stopButton();
  void updateSpeed();
  void refreshDCC();
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

  uint8_t connectWiFi(uint8_t);
  void sendCMD(String);
  void receiveCMD();
  void checkWiFi();
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
  void locoEditPage(uint8_t);
  void functionEditPage(uint8_t);

  void initEEPROM();
  uint16_t readLocoAddress(uint8_t);
  uint16_t readAccAddress(uint8_t);
  uint8_t readAccImage(uint8_t);
  String readEEPROMName(uint16_t);  
  String readLocoName(uint8_t);
  uint16_t readEEPROMAddr(uint16_t);
  uint8_t readEEPROMByte(uint16_t);
  void readAccName(uint8_t, uint8_t);
  void readCredentials();
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
void wifiPage(uint8_t);

void throttlePage(uint8_t);
void refreshThrottleValues(uint8_t);
void setActiveSlot(uint8_t);
void activateSlot(uint8_t);
void deActivateSlot(uint8_t);
void setHeadingDetails(uint8_t);
void updateSlotAddresses();
void loadFunctions(uint8_t, uint8_t);
void changeDir(uint8_t);

void nextionSetText(String, String);
void nextionCommand(String);
void nextionSetValue(String, uint8_t);
void updateNextionThrottle(uint8_t);

String nextionReceive();
void setGuest();
void nextionGetText(String);
void getValue(String);
void nextionUpload(uint8_t);
void loadHardCodedValues();
void saveSelected();
void restoreSelected();
void selectionPage(uint8_t);
void fillSelection(enum List listName);
bool connectOTAUpload();

/*
 **********************************************************************************************************
 * Data types expected to be received from the Nextion
 **********************************************************************************************************
*/
#define GUEST_ADDRESS 1
#define LOCO_ADDRESS 2
#define THROTTLE_VALUE 3
#define TOUCH_STATE 4
#define ROAD_NAME 5 
#define ROAD_NUMBER 6
#define LOCO_TYPE 7
#define THRESHOLD_VALUE 11
#define RE_ACCEL_VALUE 12
#define ACC_CMD_GAP 13
#define WIFI_TIMER_VAL 14
#define SSID_NAME 15
#define SSID_PASSWORD 16
#define HOST_IP_ADDRESS 17
#define PORT_NUMBER 18
#define WIFI_RETRIES 19
#define ACC_ADDRESS 100
#define ACC_IMAGE 21
#define ACC_NAME 22
#define FUNC_NUMBER 23
#define FUNC_IMAGE 24
#define READ_TIMEOUT 25
//#define FUNC_TYPE 26
#define TFT_IP 27
#define TFT_PW 28
#define TFT_PORT 29
#define TFT_NAME 30
#define U_SSID 31
#define U_PW 32
const char PROGRAM_ADDRESS = 8;
const char CV_NUMBER = 9;
const char CV_VALUE = 10;
const char CV2_VALUE = 33;
const char CV3_VALUE = 34;
const char CV4_VALUE = 35;
const char CV5_VALUE = 36;
const char CV6_VALUE = 37;

  /*
  ********************************************************************************************************************************
  * Defines for The Nextion
  * 
  * Nextion Page Numbers
  * 
  ********************************************************************************************************************************
  */
  #define CoverPage 0
  #define MenuPage 1
  #define ThrottlePage 2
  #define LocoEditPage 3
  #define RosterPage 4
  #define AccPage 5
  #define RoutesPage 6
  #define ProgramPage 7 
  #define ConfigPage 8 
  #define WiFiPage 9         //WiFi Status and Configuration Page
  #define AccEditPage 10
  #define FNumbersPage 11     //Function Numbers Page
  #define FImagesPage 12      //Function Image Page
  #define AccImagesPage 13
  #define FunctionEditPage 14
  #define UpdatePage 15
  #define SelectionPage 16
//  #define CreditsPage 16
//  #define InfoPage 17
//  #define TipsPage 18

/*
  **********************************
  * Offsets for start of Slot numbers
  **********************************
*/
  #define SlotAStart 20           //Used by Functions
  #define SlotBStart 50           //Used by Accessories
  #define SlotCStart 30           //Used by Accessories and Routes
  #define TabSlotStart 70         //Used by Throttle
  #define FunctionSlotStart 80    //Used by Functions
  #define FunctionPressStart 80   //Used by Functions
  #define FunctionReleaseStart 90 //Used by Functions

  const uint8_t rowsPerNextionRosterPage = 5;         //Nextion's design
/*
  **********************************
  * Program Page
  **********************************
*/
//#define CVN 0
#define CV2 2
#define CV3 3
#define CV4 4
#define CV5 5
#define CV6 6

bool replyExpected = false;            //reply from Command Station Expected
uint8_t progMode = 0;                  //progMain or progProg
#define progMain 1            //Programming on the Main Track
#define progProg 2            //Programming on the Programming Track
uint8_t progType = 0;
#define progRead 1
#define progWrite 2

/*
  **********************************
  * Configuration Page
  **********************************
*/

boolean restartNeeded = false;
/*
  ********************************************************************************************************
  * EEPROM Names, Addresses and Memory Map
  * Also refer to EEPROM.ino file for detailed breakdown
  * 
  * ESP EEPROM MEMORY MAP
  *
  * Refer to EEPROM.ino for details
  ********************************************************************************************************
*/
//EEPROM Memory Assignments
#define locoNameLen 9
#define locoTypeLen 9
#define rNumLen 2
#define accNameLen 9
#define addrLen 2
#define slotLen 2       //function slots
#define routeLen 12     //2 bytes per object - ID and State

const uint16_t locoNameBase  = 0;                                               //LName start at 0
const uint16_t locoTypeBase = locoNameBase + (numLocos * (locoNameLen));        //Types start at 0 + (50 * 9) = 450           //the +1 is for terminator
const uint16_t locoRNumBase  = locoTypeBase + (numLocos * (locoTypeLen));       //RNums start at 450 + (50 * 9) = 900         //the +2 is for 1 terminator and 1 filler
const uint16_t locoAddrBase  = locoRNumBase + (numLocos * rNumLen);             //LAddr start at 900 + (50 * 2) = 1000        //
const uint16_t locoFuncBase  = locoAddrBase + (numLocos * addrLen);             //Funcs start at 10000 + (50 * 2) = 1100      //
const uint16_t accNameBase   = locoFuncBase + (numFSlots * slotLen * numLocos); //AName start at 1100 + (10 * 2 * 50) = 2100  //
const uint16_t accAddrBase   = accNameBase + (numAccs * (accNameLen));          //AAddr start at 2100 + (96*9) = 2964
const uint16_t accImageBase  = accAddrBase + (numAccs * addrLen);               //AImag start at 2964 + (96*2) = 3156
const uint16_t accTypeBase   = accImageBase + numAccs;                          //AType start at 3156 + 96 = 3252
const uint16_t routeListBase  = accTypeBase + numAccs;                          //RList start at 3252 + 96 = 3348
const uint16_t routeStateBase = routeListBase + (numRoutes*routeLen/2);         //RStat start at 3348 + (48 * 12/2) = 3636    //Divided by 2 because routeLen includes ID and State
const uint16_t configBase    = routeStateBase + (numRoutes*routeLen/2);         //CBase start at 3636 + (48 * 12/2) = 3924
  
//ID Numbers
const uint8_t accIDBase         = 0;
const uint8_t locoIDBase        = 128;
const uint8_t routeIDBase       = 200;

// Configuration values in "EEPROM"
#define eeUseExRoster (eepromEnd - 122)     //3972 - (1)  123 = 3971
#define eeActiveSlotA (eepromEnd - 121)     //3973 - (1)  122 = 3972
#define eeActiveSlotB (eepromEnd - 120)     //3974 - (1)  121 = 3973
#define eeActiveSlotC (eepromEnd - 119)     //3975 - (1)  120 = 3974
#define eeActiveSlotD (eepromEnd - 118)     //3976 - (1)  119 = 3975
#define eeSelIDsA     (eepromEnd - 117)     //3986 - (10) 118 = 3976  10 Selected IDs
#define eeSelIDsB     (eepromEnd - 107)     //3996 - (10) 108 = 3986  10 Selected IDs
#define eeSelIDsC     (eepromEnd - 97)      //4006 - (10) 98 = 3996  10 Selected IDs
#define eeSelIDsD     (eepromEnd - 87)      //4016 - (10) 88 = 4006  10 Selected IDs
#define eeJoinMode    (eepromEnd - 77)      //4017 - (1)  78 = 4016
#define eeSSID        (eepromEnd - 76)      //4041 - (24) 77 = 4017  24 Max 24 characters //This overlaps the UNO's Route locations
#define eePWD         (eepromEnd - 52)      //4065 - (24) 53 = 4041  24 Max 24 characters
#define eeIPAddr      (eepromEnd - 28)      //4081 - (16) 29 = 4065  16 Max 16 characters xxx.xxx.xxx.xxx
#define eePort        (eepromEnd - 12)      //4083 - (2)  13 = 4081  2 Max - 1 uint16_t
#define eeLocoStopAll (eepromEnd - 10)      //4084 - (1)  11 = 4083
#define eeWiFiSeconds (eepromEnd - 9)       //4085 - (1)  10 = 4084
#define eeRNumEnabled (eepromEnd - 8)       //4086 - (1)   9 = 4085
#define eeAccDelay    (eepromEnd - 7)       //4087 - (1)   8 = 4086  Delay between Accessory Route Commands
#define eeWiFiEnabled (eepromEnd - 6)       //4088 - (1)   7 = 4087
#define eeThreshold   (eepromEnd - 5)       //4089 - (1)  6  = 4088
#define eeREIncrement (eepromEnd - 4)       //4090 - (1)  5  = 4089
#define eePUState     (eepromEnd - 3)       //4091 - (1)  4  = 4090
#define eeDCCRefresh  (eepromEnd - 2)       //4092 - (1)  3  = 4091
#define eeWiFiRetries (eepromEnd - 1)       //4093 - (1)  1  = 4092
#define eeCheckByte   eepromEnd             //4093 - (1)  0  = 4093

const uint8_t eeAddrLen = 2;
const uint8_t accessoriesPerPage = 12;
const uint8_t locosPerPage = 10;
const uint8_t functionsPerPage = 10;
const uint8_t routesPerPage = 6;
const uint16_t accStartAddress = 440;   //locoAddressBlock(40) + funcDetailsBlock(400)

/*
 ****************************************************************************************************
 * Nextion Object Image Numbers
 * IMPORTANT:
 * These numbers correspond DIRECTLY with the position of the Nextion HMI Images
 * DO NOT MODIFY!!!
 ****************************************************************************************************
*/
  //WiFi Status Images
  #define WIFI_X 30
  #define WIFI_0 31
  #define WIFI_1 32
  #define WIFI_2 33
  #define WIFI_3 34
  #define WIFI_4 35
  #define WIFI_5 36
  #define WIFI_6 37

  uint8_t wifiImage = WIFI_X;   //default to no WiFi

  //Command Station Status Images
  #define CS_G 258              //CS Good
  #define CS_X 259              //CS Bad
  #define CS_W 260              //CS Wait
  
  int csImage = CS_X;

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
  #define RNUM_OFF 170
  #define RNUM_ON 171
  #define PU_OFF 172
  #define PU_ON 173
  #define DCC_OFF 174
  #define DCC_ON 175
  #define STOP_OFF 176
  #define STOP_ON 177
  #define ROUTE_OFF 6
  #define ROUTE_ON 7
  #define DELETE_ON 214
  #define DELETE_OFF 215

  #define BLANK_BUTTON 184

  /*
   ****************************************************************************************************
   * Locos Page - Buttons, defines and Constants
   * 
  */

  /*
   ****************************************************************************************************
   * Accessory Images Page - Buttons, defines and constants
   * 
  */

  /*
  1****************************************************************************************************
  * Accessory Page - Buttons, defines, and constants
  *
  */
  bool accEditMode = false;
  bool accSelectMode = false;
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
   * Convenient Names used by Multiple routines in the Throttle Code
   * 
  */
  #define MAX_COMMAND_LENGTH 30 //Maximum length of a string received from the Base Station

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
  //Some arbitrary defines - don't modify:-)

  #define LIGHT 50712
  #define DARK 19049
  #define GREY_TO_USE LIGHT
  //#define GREY_TO_USE DARK

  const uint16_t BLACK = 0;
  const uint16_t WHITE = 65535;

  const uint16_t GREY = GREY_TO_USE;
  //const uint16_t GREY = 50712;

//  const uint16_t DARK_GREY = 25388;
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
  uint8_t retries;
  int key ;
  uint8_t g_fSlot;
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

  uint8_t accSlot;
  uint8_t activeSlot[tCount];
  uint8_t previousSlot;

  uint8_t resumeSpeeds[numLocoSlots];
  uint8_t backupID;
  uint8_t editingID;
  uint8_t selectedLocoID;
  uint8_t locoStartID = 0;
  uint8_t accStartID = 0;
  uint8_t routesStartID = 0;
  uint8_t programID;
  uint8_t nextID;               //Next available ID

  #if defined DCCPP
    uint8_t LocoFN0to4[numLocos] = {128};   //{128, 128, 128, 128, 128, 128, 128, 128, 128, 128};
    uint8_t LocoFN5to8[numLocos] = {176};   //, 176, 176, 176, 176, 176, 176, 176, 176, 176};
    uint8_t LocoFN9to12[numLocos] = {160};  //, 160, 160, 160, 160, 160,160, 160, 160, 160};
    uint8_t LocoFN13to20[numLocos] = {0};   //, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t LocoFN21to28[numLocos] = {0};   //, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  #endif

  uint8_t functions[numLocos][numFSlots]; 
  uint8_t joinMode = 0;
  uint8_t id;                                   //index into arrays
  boolean DCCflag = 0;
  boolean ledPin_state;
  unsigned long previousMillis = 0;        // will store last time LED was updated
  unsigned long currentMillis = millis();
  const long interval = 300;
  unsigned int saveAddress = 0;
  uint8_t aFlag = 0;
  uint8_t bFlag = 0;
  int encoderPos = 0;
  uint8_t oldEncPos = 0;
  uint8_t oldSpeed = 0;
  uint8_t reading = 0;
  uint8_t old_pos = 0;  //encoderPos;
  boolean buttonState = 0;
  unsigned long interruptTime;
  uint8_t button;
  uint8_t buttonPos;
  uint8_t dir = 1;    // 0 for backwards, 1 for forwards
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

  uint8_t nextionDataType = 0;
  uint8_t savedID = 0;

  //Guest Additions
  uint8_t guestActive = 0;
  uint8_t guestDir = 0;
  uint16_t guestAddress = 0;
  uint8_t newSpeed = 0;

  int long startTime = 0;
  uint8_t readTimeout = 5;
  
  uint16_t lastCSPic = 259;

  uint16_t g_programAddress = 0;
  uint8_t cv_Num = 0;
  uint8_t cv_Value = 0;
  uint8_t cv2_Value = 0;
  uint8_t cv3_Value = 0;
  uint8_t cv4_Value = 0;
  uint8_t cv5_Value = 0;
  uint8_t cv6_Value = 0;
  uint8_t ProgReturnPage = MenuPage;
  uint8_t AccReturnPage = MenuPage;
  uint8_t LocoEditReturnPage = 0;
  uint8_t LocosReturnPage = 0;
  uint8_t RoutesReturnPage = 0;
  uint8_t returnPage = MenuPage;
  String foundSSID[10] = {""};
  uint8_t wifiSeconds = WIFISECS;
  long int wifiTimer = 0;
  long int wifiTimeout = 0;
  uint8_t savedSlot = 0;
  uint8_t thNum = 0;
  uint8_t directionFlag = 0;

#endif
