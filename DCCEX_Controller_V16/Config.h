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

#ifndef CONFIG_H
  #define CONFIG_H
  #include "ImageDefs.h"
  #include "TFTList.h"

  #if __has_include ("Credentials.h")
    #include "Credentials.h"
  #else
    #warning Credentials.h not found. Using defaults from Credentials_Example.h
    #include "Credentials_Example.h"
  #endif

  //#define DCCPP                   //Un-comment for DCC++ else DCCEX

  #define ENABLE_OTA

  #define NEXTION_UPLOAD          //Enables support for remote updating of Nextion HMI (not supported on Pico yet)

  #define ENABLE_ROTARY_ENCODER //Comment out to DISABLE

  #define WIFI                    //WiFi support can be disabled here

  #define SEND_POWER_STATE        //This option allows the Nextion Controller to send Power State Command at Power-Up

//#define DISPLAY_TAB_DETAILS_GREY_BG     //This option ENABLES displaying Road Name, Number or Address, and Loco Type on ALL Throttle Tabs
  #define DISPLAY_TAB_DETAILS_YELLOW_BG   //As Above, but with a YELLOW background on the Active Tab
                                          //Comment BOTH Lines out to have ONLY the Address or Road Number displayed on the Active Tab

  #define HARD_CODED_VALUES       //Uncomment this line to enable the "Load Hard Coded Details" button on the Config Page

  #define HCL_ALL10            //Uses Specific Throttle Slots for Selected Locos

  #define SHOW_WIFI               //This option shows WiFi commands on the Serial Debug Console for debugging

  #define JOIN_OPTION ON

  #define NEW_LEFT_CHAR "_"       //The character on the Left side of the Nextion Spacebar can be re-defined here
  #define NEW_RIGHT_CHAR "$"      //The character on the Rightt side of the Nextion Spacebar can be re-defined here

  #define WIFISECS 5              //number of seconds between WiFi status refreshes
  
#endif
