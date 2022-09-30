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
  
  //#define USE_NEXTION_EEPROM     //Uncomment this line to use Nextion EEPROM (UNOs with Enhanced Nextions ONLY)
                                   //This gives 10 extra Locos, 12 extra Accessories and 18 extra Routes for UNOs
  #define HARD_CODED_LOCOS
  //#define HARD_CODED_ACCS
  //#define HARD_CODED_ROUTES

  //IMPORTANT NOTE:
  // THE CREDENTIALS BELOW ARE ONLY HERE TO INITIALIZE THE EEPROM IN THE DCC++EX CONTROLLER
  // ONCE INITIALIZED YOU NEED TO UPDATE TO SUIT YOUR ENVIRONMENT VIA THE NEXTION "WiFi Page"
  //
  #if defined (ESP8266) || (ESP32)
    #define WIFI                    //Enable only for ESP32 or ESP8266
    String ssid      = "SSID";      //Update these values to suit your own credentials
    String password  = "password";  //For the first Boot Only
    String host      = "hostIP";    //192.168.68.100 or 192.168.4.1
    uint16_t port    = 2560;
  #endif

  #if defined HARD_CODED_LOCOS

    /*
     * Optional Hard-Coded Loco Details
     * You can specify up to 10 Loco details which can be loaded into the Roster positions 1 to 10
     * To upload, press the "Load Roster" button on the "Config" Page
     * Functions for each Loco can be added, edited or deleted via the Nextion
    */
    #define numLocoDefs 10           //Change this for the number of locos you wish to hard-code

    String userLocoNames[numLocoDefs]   = {"Santa Fe SD40", "Union Pacific SD40", "CP Rail SD40-2", "Burlington North",        //Max 18 characters per name
                                           "", "", "", "", 
                                           "", ""};
                                           /*, "Loco11", "Loco12",
                                           "Loco13", "Loco14", "Loco15", "Loco16", 
                                           "Loco17", "Loco18", "Loco19", "Loco20"}; */
                                           
    uint16_t userLocoAddrs[numLocoDefs] = {3, 4, 5, 6, 0, 0, 0, 0, 0, 0};
                                          /*,           //Addresses up to 9999
                                          13, 14, 15, 16, 17, 18, 19, 20, 21, 22}; */
                                          
    uint16_t userLocoRNums[numLocoDefs] = {7010, 1211, 5328, 4652, 0, 0, 0, 0, 0, 0};    //Max 4 digits per road number
  
  #endif
  
  #if defined HARD_CODED_ACCS
  
    #define numAccDefs 12
    
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
    uint8_t  userAccImages[numAccDefs] = {202,206,210,212,220,222,214,218,222,212,220,222};
    uint8_t  userAccTypes[numAccDefs] = {0,0,0,0,0,0,0,0,0,0,0,0};    //Types not used (yet)
    
  #endif

  #if defined HARD_CODED_ROUTES

    #define numRouteDefs 2
    #define accessoriesPerRoute 6
    
    uint8_t userRouteIDs[numRouteDefs*accessoriesPerRoute] = {0,1,2,6,7,8,0,1,2,6,7,8};       //Format of a Route is accessoriesPerRoute x Acc IDs, state pairs
    uint8_t userRouteStates[numRouteDefs*accessoriesPerRoute] = {0,0,0,1,1,1,1,1,1,0,0,0};

  #endif

#endif
