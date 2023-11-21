# Nextion Based DCC++EX Controller

A Nextion Based Controller for DCC++ and DCC-EX.

Designed and written by Norman Halland  2022
  
# My Design Objective

What started out as a project to modify Dave Bodnar's original Nextion Throttle while trying to learn "C" and a bit of "C++" has turned out to be a Controller for both DCC++ and DCC-EX Command Stations using either Direct Serial or WiFi Connections, all based on the Nextion Display range. 

A basic requirement was that it would run on as many platforms as possible and thus use limited "Device Specific" features. Arduino variants from an Arduino Pro Mini, up to ESP32 devices, STM32, and others, should be able to be used, with minimal design and feature restrictions.

IMHO, the Nextion range of displays offer a great blend of features, performance and ease of use. Nextion ".tft" binaries are available for Basic and Enhanced Displays of 3.2, 3.5 and 5.0 inch sizes. Some experimental work has been done for the 4.3 inch variants if anyone is interested. In view of a previous (negative) experience, the Nextion HMI "Source" files are available only on request.

The DCC++ protocol was chosen in order to be compatible with both DCC++ and DCC-EX Command Stations. A future (ESP32 or Pico W only) version is being considered and will possibly only be for DCC-EX environments.

# Disclaimer:-(
  
I do not claim to be either a "C" nor "C++" programmer. Apart from the initial "work" I previously made available for the "Extensions to Dave Bodnar's Nextion Throttle", everything in this version represents my first personal venture into the world of "C" and a little "C++". Any critique of my code and design style is expected, welcomed and appreciated:-). I would especially welcome suggestions, ideas, and ways to do things "better", so please be free to forward any
thoughts or comments!
  
# History

Dave Bodnar originally designed a Throttle for the DCC++ system based on the Nextion display and used an Arduino UNO as the supporting platform. Dave's initial version was the basis of the version I made available around 2017. That version added some features and sported a revised Nextion HMI. This ***New*** version is essentially a re-design and re-write of everything, except for a few snippets of Dave's original code. Full acknowledgement, credit and gratitude is thus given to Dave for his work.

Subsequent to the first version I made available around 2017, little further development was done, until late in 2021 when a fellow hobbyist (Erik) started asking me some questions:-) This triggered a re-visit of the "Throttle" and I was again tempted to build on the sketchy knowledge I had gained the first time round. After many hours writing, re-writing, thinking, re-thinking, experimenting, abandoning, getting frustrated, feeling elated, and a thousand other experiences, the current version I have called the Nextion based "DCC++EX Controller" emerged. Two other hobbyists, "esfeld" (Steve) and "jbsorocaba" (Francisco) joined Erik and I as a team to discuss, test, critique and suggest ideas. Version 1.0 and now Version 1.1 represent the result of 9 months of fun and contributions by all. Recently, "mtncityman" joined the team and his "new" eyes have added valuable input for the latest Version 1.2.8.

At the outset, any thought of WiFi was outside the scope of my understanding of both "C", let alone as a communication medium. The evolution of my original goal resulted in a device with the capabilities and support as in "Features" summarized below.

# Supported Arduino Boards
 
-  UNO, including the Nano and similar mini boards - Direct and Wireless connection via Serial port and HC-12
   - Refer to the Addendum below for some UNO considerations
-  ESP8266 - WiFi and direct serial connection, and extended capacity features supported
-  ESP32 - WiFi and Direct Serial Connection, and is the preferred processor for the Arduino Sketch

# Features
  
- Nextion Displays - Basic or Enhanced version of 3.2, 3.5 and 5.0in models with similar functionality
- Direct Serial or WiFi connection (WiFi for ESP8266 and ESP32 variants)
- "Throttle" page giving direct and "instant" access to up to 10 Locos (all Arduino processor variants)
- "Inventory" of locos - 10 Locos for UNOs, up to 50 for ESP models all supporting:-
  - Loco Name, Road Number, Address, and supported functions (10 function "slots" for any of 28 functions)
- "Accessories" - from 36 for UNO models up to 96 for ESPs
- "Routes" - basic setting of up to 6 accessories per route - 6 Routes for UNOs, up to 48 routes for ESPs
  
All configuration options are done via the Nextion - No changes to the Arduino Sketch are required
   
# Getting Started
  
What You'll Need
  
