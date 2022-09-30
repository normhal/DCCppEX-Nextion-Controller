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
  switch (button)
  {
    case ConfigButton:
      initPage(ConfigPage);
      break;
    case DoneButton:
      updateWiFi();
      initPage(MenuPage);
      break;
    case WiFiEnabledON:
      wifiEnabled = 1;
      writeEEPROMByte(eewifiEnabled, wifiEnabled);                //WiFi default
      #if defined ESP
        EEPROM.commit();
      #endif
      updateWiFi();
      break;
    case WiFiEnabledOFF:
      wifiEnabled = 0;
      writeEEPROMByte(eewifiEnabled, wifiEnabled);                //WiFi default
      #if defined ESP
        EEPROM.commit();
      #endif
      updateWiFi();
      break;
    default:
      break;
  }
}
/*
 *********************************************************
 * Update WiFi Credentials
 *********************************************************
*/
void updateCredentials()
{
  #if defined ESP
    writeEEPROMByte(eeWiFiRetries, nextionGetText("Tries").toInt());
    ssid = nextionGetText("SID");
    password = nextionGetText("PW");
    host = nextionGetText("IP");
    Serial.print(F("IP: "));
    Serial.println(host);
    port = nextionGetText("PRT").toInt();
    Serial.print(F("Port: "));
    Serial.println(port);
    writeEEPROMName(eeSSID, ssid);
    writeEEPROMName(eePWD, password);
    writeEEPROMName(eeIPAddr, host);
    writeEEPROMAddr(eePort, port);
    EEPROM.commit();
  #endif
}
