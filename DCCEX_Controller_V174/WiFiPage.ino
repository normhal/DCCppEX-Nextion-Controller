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
/*
 *************************************************************************************************************************
 * Process Status Page
 *************************************************************************************************************************
*/
void wifiPage(uint8_t button)
{
#if defined WIFI
  if(message.startsWith("SS"))
  { 
    WiFiEnabled = 0;
    writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
    ssid = message.substring(2);
    writeEEPROMName(eeSSID, ssid);
    updateWiFi();
    return;
  }
  if(message.startsWith("PA"))
  {
    WiFiEnabled = 0;
    writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
    password = message.substring(2);
    writeEEPROMName(eePWD, password);
    return;
  }
  if(message.startsWith("IP"))
  {
    WiFiEnabled = 0;
    writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
    host = message.substring(2);
    writeEEPROMName(eeIPAddr, host);
    return;
  }
  if(message.startsWith("PO"))
  {
    WiFiEnabled = 0;
    writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
    port = message.substring(2).toInt();
    writeEEPROMAddr(eePort, port);
    return;
  }
  if(message.startsWith("RC"))
  {
    WiFiRetries = message.substring(2).toInt(); 
    Console.printf("Saving Retries: %d\n\r", WiFiRetries);
    writeEEPROMByte(eeWiFiRetries, WiFiRetries);
    return;
  }
  if(message.startsWith("N3"))
  {
    wifiSeconds = message.substring(2).toInt();
    writeEEPROMByte(eeWiFiSeconds, wifiSeconds);
    return;
  }
  switch (button)
  {
    case ConfigButton:
        if(!EEPROM.commit()) Console.println("EEPROM.commit Failed"); 
        initPage(ConfigPage);
        break;
      case Done_Press:
        saveCredentials();
        if(!EEPROM.commit()) Console.println("EEPROM.commit Failed");
        initPage(MenuPage);
        break;
      case Scan_Press:
        WiFiEnabled = 0;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);
        nextionSetValue("WiFiEN",0);
        listName = SSIDs;
        returnPage = WiFiPage;
        initPage(SelectionPage);
        break;
      case  RetryButton:
        WiFiEnabled = 1;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);
        nextionSetValue("WiFiEN",1);
        nextionSetText("Progress", "Retrying...");
        if(!EEPROM.commit()) Console.println("EEPROM.commit Failed");
        updateWiFi();
        break;
      case WiFiEnabledON:
        WiFiEnabled = 1;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);
        nextionSetText("Progress", "Enabling WiFi...");
        updateWiFi();
        break;
      case WiFiEnabledOFF:
        WiFiEnabled = 0;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
        nextionSetText("Progress", "Disabling WiFi...");
        nextionCommand("Sig.pic=" + String(WIFI_X));
        updateWiFi();
        break;
      default:
        break;
    }
  #endif
}
