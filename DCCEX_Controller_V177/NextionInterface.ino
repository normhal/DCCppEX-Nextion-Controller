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
    if(readTemp == '|')  readTemp = '\0';
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
