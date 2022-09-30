/***********************************************************************
*                  
* Nextion Based DCC++EX Controller                 
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
*  Designed and written by Norman Halland  2022
*  
*  My Design Objective
* 
*  What started out as a project to modify Dave Bodnar's original Nextion 
*  Throttle while trying to learn "C" and a bit of "C++" has turned out
*  to be a Controller for both DCC++ and DCC-EX Command Stations using either 
*  Direct Serial or WiFi Connections, all based on the Nextion Display range.
*  
*  A basic requirement was that it would support as many common platforms 
*  as possible. Thus, Arduino variants from an Arduino Nano, up to ESP32 devices
*  are supported, with some obvious design and feature restrictions.
*  Nextion ".tft" binaries are available for Basic and Enhanced Displays 
*  of 3.2, 3.5 and 5.0 inch sizes. Some experimental work has been done for 
*  the 4.3 inch variants if anyone is interested. In view of a previous (negative)
*  experience, I will only make the Nextion HMI "Source" code available on request.
*  
*  The DCC++ protocol was chosen in order to be compatible with both DCC++ and DCC-EX 
*  Command Stations. A future (ESP32 only) version is being considered and will possibly
*  only be for DCC-EX environments.
*  
*  Disclaimer:-(
*  
*  I do not claim to be a "C" nor "C++" programmer. Apart from the initial "work" I previously made available
*  for the "Extensions to Dave Bodnar's Nextion Throttle", everything in this version represents my first 
*  venture into the world of "C" and a little "C++". Any critique of my code and design style is expected,
*  welcomed and appreciated:-)
*  
*  I would especially welcome suggestions, ideas, and ways to do things "better", so please be free to forward any
*  thoughts or comments you may feel useful.
*  
*  History
*  
*  Dave Bodnar originally designed a Throttle for the DCC++ system based on the Nextion display and used an Arduino UNO as 
*  the supporting platform. Dave's initial version was the basis of the version I made available around 2017. This version is 
*  essentially a re-design and re-write of everything, except for a few snippets of Dave's original code. 
*  Full acknowledgement, credit and gratitude is thus given to Dave for his work.
*   
*  Subsequent to the first version I made available around 2017, little further development was done, until late in 2021 when a fellow
*  hobbyist (Erik) started asking me some questions:-) This triggered a re-visit of the "Throttle" and I was again tempted to build on the 
*  sketchy knowledge I had gained the first time round. After many hours writing, re-writing, thinking, re-thinking, experimenting, abandoning,
*  getting frustrated, feeling elated, and a thousand other experiences, the current version I have called the "DCC++EX Nextion Controller" emerged.
*  Two other hobbyists, "esfeld" (Steve) and "jbsorocaba" (Francisco) joined Erik and I as a team to discuss, test, critique and suggest ideas. 
*  This "Final" version represents the result of 9 months of fun and contributions by all. 
*  
*  My goal in December 2021 was to develop an enhanced "Throttle" for the DCC++ and DCC-EX systems which could connect directly to the Base 
*  Station via a Serial Port. Any thought of WiFi was outside the scope of my understanding of both "C" and as a communication medium. 
*  The evolution of my original goal resulted in a device with the capabilities and support as in "Features" summarized below.
*  
*  Supported Arduino Boards
*  
*  -  UNO, including Nano and similar mini boards - Direct and Wireless connection via Serial port and HC-12
*  -  ESP8266 - WiFi and direct serial connection, and extended features supported
*  -  ESP32 - WiFi and Direct Serial Connection, and is the preferred processor for the Arduino Sketch
*  
*  Features
*  
*  - Nextion Displays - Basic or Enhanced version of 3.2, 3.5 and 5.0in models with similar functionality
*  - Direct Serial or WiFi connection
*  - "Throttle" page giving instant access to up to 10 Locos (all processors)
*  - "Roster" of locos - 10 Locos for UNOs, up to 40 for ESP models all supporting:-
*    - Loco Name, Road Number, Address, and supported functions (10 function "slots" for any of 28 functions)
*  - "Accessories" - from 36 for UNO models up to 96 for ESPs
*  - "Routes" - basic setting of up to 6 accessories per route - 6 Routes for UNOs, up to 48 routes for ESPs
*  
*  All configuration options are done via the Nextion - No changes to the Arduino Sketch are required
*    
*  Getting Started
*  
*  What You'll Need
*  
*  - A Configured DCC++ Basic or DCC-EX Base Station:-)
*  - A Nextion Display of your choice. The Enhanced models provide a few extra capabilites 
*    when using any UNO variant (Extra EEPROM:-)).
*  - A Host Processor for this Arduino Sketch. Arduino UNO (and variants), ESP8266 or ESP32 (preferred).
*  
*  Optional Features
*  - Rotary Encoder
*    - See pin connections below. Can be used in parallel with the Nextion Slider
*  - Wireless Connection
*    - 2 x HC-12 communication boards connected to the UNO's serial port can 
*      provide a wireless link between the Base Station and the Controller
*  - An ESP8266 as the Host processor
*    - This will connect to a DCC-EX Base Station (STA or AP mode) using the DCC++ protocol
*    - Software Serial is used to interface the Nextion to the ESP8266. This has proved to be slightly un-reliable
*      so is not the preferred processor.
*  - An ESP32 as the Host processor (Recommended Configuration)
*    - The amount of integrated Memory, additional Hardware Serial Port and WiFi make this the processor of choice
*    - In addition to facilitating WiFi, its range of features and relativly affordable cost, contribute to its strengths
*  
*  Connecting it All Together
*  
*  Pin Assignments for UNO variants
*  
*  - Rotary Encoder
*    - Reverse Button        - Arduino pin D8
*    - Clockwise Pulses      - Arduino pin D3
*    - AntiClockwise Pulses  - Arduino pin D2
*    Note: There are a few different versions of Rotary Encoders. Refer to the appropriate Datasheet for specific pin assignments
*  - Nextion Display
*    - Software Serial RX    - Arduino pin D4 to Nextion TX wire (Blue)
*    - Software Serial TX    - Arduino pin D5 to Nextion RX wire (Yellow)
*  - Connection to Serial
*    - Hardware Serial RX    - Arduino pin Dx to DCC++, DCC-EX, or HC-12 TX pin 
*    - Hardware Serial TX    - Arduino pin Dx to DCC++, DCC-EX, or HC-12 RX pin 
*   
*  Pin Assignments for ESP8266
*  
*  - Rotary Encoder
*    - Reverse Button        - ESP8266 pin GPIO5
*    - Clockwise Pulses      - ESP8266 pin GPIO13
*    - AntiClockwise Pulses  - ESP8266 pin GPIO12
*    Note: There are a few different versions of Rotary Encoders. Refer to the appropriate Datasheet for specific pin assignments
*  - Nextion Display
*    - Software Serial RX     - ESP8266 pin GPIO4 to Nextion TX wire (Blue)
*    - Software Serial TX     - ESP8266 pin GPIO14 to Nextion RX wire (Yellow)
*  - Connection to Serial (for debug Console or if using Direct Connection to Command Station)
*    - Hardware Serial RX     - ESP8266 pin GPIOx to DCC++, DCC-EX, or HC-12 TX pin 
*    - Hardware Serial TX     - ESP8266 pin GPIOx to DCC++, DCC-EX, or HC-12 RX pin 
*   
*  Pin Assignments for ESP32
*  
*  - Rotary Encoder
*    - Reverse Button        - ESP32 pin GPIO13
*    - Clockwise Pulses      - ESP32 pin GPIO5
*    - AntiClockwise Pulses  - ESP32 pin GPIO4
*    Note: There are a few different versions of Rotary Encoders. Refer to the appropriate Datasheet for specific pin assignments
*  - Nextion Display
*    - Hardware Serial RX    - ESP32 pin GPIO16 to Nextion TX wire (Blue) 
*    - Hardware Serial TX    - ESP32 pin GPIO17 to Nextion RX wire (Yellow) 
*  - Connection to Serial (for debug Console or if using Direct Connection to Command Station)
*    - Hardware Serial RX    - ESP32 pin GPIOx to DCC++, DCC-EX, or HC-12 TX pin 
*    - Hardware Serial TX    - ESP32 pin GPIOx to DCC++, DCC-EX, or HC-12 RX pin 
*  
*  Note: 
*    Arduino boards using the ATMega32U4 processor and most mini boards using the ATMega328P
*    chip can be programmed to use the "UNO" bootloader which is smaller and yields 
*    the same usable memory as a Standard UNO. No instability has been observed during testing so far...
*    
*  Compiling the Arduino Sketch
*  
*    The "Config.h" file in the sketch is the ONLY file you MIGHT need to modify for your preferences.
*    All operational configuration values can be modified via the "Config" and "WiFi Status" pages of the Nextion. 
*    
*    Once the Arduino IDE has been set up for the intended host processor, no additional libraries are 
*    needed, although the UNO and ESP8266 versions need the Software Serial library. 
*    
*    The Nextion interface uses my own implementation using ideas from the Bently Born Nextion Library, and 
*    a scaled down version of the technique published by Perry Bebbington. 
*  
*  How To's
*  
*  Once everything is connected and Power is applied to the DCC++EX Controller, you'll need to do some basic configuration.
*  TIP: At any stage, touching the "DCC++EX" heading at the top of any page will take you back to the Menu Page.
*  
*  At first the "Cover" page will be displayed for a few seconds, followed by the "Menu" page.
*  
*  1. How To Populate the Loco Roster. 
*     Two methods are provided:-
*     1. If Enabled during compilation it is possible to use "Hard-Coded" Locos and Accessories.
*        Which options are enabled is dependent on available memory - UNOs are very constrained:-(
*        - Select the "Config" option from the Menu, then select the "Load Pre-Defined Details" option
*          - This will very quickly populate the first Roster Entries with the number of basic Loco details 
*            defined in "Config.h" for subsequent editing. Default is 4 Locos.
*          - In addition a page of Accessories and two very simple "Routes" can be loaded
*       or
*     2.  Select the "Roster" page, highlight one of the "Slots" by touching it, then press the "Edit" button
*         - The "Loco Edit" page will be presented for completion.
*         - Press "Done" and the Roster will be displayed again, ready for further additions.
*     Highlighting a Roster Row and pressing the "Select" button will pass that loco's details to the 
*     active Loco "Tab" on the "Throttle" page and locos can be operated.
*  
*  2. How To Populate the Accessory page
*     - Select the "Acc" page 
*       - Can be called from the Menu, Throttle, or Routes pages
*     - Enable "Edit" mode by pressing the "Edit" button
*     - Press one of the 12 (invisible) Accessory "Slots"
*     - Enter the Accessory DCC address first (May NOT be 0!)
*     - In the current Version, only "DCC" type accessories are supported
*     - Press the Blank image below the Image heading to select the desired Accessory Image
*       - Select the desired image for the Accessory being edited/created 
*     - Enter an optional short description for the Turnout - handy when defining a Route:-)
*       - You can test the operation of the accessory before completing
*     - Press the "Done" button
*     - Enter more Accessories as required. "Edit" mode remains active until de-selected.
*     - Existing Accessories can be edited in the same way
*       - To Delete an Accessory, set its Address to Zero 
*     - Depending on the Processor Type in use, up to 8 pages of Accessories can be defined
*     - You can test or operate the Accessories after Edit mode is disabled
*  
*  3. How To Define routes
*     - Select the "Routes" page
*       - Can be called from the Menu, Throttle, or Acc pages
*     - Enable "Edit" mode by pressing the "Edit" button
*     - Press the Route button for the Route you wish to edit or create
*     - The "Acc" page will be presented with the "Select" mode enabled
*       - "Select" mode is only valid when called from the Route page
*       - Leave "Select" mode active while setting and selecting accessories as follows:-
*         - Set the desired Accessory to the state you need the Route to set it to, by pressing the Accessory Image
*         - Press the Accessory Name or Address to include it in the Route
*           - up to 6 accessories can be included in the Route
*           - Selected Accessories will have their background colour changed to Yellow
*       - Press the "Done" button to save the Route
*     - Press the Route Button to execute the "Route"
*     - "Editing" a route means "Re-Creating" it:-)
*     
*  4. Have Fun:-)
*  
*  
*  See the Nextion "Credits", "Info" and "Tips" pages for Acknowledgements and a few other items.
*  
**************************************************************************************************************************
* Changes brought about in Version 1.x
* 
* Split Loco Name (18+1 characters) into two 8 character fields (each with 1 terminator)
* - Name Acronym instead of 18 Character Name
* - Loco Type
*                                                       3.5 HMI     5.0 HMI     Sketch
* Areas requiring modification
* - Throttle Page (moderately significant)              - done
* - Loco Edit Page (medium to significant)
* - Roster Page (significant)
* - Function Edit Page (trivial)                        - done     Pending
* 
*  
*  
*  
**************************************************************************************************************************
*/
#include "DCCEX_Throttle.h"
/*
**************************************************************************************************************************
* Setup Code
**************************************************************************************************************************
*/
void setup() 
{
  Serial.begin(115200);     //For the direct connection to DCC++ or DCC-EX
  nextion.begin(38400);     //This speed has been found to be suitable for both Hard- and Software Nextion Connections.
  
  initPage(CoverPage);
  wait(3000);

  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), PinA, RISING); //interrupt on PinA, rising edge signal & executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(digitalPinToInterrupt(pinB), PinB, RISING); // same for B

  #if defined ESP
    EEPROM.begin(eepromEnd+1);                     //Same as UNO - for now
    wifiEnabled = readEEPROMByte(eewifiEnabled);
    #if defined WIFI
      if(connectWiFi(readEEPROMByte(eeWiFiRetries)) == 1)
      {
        Serial.println(F("WiFi NOT connected! "));
        Serial.print(F("IP address: "));
        Serial.println(WiFi.localIP());
        Serial.print(F("RSSI: "));
        Serial.println(WiFi.RSSI());
      }
    #endif
  #endif

  #if defined SEND_POWER_STATE
    if (readEEPROMByte(eePUState) == 1) powerONButton();    // Set both the Nextion and Command Station Power State
    else powerOFFButton();
  #endif

  if (EEPROM.read(eepromEnd) != EEPROMCODE) initEEPROM();       //Test to see if EEPROM has been initialized before
  uint8_t r = 0;
  for(uint16_t s = eeSelIDs; s < (eeSelIDs + numLocoSlots); s++)
  {
    selectedIDs[r] = readEEPROMByte(s);
    r++;
  }
  activeSlot = readEEPROMByte(eeActiveSlot);
  #if !defined WIFI
    initPage(MenuPage);                       //Display the Menu Page first after the Cover Page/
  #endif
  nextionCommand("bkcmd=0");    //Suppress Error details from the Nextion
  //Initialize Function State arrays for all possible Locos
  for(uint8_t i = 0; i < numLocos; i++)
  {
    LocoFN0to4[i] = 128;
    LocoFN5to8[i] = 176;
    LocoFN9to12[i] = 160;
    LocoFN13to20[i] = 0;
    LocoFN21to28[i] = 0;
  }
  //Ensure Edit Mode is OFF
  eMode = 0;
}//Ready to Rock and Roll
/*
**************************************************************************************************************************
* Main Program Loop
**************************************************************************************************************************
*/
void loop() 
{
  encoderInterrupt();                     // check for rotary encoder activity
  receiveCMD();                           // Receive anything from the Command Station
  if (readEEPROMByte(eeDCCRefresh) == 1)
  {
    DCCcurrentTime = millis(); //send DCC every 30000 ms
    if (((DCCcurrentTime - DCCtime) >= 30000) | (DCCflag == 1)) 
    {
      DCCflag = 0;
      DCCtime = DCCcurrentTime;
      doDCC(activeSlot);
    }
  }
  checkREButton();      // check for change in direction from Rotary Encoder
  buttonScheduler();    // Process the received Nextion data
  #if defined WIFI
    checkWiFi();
  #endif
}   
/*
 **************************************************************************************************************************
 * Process each Nextion Page according to its button message
 * First look for any action common to all Pages
 * Thereafter direct Button input to Routine appropriate to active Nextion Page
 **************************************************************************************************************************
*/
void buttonScheduler()
{
  {
    message = nextionReceive();           //returns a String of max 4 characters
    if (message != "") 
    {
      uint8_t button = message.toInt();     //It's all about the button:-)
      switch(button)
      {
        case StopButton:
          stopButton();
          break;
        case GoButton:
          goButton();
          break;
        case PowerONButton:
          powerONButton();
          break;
        case PowerOFFButton:
          powerOFFButton();
          break;
        case MenuButton:
          initPage(MenuPage);
          break;
        case ThrottleButton:
          initPage(ThrottlePage);
          break;
        #if defined WIFI
        case WiFiRetryButton:
          if(wifiEnabled == 1)
          {
            wifiImage = WIFI_OFF;
            Serial.println(F("Retrying..."));
            nextionCommand("Sig.pic=" + String(wifiImage));
            if(WiFi.status() != WL_CONNECTED) connectWiFi(readEEPROMByte(eeWiFiRetries));
          }
          if(!client.connect(host.c_str(), port))
          {
            wifiProgress = "DCCEX Connection Failed";
            nextionSetText("Progress", wifiProgress);
            Serial.println(wifiProgress);
          }else{
            wifiProgress = "DCCEX Connection Successful";
            nextionSetText("Progress", wifiProgress);
            Serial.println(wifiProgress);
          }
          break;
        #endif
        default:                //switch accordingly to active Page
          switch(nextionPage)   //These cases execute the "Process" routines
          {
            case MenuPage:
              menuPage(button);
              break;
            case ThrottlePage:
              throttlePage(button);
              break;
            case LocoEditPage:
              locoEditPage(button);
              break;
            case RosterPage:
              rosterPage(button);
              break;
            case AccPage:
              accPage(button);
              break;
            case RoutesPage:
              routesPage(button);
              break;
            case ProgramPage:
              programPage(button);
              break;
            case ConfigPage:
              configPage(button);
              break;
            case AccEditPage:
              accEditPage(button);
              break;
            case StatusPage:
              statusPage(button);
              break;
            case FunctionEditPage:
              functionEditPage(button);
              break;
            case CreditsPage:
              creditsPage(button);
              break;
            case InfoPage:
              infoPage(button);
              break;
            case TipsPage:
              tipsPage(button);
              break;
            default:
              break;
          }
      }
    }
  }
}
/*
 **************************************************************************************************************************
 * Process Buttons common to All Pages and non-functional Page "Catch-Alls"
 **************************************************************************************************************************
 * "STOP" button
 *************************************************************************************************************************
*/
void stopButton()
{
  if(readEEPROMByte(eeLocoStopAll) == 1)
  {
    for(uint8_t i = 0; i < numLocoSlots; i++)
    {
      resumeSpeeds[i] = locos[selectedIDs[i]].speed;  
      locos[selectedIDs[i]].speed = 0;
      doDCC(i);
    }
  }else {
    resumeSpeeds[activeSlot] = locos[selectedIDs[activeSlot]].speed;
    locos[selectedIDs[activeSlot]].speed = 0;
    doDCC(activeSlot);
  }
  activateSlot(activeSlot);  
}
/*
 *************************************************************************************************************************
 * "GO" button
 *************************************************************************************************************************
*/
void goButton()
{
  if(readEEPROMByte(eeLocoStopAll) == 1)
  {
    for(uint8_t i = 0; i < numLocoSlots; i++)
    {
      locos[selectedIDs[i]].speed = resumeSpeeds[i];  
      doDCC(i); 
    }
  }else {
    locos[selectedIDs[activeSlot]].speed = resumeSpeeds[activeSlot];
    doDCC(activeSlot);  
  }
  activateSlot(activeSlot);  
}
/*
 *************************************************************************************************************************
 * "PowerON" button
 *************************************************************************************************************************
*/
void powerONButton()
{
  PowerState = 1;
  nextionSetValue(F("Power"), 1);
  sendCMD(F("<1>"));
}
/*
 *************************************************************************************************************************
 * "PowerOFF" button
 *************************************************************************************************************************
*/
void powerOFFButton()
{
  PowerState = 0;
  nextionSetValue(F("Power"), 0);
  sendCMD(F("<0>"));
}
/*
 *************************************************************************************************************************
 * Process Menu Page
 *************************************************************************************************************************
*/
void menuPage(uint8_t button)
{
  originPage = MenuPage;
  prevPage = button;
  if(button != 0) initPage(button);
}
/*
 *************************************************************************************************************************
 * Process Credits Page
 *************************************************************************************************************************
*/
void creditsPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(MenuPage);
      break;
    case CancelButton:
      initPage(MenuPage);
      break;
  }
}
/*
 *************************************************************************************************************************
 * Process Information Page
 *************************************************************************************************************************
*/
void infoPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(MenuPage);
      break;
    case CancelButton:
      initPage(MenuPage);
      break;
  }
}
/*
 *************************************************************************************************************************
 * Process Accessory Images Page
 *************************************************************************************************************************
*/
void accImagesPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(AccEditPage);
      break;
    case CancelButton:
      initPage(AccEditPage);
      break;
  }
}  /*
 *************************************************************************************************************************
 * Process Tips Page
 *************************************************************************************************************************
*/
void tipsPage(uint8_t button)
{
  switch (button)
  {
    case DoneButton:
      initPage(MenuPage);
      break;
    case CancelButton:
      initPage(MenuPage);
      break;
  }
}  
