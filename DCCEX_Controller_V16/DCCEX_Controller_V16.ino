/***********************************************************************

  Nextion Based DCC++EX Controller

  COPYRIGHT (c) 2022, 2023, 2024 Norman Halland (NormHal@gmail.com)

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
 * Designed and written by Norman Halland  2022, 2023, 2024
 * 
 ***********************************************************************

  Version History

    V1.6 - Public Release on 17 March 2024
    Features
    - Supports ESP32, ESP8266 and Raspberry Pi Pico processors
    - Nextion Touch or Optional Rotary Encoder operation
    - DCCpp or DCCEX protocols to support DCCpp and DCCEX Command Stations
    - Supports Direct Command Station Connection or WiFI
    - Throttle Page gives instant access to 10 Locos
      - Throttle Page displays up to 10 Loco Road Names, Numbers and Addresses
      - Active Loco Details visible at the top of each Nextion Page
      - Guest Loco
        - Touch the Loco Address on the Throttle Page and have instant access to any valid Loco Address
        - Currently Active Loco restored after Guest abandoned
    - Loco Shed (Inventory/Roster) of up to 50 Locos
      - Each Loco can have any combination (out of 68) of 10 Functions
    - Up to 96 Accessories
    - Up to 48 "Routes"
    - All models support Ops or Program Track Programming
    - Full support for OTA and Remote Nextion upload using modified ESPNexUpload Library
    - Comprehensive User Guide

    V1.6a - 19 March - bugfix
    - Disabled WiFi Status check when WiFi disabled - limit unnecessary data sent out console/Direct Connection
    - Limited refresh of Throttle Page fields to Throttle Page only (prevent field corruption on WiFi Status Page)
*****************************************************************************************************************

Getting Started

This is a brief summary of the README.md file on Github - https://github.com/normhal/DCCppEX-Nextion-Controller

Pin Assignments for ESP8266

    Rotary Encoder
        Reverse Button - ESP8266 pin GPIO5 or D1 on Wemos Mini D1
        Clock Pulses - ESP8266 pin GPIO13 or D7 on Wemos Mini D1
        Data Pulses - ESP8266 pin GPIO12 or D6 on Wemos Mini D1
        - Swop Clock and Data to change rotation direction

    Nextion Display
        Software Serial RX - ESP8266 pin GPIO4 or D2 on Wemos Mini D1 to Nextion TX wire (Blue)
        Software Serial TX - ESP8266 pin GPIO14 or D5 on Wemos Mini D1 to Nextion RX wire (Yellow)

    Connection to Command Station (for debug Console or if using Direct Connection to Command Station)
        Hardware Serial RX - ESP8266 pin RX to DCC++, DCC-EX, or HC-12 TX pin
        Hardware Serial TX - ESP8266 pin TX to DCC++, DCC-EX, or HC-12 RX pin

Pin Assignments for ESP32

    Rotary Encoder
        Reverse Button - ESP32 pin GPIO13
        Clock Pulses - ESP32 pin GPIO5
        Data Pulses - ESP32 pin GPIO4
        - Swop Clock and Data to change rotation direction

    Nextion Display to UART2
        Hardware Serial RX - ESP32 pin GPIO16 to Nextion TX wire (Blue)
        Hardware Serial TX - ESP32 pin GPIO17 to Nextion RX wire (Yellow)

    Connection to Command Station (UART0, for debug Console or if using Direct Connection to Command Station)
        Hardware Serial RX - ESP32 pin GPIO3 (TX0) to DCC++, DCC-EX, or HC-12 TX pin
        Hardware Serial TX - ESP32 pin GPIO1 (RX0) to DCC++, DCC-EX, or HC-12 RX pin

Pin Assignments for RaspBerry Pi Pico and Pico W

    Rotary Encoder
        Reverse Button - Pico pin GPIO13
        Clock Pulses - Pico pin GPIO5
        Data Pulses - Pico pin GPIO4
        - Swop Clock and Data to change rotation direction

    Nextion Display to UART2
        Hardware Serial RX - Pico pin GPIO8 to Nextion TX wire (Blue)
        Hardware Serial TX - Pico pin GPIO9 to Nextion RX wire (Yellow)

    Connection to Command Station (for debug Console or if using Direct Connection to Command Station)
        Hardware Serial RX - Pico pin GPIO3 to DCC++, DCC-EX, or HC-12 TX pin
        Hardware Serial TX - Pico pin GPIO1 to DCC++, DCC-EX, or HC-12 RX pin


Note Regarding Rotary Encoders

There are a few different versions of Rotary Encoders. Refer to the appropriate Datasheet for specific pin assignments

    Reverse button = "SW"
    Data pulses = "DT"
    Clock pulses = "CLK"

=**************************************************************************************************************************
*/
#if __has_include ("Hard_Coded_Values.h")
  #include "Hard_Coded_Values.h"
