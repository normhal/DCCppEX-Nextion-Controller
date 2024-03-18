/*
***********************************************************************************************************
* Nextion Interface
* 
* COPYRIGHT (c) 2022 Norman Halland (NormHal@gmail.com)
* 
* Some ideas come from Perry Bebbington's method to provide a degree of
* "handshaking" between the Nextion and the Microcontroller code.
* 
* Bently Born's Nextion Library was used as a reference for functions.
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
* Provides all the routines used to communicate to and from the Nextion
* 
***********************************************************************************************************
*/

/*
 ***********************************************************************************************************
 * Listen for and Receive char data from the Nextion 
 ***********************************************************************************************************
*/
String nextionReceive(){
  #define ReadBufferLen 30            //provides space for String
  String readData = "";               //ensure buffer is empty
  char readTemp;
  uint8_t readIndex = 0;
  while(nextion.available() > 0)
  {
    readTemp =  nextion.read();
    readData += readTemp;
    readIndex++;
    if (readIndex >= ReadBufferLen) return readData;       //Maximum length of 20
    wait(10);
  }
  return readData;
}
/*
 ********************************************************************************************************************
 * Set the contents of a Nextion Text Field
 ********************************************************************************************************************
*/
void nextionSetText(String component, String detail)
{
  nextionCommand(component + ".txt=\"" + detail + "\"");
  wait(5);
}
/*
 ********************************************************************************************************************
 * Set a Nextion Value (8 bit)
 ********************************************************************************************************************
 */
void nextionSetValue(String component, uint8_t value){
  nextionCommand(component +".val=" + value);
  return;
}
/*
 ********************************************************************************************************************
 * Send a Command to the Nextion
 ********************************************************************************************************************
 */
void nextionCommand(String cmd)
{
  while (nextion.available()) nextion.read();      //read any current data from the Nextion
  nextion.print(cmd);
  nextion.write(0xFF);
  nextion.write(0xFF);
  nextion.write(0xFF);
}
/*
 **************************************************************************************************************************
 * GetText converts Text data received from the Nextion into the correct format and directly updates where intended
 * Possible nextionDataType values - refer line 285 onwards in DCCEX_Throttle.h
 **************************************************************************************************************************
*/
void nextionGetText(String unFilteredText)
{
  String receivedText;
  uint8_t i = 0;
  for(i = 0; i < unFilteredText.length(); i++)
  {
    if(unFilteredText[i] != 0x7c)       //remove random mystery (Termination?) character from Nextion data
    {
      receivedText = receivedText + unFilteredText[i];
    }
  }
  switch (nextionDataType)
  {
    case GUEST_ADDRESS:
      guestAddress = receivedText.toInt();
      break;
    case LOCO_ADDRESS:
      writeLocoAddress(editingID, receivedText.toInt());
      break;
    case ROAD_NAME:
      writeEEPROMName((locoNameBase + (editingID * (locoNameLen))), receivedText);
      break;
    case ROAD_NUMBER:
      writeLocoRNum(editingID, receivedText.toInt());
      break;
    case LOCO_TYPE:
      writeEEPROMName((locoTypeBase + (editingID * (locoTypeLen))), receivedText);
      break;
    case ACC_IMAGE:
      writeEEPROMByte(accImageBase + (editingID), receivedText.toInt());                //and its Image
      break;
    case ACC_ADDRESS:
      writeAccAddress(editingID, receivedText.toInt());                       //and the Address
      break;
    case ACC_NAME:
      writeEEPROMName((accNameBase + (editingID * (accNameLen))), receivedText);
      break;
    case FUNC_NUMBER:
      writeEEPROMByte((locoFuncBase + (editingID * 20)) + (g_fSlot*2), receivedText.toInt());
      break;
    case FUNC_IMAGE:
      writeEEPROMByte((locoFuncBase + (editingID * 20)) + ((g_fSlot*2)+1), receivedText.toInt());
      break;
    case SSID_NAME:
      writeEEPROMName(eeSSID, receivedText);
      ssid = receivedText;
      break;
    case SSID_PASSWORD:
      writeEEPROMName(eePWD, receivedText);
      password = receivedText;
      break;
    case HOST_IP_ADDRESS:
      writeEEPROMName(eeIPAddr, receivedText);
      host = receivedText;
      break;
    case PORT_NUMBER:
      writeEEPROMAddr(eePort, receivedText.toInt());
      port = receivedText.toInt();
      break;
    case WIFI_RETRIES:
      writeEEPROMByte(eeWiFiRetries, receivedText.toInt());
      retries = receivedText.toInt(); 
      break;
    case THRESHOLD_VALUE:
      writeEEPROMByte(eeThreshold, receivedText.toInt());
      break;
    case RE_ACCEL_VALUE:
      writeEEPROMByte(eeREIncrement, receivedText.toInt());
      break;
    case ACC_CMD_GAP:
      writeEEPROMByte(eeAccDelay, receivedText.toInt());
      break;
    case WIFI_TIMER_VAL:
      writeEEPROMByte(eeWiFiSeconds, receivedText.toInt());
      wifiSeconds = receivedText.toInt();
      break;
    case PROGRAM_ADDRESS:
      g_programAddress = receivedText.toInt();
      break;
    case CV_NUMBER:
      cv_Num = receivedText.toInt();
      break;
    case CV_VALUE:
      cv_Value = receivedText.toInt();
      break;
    case CV2_VALUE:
      cv2_Value = receivedText.toInt();
      break;
    case CV3_VALUE:
      cv3_Value = receivedText.toInt();
      break;
    case CV4_VALUE:
      cv4_Value = receivedText.toInt();
      break;
    case CV5_VALUE:
      cv5_Value = receivedText.toInt();
      break;
    case CV6_VALUE:
      cv6_Value = receivedText.toInt();
      break;
    case TFT_IP:
      tftHost = receivedText;
      break;
    case TFT_PORT:
      tftPort = receivedText.toInt();
      break;
    case TFT_NAME:
      url = ('/' + receivedText);
      break;
    case U_SSID:
      ussid = receivedText;
      break;
    case U_PW:
      upassword = receivedText;
      break;
    default:
      break;
  }
}
/*
 ********************************************************************************************************************
 * Get a Value from a numeric Nextion object
 * Code borrowed from Bently Born's Nextion Library
 ********************************************************************************************************************
*/
unsigned int nextionGetValue(String object)
{
  unsigned int value = 0;
  nextionCommand("get "+ object +".val");
  uint8_t temp[8] = {0};
  nextion.setTimeout(20);
  if (sizeof(temp) != nextion.readBytes((char *)temp, sizeof(temp))) return -1;
  if((temp[0]==(0x71))&&(temp[5]==0xFF)&&(temp[6]==0xFF)&&(temp[7]==0xFF))
  {
    value = (temp[4] << 24) | (temp[3] << 16) | (temp[2] << 8) | (temp[1]);     //Little-endian conversion
  }
  return value;
}
/*
 ********************************************************************************************************************
 * Wait passed value of milliseconds
 ********************************************************************************************************************
*/
void wait(uint16_t period)
{
  unsigned long start = millis();
  while((millis()-start < period)){}
}
