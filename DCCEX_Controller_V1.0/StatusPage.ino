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
    case RetryButton:
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
    writeEEPROMByte(eeWiFiRetries, (myNextion.getComponentText("Retries", NEXWAIT)).toInt());
    writeEEPROMByte(eewifiEnabled, myNextion.getComponentValue("WiFi"));
    wifiEnabled = myNextion.getComponentValue("WiFi");
    writeEEPROMName(eeSSID, String(myNextion.getComponentText("SSID", NEXWAIT)) + '\0');
    writeEEPROMName(eePWD, String(myNextion.getComponentText("Password", NEXWAIT)) + '\0');
    writeEEPROMName(eeIPAddr, String(myNextion.getComponentText("HostIP", NEXWAIT)) + '\0');
    writeEEPROMAddr(eePort, (myNextion.getComponentText("Port", NEXWAIT)).toInt());
    ssid = String(myNextion.getComponentText("SSID", NEXWAIT)).c_str();
    password = String(myNextion.getComponentText("Password", NEXWAIT)).c_str();
    host = String(myNextion.getComponentText("HostIP", NEXWAIT)).c_str();
    port = String(myNextion.getComponentText("Port", NEXWAIT)).toInt();
    EEPROM.commit();
  #endif
}
