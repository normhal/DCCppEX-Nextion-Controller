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
void statusPage(uint8_t button)
{
#if defined WIFI
  switch (button)
  {
    case ConfigButton:
        if(!EEPROM.commit()) console.println("EEPROM.commit Failed"); 
        initPage(ConfigPage);
        break;
      case Done_Press:
        saveCredentials();
        if(!EEPROM.commit()) console.println("EEPROM.commit Failed");
        initPage(MenuPage);
        break;
      case Scan_Press:
        WiFiEnabled = 0;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);
        nextionSetValue("WiFi",0);
        listName = SSIDs;
        returnPage = StatusPage;
        initPage(SelectionPage);
        break;
      case  RetryButton:
        WiFiEnabled = 1;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);
        
        nextionSetText("Progress", "Retrying...");
        if(!EEPROM.commit()) console.println("EEPROM.commit Failed");
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
        updateWiFi();
        break;
      case SSID_Press:
        WiFiEnabled = 0;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
        nextionDataType = SSID_NAME;
        updateWiFi();
        break;
      case PW_Press:
        WiFiEnabled = 0;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
        nextionDataType = SSID_PASSWORD;
        break;
      case IP_Press:
        WiFiEnabled = 0;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
        nextionDataType = HOST_IP_ADDRESS;
        break;
      case Port_Press:
        WiFiEnabled = 0;
        writeEEPROMByte(eeWiFiEnabled, WiFiEnabled);                //WiFi default
        nextionDataType = PORT_NUMBER;
        break;
      case Retries_Press:
        nextionDataType = WIFI_RETRIES;
        break;
      default:
        break;
    }
  #endif
}
