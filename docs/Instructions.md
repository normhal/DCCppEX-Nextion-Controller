# Getting the Arduino Sketch

Firstly, you will need to have the Arduino IDE installed on your computer. There are many sites which can guide you to getting ready to compile the code for yourself, so I won't try to "re-invent the wheel".

In summary, you will need to set the IDE up to support the processor you intend to use.
For Arduino UNOs and its variants, all you need is to download and install the IDE from http://arduino.cc. As of this writing you have the choice of either the older, "tried-and-tested" Version 1.8.19, or the  * New * Version 2.0. Both versions are suitable, so it's a matter of personal choice which one to use:-) Once installed, all that is required is that you select the board you intend to use - no additional libraries are required.

If using an ESP32 or ESP8266, there are some additional steps you need to take to prepare to compile for them.

Complete the actions in the files below before continuing

# Downloading the Sketch

Github offers two ways for you to get the code. Either click on the "Code" button and download the "Zip" file. This is by far the simplest and most straight forward method, so download the zipped file (about 9MB at this time) and extract to your preferred location. You will end up having a folder called "DCCppEX-Nextion-Controller-main". This folder will contains Version 1.0 and 1.1 of the Controller Sketch (Version 1.0 is simply to show the development hisotry - use Version 1.1:-)), and this document in a "Docs" folder. 

Move the "DCCEX_Controller_V1.1" to your Arduino Sketch folder. Keep the name as the Arduino IDE requires that the name of the folder is identical to the Master Sketch file.

# Building and Flashing the Sketch

Once this is done you are ready to compile. Open the Arduino IDE and open the folder titled "DCCEX_Controller_V1.1", You will see a bunch of files ending with ".ino". The one you want is "DCCEX_Controller_V1.1.ino". When you open this file, all the other files are automatically included and you're ready to compile.

Connect your Arduino to a USB port, select the correct board, and Serial Port, and press the "Upload" button (->) at the top left corner of the IDE. This will start the compile and within a minute or two if all is well, the IDE will start flashing the Arduino. The ESP and ESP8266 models take a bit longer, but should compile without error.