- A Configured DCC++ Basic or DCC-EX Command Station:-)
- A Nextion Display of your choice. The Enhanced models provide a few extra capabilites when using any UNO variant (Extra EEPROM:-)).
- A Host Processor for the Arduino Sketch - Arduino UNO (and variants), ESP8266 or ESP32 (preferred).
  
# Optional Features

- Rotary Encoder
  - See pin connections below. Can be used in parallel with the Nextion Slider
- Wireless Connection
  - 2 x HC-12 communication boards connected to the UNO's serial port can provide a wireless link between the Base Station and the Controller
- An ESP8266 as the Host processor
  - This will connect to a DCC-EX Base Station (STA or AP mode) using the DCC++ protocol
  - Software Serial is used to interface the Nextion to the ESP8266. This has proved to be slightly un-reliable so is not the preferred processor.
- An ESP32 as the Host processor (Recommended Configuration)
  - The amount of integrated Memory, additional Hardware Serial Port and WiFi make this the processor of choice
  - In addition to facilitating WiFi, its range of features and relativly affordable cost, contribute to its strengths
  
# Connecting it All Together
 
# Pin Assignments for UNO (and variants)
  
- Rotary Encoder
  - Reverse Button        - Arduino pin D8
  - Clockwise Pulses      - Arduino pin D3 or D7 if not using Interrupts (a Config.h option)
  - AntiClockwise Pulses  - Arduino pin D2 or D6 if not using Interrupts (a Config.h option)

- Nextion Display
  - Software Serial RX    - Arduino pin D4 to Nextion TX wire (Blue)
  - Software Serial TX    - Arduino pin D5 to Nextion RX wire (Yellow)

- Connection to Command Station
  - Hardware Serial RX    - Arduino pin D0 to DCC++, DCC-EX, or HC-12 TX pin 
  - Hardware Serial TX    - Arduino pin D1 to DCC++, DCC-EX, or HC-12 RX pin 

# Pin Assignments for ESP8266
 
- Rotary Encoder
  - Reverse Button        - ESP8266 pin GPIO5
  - Clockwise Pulses      - ESP8266 pin GPIO13
  - AntiClockwise Pulses  - ESP8266 pin GPIO12

- Nextion Display
  - Software Serial RX     - ESP8266 pin GPIO4 to Nextion TX wire (Blue)
  - Software Serial TX     - ESP8266 pin GPIO14 to Nextion RX wire (Yellow)
  
- Connection to Command Station (for debug Console or if using Direct Connection to Command Station)
  - Hardware Serial RX     - ESP8266 pin GPIOx to DCC++, DCC-EX, or HC-12 TX pin 
  - Hardware Serial TX     - ESP8266 pin GPIOx to DCC++, DCC-EX, or HC-12 RX pin 
   
# Pin Assignments for ESP32

- Rotary Encoder
  - Reverse Button        - ESP32 pin GPIO13
  - Clockwise Pulses      - ESP32 pin GPIO5
  - AntiClockwise Pulses  - ESP32 pin GPIO4

- Nextion Display
  - Hardware Serial RX    - ESP32 pin GPIO16 to Nextion TX wire (Blue) 
  - Hardware Serial TX    - ESP32 pin GPIO17 to Nextion RX wire (Yellow) 

- Connection to Command Station (for debug Console or if using Direct Connection to Command Station)
  - Hardware Serial RX    - ESP32 pin GPIOx to DCC++, DCC-EX, or HC-12 TX pin 
  - Hardware Serial TX    - ESP32 pin GPIOx to DCC++, DCC-EX, or HC-12 RX pin 
  
# Note Regarding Rotary Encoders

There are a few different versions of Rotary Encoders. Refer to the appropriate Datasheet for specific pin assignments

If you're using one of the Rotary Encoder modules shown below, these are the connections required:
  - Reverse button = "SW"
  - Clockwise pulses = "DT"
  - Anticlockwise pulses = "CLK"

