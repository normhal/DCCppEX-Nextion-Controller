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
  #define DISPLAY_TAB_DETAILS_YELLOW_BG   //As Above, but with a YELLOW background on the Active Tab
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
    //Up to 10 Locos can be defined for an UNO or 50 for ESP
       
    #define numLocoDefs 20    //Don't exceed 10 for an UNO, but can go up to 50 for an ESP:-)

    String userLocoNames[numLocoDefs]   = {"Santa Fe", "UP", "CP Rail", "BNSF", "UP"        //Max 8 characters per name
                                          ,"Santa Fe", "UP", "CP Rail", "CP Rail", "BNSF"
      #if numLocoDefs == 20
                                          ,"UP", "CP Rail", "UP", "Santa Fe", "UP"
                                          ,"CP Rail", "CP Rail", "BNSF", "BNSF", "UP"
      #endif
                                          };
                                          
    String userLocoTypes[numLocoDefs]   = {"SD40-2", "SD40-2", "SD40-3", "SD40-2", "SD80"        //Max 8 characters per type
                                          ,"SD40-2", "SD40-2", "SD40-3", "SD80", "SD40-2"
      #if numLocoDefs == 20
                                          ,"SD40-2", "SD40-3", "SD80", "SD40-2", "SD40-2"
                                          ,"SD40-3", "SD40-3", "SD40-2", "SD40-3", "SD40-2"
      #endif
                                          };
    //Loco Addresses
    uint16_t userLocoAddrs[numLocoDefs] = {3, 4, 5, 6, 7
                                           ,8, 9, 10, 11, 12
      #if numLocoDefs == 20
                                           ,13, 14, 15, 16, 17
                                           ,18, 19, 20, 21, 22
      #endif
                                           };

    //Loco Road Numbers                                      
    uint16_t userLocoRNums[numLocoDefs] = {5108, 5109, 5110, 5111, 5112 
                                           ,5113, 5114, 5115, 5116, 5117
      #if numLocoDefs == 20
                                           ,5118, 5119, 5120, 5121, 5122
                                           ,5123, 5124, 5125, 5126, 5127
      #endif
                                           };    //Max 4 digits per road number

    /***********************************************************************************************************/
    //Up to 12 Accessories Names can be defined
      
    #define numAccDefs 12       //12 fills one page on the Nextion Controller
    
    String userAccNames[numAccDefs] = {"ST1-W1",
                                       "ST1-W2",
                                       "ST1-W3",
                                       "ST2-W1",
                                       "ST2-W2",
                                       "ST2-W3",
                                       "ST1-E1",
                                       "ST1-E2",
                                       "ST1-E3",
                                       "ST2-E1",
                                       "ST2-E2",
                                       "ST2-E3"};
    
    uint16_t userAccAddrs[numAccDefs] = {400,401,402,406,407,408,403,404,405,409,410,411};

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

    uint8_t  userAccImages[numAccDefs] = {LH0,RH180,RH0,LH180,RH0,LH180,LH0,RH180,LH0,RH180,LH0,RH180};
    uint8_t  userAccTypes[numAccDefs] = {0,0,0,0,0,0,0,0,0,0,0,0};    //Types not used (yet)

    /***********************************************************************************************************/
    //Up to 6 Routes can be defined - 6 accessory IDs per route   
    
    #define numRouteDefs 2          //6 Routes per Nextion Page

    #define accessoriesPerRoute 6
    #define C 0                     //Closed = 0
    #define T 1                     //Thrown = 1
    
    uint8_t userRouteIDs[numRouteDefs*accessoriesPerRoute]    = {0,1,2,6,7,8,0,1,2,6,7,8};       //Format of a Route is accessoriesPerRoute x Acc IDs, state pairs
    uint8_t userRouteStates[numRouteDefs*accessoriesPerRoute] = {C,C,C,T,T,T,T,T,T,C,C,C};

  #endif
#endif