#else
  #warning Hard_Coded_Values.h not found. Using defaults from Credentials_Example.h
  #include "Hard_Coded_Values_Example.h"
#endif
#include "DCCEX_Controller.h"
/*
**************************************************************************************************************************
* Setup Code
**************************************************************************************************************************
*/
void setup()
{
  console.begin(115200);     //For the direct connection to DCC++ or DCC-EX
  wait(250);
  nextion.begin(57600);     //This speed has been found to be suitable for both Hard- and Software Nextion Connections.
//  nextion.begin(115200);     //This speed has been found to be suitable for both Hard- and Software Nextion Connections.
//  nextion.begin(115200, SERIAL_8N1, 16, 17);     //ESP32 UART2 pins can be re-mapped if desired
  wait(250);
  nextion.flush();

  #if defined ENABLE_ROTARY_ENCODER
    // Setup pins and Interrupts for Rotary Encoder
    pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
    pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
    pinMode(REButtonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pinA), PinA, CHANGE); //interrupt on PinA, rising edge signal & executing the "PinA" Interrupt Service Routine (below)
    attachInterrupt(digitalPinToInterrupt(pinB), PinB, CHANGE); //same for B
  #endif

  enum List listName;
  
  EEPROM.begin(eepromSize);                 
  if (EEPROM.read(eepromEnd) != EEPROMCODE) initEEPROM();       //Test to see if EEPROM has been initialized before
  uint8_t r = 0;
  for (uint16_t s = eeSelIDs; s < (eeSelIDs + numLocoSlots); s++)
  {
    selectedIDs[r] = readEEPROMByte(s);
    r++;
  }
  joinMode = readEEPROMByte(eeJoinMode);
  wifiSeconds = readEEPROMByte(eeWiFiSeconds);
  
initPage(CoverPage);
wait(100);
nextionSetText("Version", Version);
wait(2000);

activeSlot = readEEPROMByte(eeActiveSlot);
WiFiEnabled = readEEPROMByte(eeWiFiEnabled);
#if defined WIFI
  if(WiFiEnabled == 1)
  {
    console.println("WiFi is Enabled");
    readCredentials();
    console.println("Connecting With: ");
    console.println(ssid);
    console.println(password);
    WiFiClient client;
    retries = readEEPROMByte(eeWiFiRetries);
    if (connectWiFi(retries) == 1)
    {
      int i=0;
      console.println(F("WiFi NOT connected! "));
      console.print(F("IP address: "));
      console.println(WiFi.localIP());
      console.print(F("RSSI: "));
      console.println(WiFi.RSSI(i));
    }
    wait(100);
  }
#endif
#if defined SEND_POWER_STATE
  if (readEEPROMByte(eePUState) == 1) powerONButton();    // Set both the Nextion and Command Station Power State
  else powerOFFButton();
#endif

initPage(MenuPage);                       //Display the Menu Page first after the Cover Page/
nextionCommand("bkcmd=0");    //Suppress Error details from the Nextion

//Initialize Function State arrays for all possible Locos
#if defined DCCPP
  for (uint8_t i = 0; i < numLocos; i++)
  {
    LocoFN0to4[i] = 128;
    LocoFN5to8[i] = 176;
    LocoFN9to12[i] = 160;
    LocoFN13to20[i] = 0;
    LocoFN21to28[i] = 0;
  }
#endif
//Ensure Edit Mode is OFF
eMode = 0;

#if defined NEW_LEFT_CHAR
  nextionSetText("keybdA.b241",NEW_LEFT_CHAR);
  wait(10);
#endif
#if defined NEW_RIGHT_CHAR
  nextionSetText("keybdA.b243",NEW_RIGHT_CHAR);
  wait(10);
#endif
}
/*
 **************************************************************************************************************************
 * Main Program Loop
 **************************************************************************************************************************
*/
void loop()
{
  #if defined ENABLE_OTA
    ArduinoOTA.handle();
  #endif
  #if defined ENABLE_ROTARY_ENCODER
    checkREButton();                      // check for change in direction from Rotary Encoder
  #endif
  updateSpeed();                        // check for speed change
  receiveCMD();                         // Receive and Process anything from the Command Station
  refreshDCC();                         // Refresh current Loco if enabled
  buttonScheduler();                    // Process any received Nextion data
#if defined WIFI
    if((millis() - wifiTimer) >= (wifiSeconds * 1000))
    {
      checkWiFi();
    }
#endif
}
