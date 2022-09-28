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

  // CAUTION!!! Use the option below with care - enabling this AFTER you've used an UNO without it 
  // can have unpredictable results!!!
  //  #define USE_NEXTION_EEPROM                //Uncomment this line to use Nextion EEPROM (UNOs with Enhanced Nextions ONLY)
                                              //This gives 10 extra Locos, 12 extra Accessories and 18 extra Routes for UNOs

  #define SEND_POWER_STATE                //This option allows the Nextion Controller to send Power State Commands at Power-Up
  
  //#define DISPLAY_TAB_DETAILS_GREY_BG     //This option ENABLES displaying Road Name, Number or Address, and Loco Type on ALL Throttle Tabs
  //#define DISPLAY_TAB_DETAILS_YELLOW_BG   //As Above, but with a YELLOW background on the Active Tab
                                          //Comment BOTH Lines out to have ONLY the Address or Road Number displayed on the Active Tab
                                               
  #define HARD_CODED_VALUES               //Uncomment this line to enable the "Load Hard Coded Details" button on the Config Page
                                            //WARNING: Leaving this option ENABLED for an UNO based Controller results in
                                            //unpredictable behaviour. It is recommended to Comment it out and reflash the UNO after 
                                            //loading the sample Locos, Accessories and Routes 

  // IMPORTANT NOTE:
  // THE CREDENTIALS BELOW ARE ONLY HERE TO INITIALIZE THE EEPROM IN THE DCC++EX CONTROLLER
  // ONCE INITIALIZED YOU NEED TO UPDATE TO SUIT YOUR ENVIRONMENT VIA THE NEXTION "WiFi Status Page"
  //

  #if defined (ESP8266) || (ESP32)
  
    #define WIFI                        //WiFi on ESP32 or 8266 can be disabled here
    #define SHOW_WIFI                   //This option shows WiFi commands on the Serial Debug Console

    String ssid      = "SSID";          //Update these values to suit your own credentials
    String password  = "PASSWORD";      //For the first Boot Only
    String host      = "10.0.0.15";     //DCCEX IP Address
    uint16_t port    = 2560;

  /***********************************************************************************************************/
  // DO NOT MODIFY ANYTHING BELOW THIS LINE EXCEPT TO TAILOR YOUR OWN HARD-CODED DETAILS
  /***********************************************************************************************************/

    #if defined WIFI
      #define WIFI_DEFINED 1
    #else
      #define WIFI_DEFINED 0
    #endif
  #endif

  #if defined AVR_UNO && defined HARD_CODED_VALUES
    #define SAVE_MEMORY
  #endif

  /*
  /***********************************************************************************************************
     * Optional Hard-Coded Loco Details
     * You can specify up to 10 Loco details which can be loaded into the Roster positions 1 to 10
     * To upload, press the "Load Hard Coded Details" button on the "Config" Page
     * Functions for each Loco can be added, edited or deleted via the Loco Edit Page
  /***********************************************************************************************************
  */
  #ifdef HARD_CODED_VALUES

    /***********************************************************************************************************/
    //Up to 10 Locos can be defined
       
    #define numLocoDefs 10    //Don't exceed 10 for an UNO, but can go up to 50 for an ESP:-)
    #define locoNameLen 9     //Don't modify
    #define locoTypeLen 9     //Don't modify

    struct userLoco {
      char locoName[locoNameLen];
      char locoType[locoTypeLen];
      uint16_t roadNum;
      uint16_t address;
    };
    struct userLoco userLocos[numLocoDefs] = 
    { 
        {"Santa Fe", "SD40-2", 5100, 3},        //Format is Name(max 8 chars), Type (max 8 chars), Road Number, Address
        {"UP", "SD40", 5101, 4},
        {"CP Rail", "SD40-3", 5102, 5},
        {"BNSF", "SD40-2", 5103, 6},
        {"CN", "SD80", 5104, 7},
        {"NYC", "SD40-2", 5105, 8},
        {"Chessie", "SD40-2", 5106, 9},
        {"NH", "SD40-2", 5107, 10},
        {"Santa Fe", "SD40-2", 5108, 11},
/*         
        {"Santa Fe", "GP38", 5109, 12}, 
        {"Santa Fe", "SD40-2", 5100, 3},        //Format is Name(max 8 chars), Type (max 8 chars), Road Number, Address
        {"CN", "SD40-3", 5102, 5},
        {"Chessie", "SD40-2", 5106, 9},
        {"Santa Fe", "SD40-2", 5108, 11}, 
        {"BNSF", "SD40-2", 5103, 6},
        {"CP Rail", "SD80", 5104, 7},
        {"NYC", "SD40-2", 5105, 8},
        {"UP", "SD40", 5101, 4},
        {"NH", "SD40-2", 5107, 10},
*/
        {"Santa Fe", "GP38", 5109, 12} 
    };
    /***********************************************************************************************************/
    //Up to 12 Accessories Names can be defined
      
    #define numAccDefs 12       //12 fills one page on the Nextion Controller
    #define accNameLen 9        //Don't Modify
    
    struct userAcc {      
      char accName[accNameLen];
      uint16_t accAddress; 
      uint8_t accImage;
      uint8_t accType;
    };

    // Accessory Images as defined within the Nextion
    //
    #define LH0 202     //Left Hand 0 degrees
    #define LH90 200    //Left Hand 90 Degrees
    #define LH180 206   //Left Hand 180 Degrees
    #define LH270 204   //Left Hand 270 Degrees
    #define RH0 210     //Right Hand 0 degrees
    #define RH90 208    //Right Hand 90 Degrees
    #define RH180 214   //Right Hand 180 Degrees
    #define RH270 212   //Right Hand 270 Degrees

    struct userAcc userAccs[numAccDefs] = {   //Accessory IDs are 0 to 9
      {"ST1-W1", 400, LH0, 0},              //Format is Name (8 Chars max), Address, Image, Type(not used yet) 
      {"ST1-W2", 401, RH180, 0},
      {"ST1-W3", 402, RH0, 0},
      {"ST2-W1", 403, LH180, 0},
      {"ST2-W2", 404, RH0, 0},
      {"ST2-W3", 405, LH180, 0},
      {"ST1-E1", 406, LH0, 0},
      {"ST1-E2", 407, RH180, 0},
      {"ST1-E3", 408, LH0, 0},
      {"ST2-E1", 409, RH180, 0},
      {"ST2-E2", 410, LH0, 0},
      {"ST2-E3", 411, RH180, 0}
    };
  
    /***********************************************************************************************************/
    //Up to 6 Routes can be defined - 6 accessory IDs per route   
    
    #define numRouteDefs 6          //6 Routes per Nextion Page
    #define accessoriesPerRoute 6   //max 6 Accessories per Route
    #define C 0                     //Closed = 0
    #define T 1                     //Thrown = 1

    struct routeElement
    {
      uint8_t accID;
      uint8_t accState;
    };

    struct routeElement routeElements[numRouteDefs][accessoriesPerRoute] = 
    {
      {{0,C}, {1,C}, {2,C}, {6,T}, {7,T}, {8,T}},   //Format of an Element is Acc ID, Required State (C)losed or (T)hrown
      {{0,T}, {1,T}, {2,T}, {6,C}, {7,C}, {8,C}},   //see Accessories above for ID numbers
      {{0,C}, {1,C}, {2,C}, {6,T}, {7,T}, {8,T}},
      {{0,T}, {1,T}, {2,T}, {6,C}, {7,C}, {8,C}},
      {{0,C}, {1,C}, {2,C}, {6,C}, {7,C}, {8,C}},
      {{0,T}, {1,T}, {2,T}, {6,T}, {7,T}, {8,T}}
    };
  #endif
#endif