![rotary_encoder_reduced](https://user-images.githubusercontent.com/14205909/193545784-de124fde-ba3b-4e78-a921-31cfb93faf23.jpg)

# Note Regarding UNO variants

Arduino boards using the ATMega32U4 processor and most mini boards using the ATMega328P chip can be programmed to use the "UNO" bootloader which is smaller and yields the same usable memory as a Standard UNO. In view of the size of the Arduino Sketch, this change is required to provide sufficient progam memory. "Erik" (erik84750) kindly provided the Addendum below for instructions how to replace the UNO bootloader. 
    
# Compiling the Arduino Sketch
  
Refer to the "Instructions.md" file in the "Docs" folder to configure the Arduin IDE as well as to Download, Build and Flash the Arduino Sketch.

The "Config.h" file in the sketch is the ONLY file you MIGHT need to modify for your preferences. All operational configuration values can be modified via the "Config" and "WiFi Status" pages of the Nextion. 
    
Once the Arduino IDE has been set up for the intended host processor, no additional libraries are needed, although the UNO and ESP8266 versions need the Software Serial library. 
    
The Nextion interface uses my own implementation using ideas from the Bently Born Nextion Library. 

# How To's
 
Once everything is connected and Power is applied to the DCC++EX Controller, you'll need to do some basic configuration.
TIP: At any stage, touching the Center portion of the Top of any page of the Nextion will take you back to either the Throttle or the Menu Page.

At first the "Cover" page will be displayed for a few seconds, followed by the "Menu" page. If using WiFi, the "WiFi Status" Page will be displayed after the Cover Page to allow for configuration changes or confiirmation of WiFi Status.
 
# How To Populate the Locomotive "Shed" or Inventory. 
  Two methods are provided:-
  - If Enabled before compilation, it is possible to use sample "Hard-Coded" Locos, Accessories and Routes.
    Available memory limits haw many can be Pre-Configured - UNOs are very constrained:-(
    - Select the "Config" option from the Menu, then select the "Load Pre-Defined Details" option
      - This will very quickly populate the first Inventory entries with the number of basic Loco details defined in "Config.h" for subsequent editing. 
        Functions and their characteristics must be done manually. Default is 10 Locos.
      - In addition a page of Accessories and two very simple "Routes" are loaded
  or
  - Select the "Locos" page, highlight one of the "Slots" by touching the row of the "Slot" you wish to update, then press the "Edit" button
    - The "Loco Edit" page will be presented for completion.
      - Press "Done" and the "Locomotive Shed" page will be displayed again, ready for further additions.

Highlighting an Inventory Row and pressing the "Select" button will pass that loco's details to the active Loco "Tab" on the "Throttle" page and the loco can then be operated.

# How To Populate the Accessory page
  - Select the "Acc" page 
    - Can be called from the Menu, Throttle, or Routes pages
    - Enable "Edit" mode by pressing the "Edit" button
    - Press one of the 12 (invisible) Accessory "Slots"
    - Enter the Accessory DCC address first (May NOT be 0!)
    - In the current Version, only "DCC" type accessories are supported
    - Press the Blank image below the Image heading to select the desired Accessory Image
      - Select the desired image for the Accessory being edited/created 
    - Enter an optional short description for the Turnout - handy when defining a Route:-)
      - You can test the operation of the accessory before completing
    - Press the "Done" button
    - Enter more Accessories as required. "Edit" mode remains active until de-selected.
    - Existing Accessories can be edited in the same way
      - To Delete an Accessory, set its Address to Zero 
    - Depending on the Processor Type in use, up to 8 pages of Accessories can be defined
    - You can test or operate the Accessories after Edit mode is disabled
  
# How To Define routes
  - Select the "Routes" page
    - Can be called from the Menu, Throttle, or Acc pages
    - Enable "Edit" mode by pressing the "Edit" button
    - Press the Route button for the Route you wish to edit or create
    - The "Acc" page will be presented with "Select" mode enabled
      - "Select" mode is only valid when called from the Route page
    - Leave "Select" mode active while setting and selecting accessories as follows:-
      - Set the desired Accessory to the state you need the Route to set it to, by pressing the Accessory Image
      - Press the Accessory Name or Address to include it in the Route
        - up to 6 accessories can be included in the Route
        - Selected Accessories will have their background colour changed to Yellow
      - Press the "Done" button to save the Route
    - Press the Route Button to execute the "Route"
    - "Editing" a route means "Re-Creating" it:-)
     
# Have Fun:-)
  
See the Nextion "Credits", "Info" and "Tips" pages for Acknowledgements and a few other items.

# Addendum

Instructions for Replacing the Bootloader to increase Available Program memory on some Arduino variants kindly provided by Erik (erik84750)

[ATMega328P.Bootloaders.pdf](https://github.com/normhal/DCCppEX-Nextion-Controller/files/13425770/ATMega328P.Bootloaders.pdf)

