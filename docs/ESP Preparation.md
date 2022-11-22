# ESP32 and ESP8266 Preparation

Click on "File", then "Preferences" ont the top row of the Arduino IDE, and then add the following line to the section entitled "Additional Boards Manager URLs" : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json to add support for the ESP32. Version 2.0.5 (or later) of the ESP32 package is required.

For the ESP8266, add http://arduino.esp8266.com/stable/package_esp8266com_index.json to the section entitled "Additional Boards Manager URLs" by either separating it from previous additions with a "comma", or clicking on the icon on the right hand side of the space and adding a new line.

Once you've added one or both of the above, click "OK", then Select "Tools" on the top row, then look for "Board...", and then select "Boards Manager". Wait a few seconds for the page to configure itself, then in the space marked "Filter your search...: Type "esp32" or "esp8266" as appropriate. Click "Install" and when complete "Close" the Boards Manager. Again select "Tools", then either "ESP32 Arduino" or "ESP8266 Boards" and select either "ESP32 Dev Module" or "Generic esp8266 Module". When done, select "Close" and you're ready to compile:-)
