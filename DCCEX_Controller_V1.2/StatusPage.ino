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
  #if !defined SAVE_MEMORY && defined ESP
    switch (button)
    {
      case ConfigButton:
        #if defined ESP || defined PICO_W
          EEPROM.commit();
        #endif
        initPage(ConfigPage);
        break;
      case DoneButton:
        writeEEPROMByte(eewifiEnabled, wifiEnabled);                
        #if defined ESP || defined PICO_W
          EEPROM.commit();
        #endif
        updateWiFi();
        initPage(MenuPage);
        break;
      case  RetryButton:
        wifiEnabled = 1;
        writeEEPROMByte(eewifiEnabled, wifiEnabled);
        initPage(StatusPage);
        nextionSetText("Progress", "Retrying...");
        #if defined ESP || defined PICO_W
          EEPROM.commit();
        #endif
        updateWiFi();
        break;
      case WiFiEnabledON:
        wifiEnabled = 1;
        writeEEPROMByte(eewifiEnabled, wifiEnabled);
        nextionSetText("Progress", "Enabling WiFi...");
        #if defined ESP || defined PICO_W
          EEPROM.commit();
        #endif
        updateWiFi();
        break;
      case WiFiEnabledOFF:
        wifiEnabled = 0;
        writeEEPROMByte(eewifiEnabled, wifiEnabled);                //WiFi default
        nextionSetText("Progress", "Disabling WiFi...");
        #if defined ESP || defined PICO_W
          EEPROM.commit();
        #endif
        updateWiFi();
        break;
      case SSID_Press:
        nextionDataType = SSID_NAME;
        break;
      case PW_Press:
        nextionDataType = SSID_PASSWORD;
        break;
      case IP_Press:
        nextionDataType = HOST_IP_ADDRESS;
        break;
      case Port_Press:
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
