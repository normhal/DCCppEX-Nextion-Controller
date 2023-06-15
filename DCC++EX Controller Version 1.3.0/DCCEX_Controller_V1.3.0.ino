/***********************************************************************

  Nextion Based DCC++EX Controller

  COPYRIGHT (c) 2022 Norman Halland (NormHal@gmail.com)

   This program and all its associated modules is free software:
   you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software
   Foundation, either version 3 of the License, or (at your option)
   any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses

************************************************************************/
/*
 * Designed and written by Norman Halland  2022
 * 
 ***********************************************************************

  Version History

   V1.0 Published on:-
    Features
    - Supports AVR ATMega328 and ESP8266 and ESP32 processors
    - Nextion Touch or Optional Rotary Encoder operation
    - DCCpp protocol to support DCCpp and DCCEX Command Stations
    - Supports Direct Command Station Connection or WiFI (ESP Versions)
    - Throttle Page gives instant access to 10 Locos
    - Loco Shed (Inventory) of up to 50 Locos
      - Each Loco can have any combination (out of 28) of 10 Functions
    - Up to 96 Accessories
    - Up to 48 "Routes"
    - All models support Ops or Program Track Programming

   V1.1 Published on:-
    Includes all the Features of Version 1.0
    Added Features
    - Throttle Page displays up to 10 Loco Road Names, Numbers and Addresses
    - Active Loco Details visible at the top of each Nextion Page
    - Various bug fixes
    
   V1.2.7 Published on:- 
    Includes all the Features of Version 1.1
    Items Changed
    - Extensive re-assignment of Nextion Identifiers
    - Revised Method to receive Text from the Nextion
    - Rotary Encoder event processing can be either Polled or Interrupt based
    Added Features
    - Guest Loco
      - Touch the Loco Address on the Throttle Page and have instant access to any valid Loco Address
      - Currently Active Loco restored after Guest abandoned
    - Nextion Pages return to the Calling Page
    - Various bug fixes

    V1.2.8 Published on:-
    - Cleaned up version for Public use
    - The formal release of Version 1.2.x
    
    V1.2.9 Published on:- 30 January 2023
    - Added Pre-defined Functions for locos - 2 functions each
    - Added programability of Nextion Keyboard characters on left and right side of Space Bar
      - "Load" button on Config Page performs the update
    V1.3.0
    - Fixed missing Programming "Read" code:-(
    - Made provision for varying Nextion Colour Schemes

Getting Started

This is a brief summary of the README.md file on Github - https://github.com/normhal/DCCppEX-Nextion-Controller

Pin Assignments for UNO (and variants)

    Rotary Encoder
        Reverse Button - Arduino pin D8
        Clock Pulses - Arduino pin D3 for Interrupt option, D6 for Polled
        Data Pulses - Arduino pin D2 for Interrupt option, D7 for Polled
        - Swop Clock and Data to change rotation direction
        Refer to "Config.h" Line 32 to select option

    Nextion Display
        Software Serial RX - Arduino pin D4 to Nextion TX wire (Blue)
        Software Serial TX - Arduino pin D5 to Nextion RX wire (Yellow)

    Connection to Command Station
        Hardware Serial RX - Arduino pin D0 to DCC++, DCC-EX, or HC-12 TX pin
        Hardware Serial TX - Arduino pin D1 to DCC++, DCC-EX, or HC-12 RX pin

Pin Assignments for ESP8266

    Rotary Encoder
        Reverse Button - ESP8266 pin GPIO5
        Clock Pulses - ESP8266 pin GPIO13
        Data Pulses - ESP8266 pin GPIO12
        - Swop Clock and Data to change rotation direction

    Nextion Display
        Software Serial RX - ESP8266 pin GPIO4 to Nextion TX wire (Blue)
        Software Serial TX - ESP8266 pin GPIO14 to Nextion RX wire (Yellow)

    Connection to Command Station (for debug Console or if using Direct Connection to Command Station)
        Hardware Serial RX - ESP8266 pin GPIOx to DCC++, DCC-EX, or HC-12 TX pin
        Hardware Serial TX - ESP8266 pin GPIOx to DCC++, DCC-EX, or HC-12 RX pin

Pin Assignments for ESP32

    Rotary Encoder
        Reverse Button - ESP32 pin GPIO13
        Clock Pulses - ESP32 pin GPIO5
        Data Pulses - ESP32 pin GPIO4
        - Swop Clock and Data to change rotation direction

    Nextion Display
        Hardware Serial RX - ESP32 pin GPIO16 to Nextion TX wire (Blue)
        Hardware Serial TX - ESP32 pin GPIO17 to Nextion RX wire (Yellow)

    Connection to Command Station (for debug Console or if using Direct Connection to Command Station)
        Hardware Serial RX - ESP32 pin GPIOx to DCC++, DCC-EX, or HC-12 TX pin
        Hardware Serial TX - ESP32 pin GPIOx to DCC++, DCC-EX, or HC-12 RX pin

Note Regarding Rotary Encoders

There are a few different versions of Rotary Encoders. Refer to the appropriate Datasheet for specific pin assignments

    Reverse button = "SW"
    Data pulses = "DT"
    Clock pulses = "CLK"

Note Regarding UNO variants

Arduino boards using the ATMega32U4 processor and most mini boards using the ATMega328P chip can be 
programmed to use the "UNO" bootloader which is smaller and yields the same usable memory as a Standard UNO. 
In view of the size of the Arduino Sketch, this change is required to provide sufficient progam memory. 
"Erik" (erik84750) kindly provided the Addendum below for instructions how to replace the UNO bootloader.

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
//  nextion.begin(57600, SERIAL_8N1, 18, 19);     //ESP32 UART2 pins can be re-mapped if desired
  nextion.begin(57600);     //This speed has been found to be suitable for both Hard- and Software Nextion Connections.
  wait(100);
  initPage(CoverPage);
  wait(3000);

  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(buttonPin, INPUT_PULLUP);
  
#if defined RE_INTERRUPTS
  attachInterrupt(digitalPinToInterrupt(pinA), PinA, RISING); //interrupt on PinA, rising edge signal & executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(digitalPinToInterrupt(pinB), PinB, RISING); // same for B
#endif

#if defined ESP || defined PICO_W
  EEPROM.begin(eepromEnd + 1);                   //Same as UNO - for now
  wifiEnabled = readEEPROMByte(eewifiEnabled);
  Serial.print(F("WiFi Enabled Status: "));
  Serial.println(wifiEnabled);
  #if defined WIFI
    if (connectWiFi(readEEPROMByte(eeWiFiRetries)) == 1)
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
  for (uint16_t s = eeSelIDs; s < (eeSelIDs + numLocoSlots); s++)
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
  for (uint8_t i = 0; i < numLocos; i++)
  {
    LocoFN0to4[i] = 128;
    LocoFN5to8[i] = 176;
    LocoFN9to12[i] = 160;
    LocoFN13to20[i] = 0;
    LocoFN21to28[i] = 0;
  }
  //Ensure Edit Mode is OFF
  eMode = 0;
#if defined RE_POLLED
  lastREpinA = digitalRead(pinA);
#endif
}
/*
 **************************************************************************************************************************
 * Main Program Loop
 **************************************************************************************************************************
*/
void loop()
{
  rotaryEncoder();                      // check for rotary encoder activity
  receiveCMD();                         // Receive and Process anything from the Command Station
  refreshDCC();                         // Refresh current Loco if enabled
  checkREButton();                      // check for change in direction from Rotary Encoder
  buttonScheduler();                    // Process any received Nextion data
#if defined WIFI
  checkWiFi();
#endif
}
