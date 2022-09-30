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
* A packet from the Nextion is:
* 3 x 0x5a sync/identification bytes
* 1 x byte padding (not used yet) - reserved for a possible future idea
* x bytes making up a String
* 
***********************************************************************************************************
*/

/*
 ***********************************************************************************************************
 * Listen for and Receive char data from the Nextion 
 ***********************************************************************************************************
*/
String nextionReceive(){
  #define ReadBufferLen 25            //provides space a String of 20 bytes
  String readData = "";               //ensure buffer is empty
  bool syncFound = 0;                 //reset identifier counter
  static uint8_t readIndex = 0;
  static uint8_t syncCount = 0;
  char readTemp;
  const uint8_t syncChar = 0x5a;      //the sync character
  readIndex = 0;

  while(nextion.available() > 0){
    readTemp =  nextion.read();
    wait(2);
    if(readTemp == syncChar){
      ++syncCount;
      if(syncCount > 2){                 //3 sync characters received
        syncFound = 1;
        readData = "";
        readIndex = 0;
        syncCount = 0;                  //reset the sync counter
      }
    }else syncCount = 0;
    if(syncFound == 1) {
      if(readIndex >=2) (readData += readTemp);
      ++readIndex;
    }
    if (readIndex >= 20) return readData; // readIndex = ReadBufferLen - 1; 
  }
  return readData;
}
/*
 ********************************************************************************************************************
 * Request the contents of a Nextion Text Field
 ********************************************************************************************************************
*/
String nextionGetText(String hiddenObject){              //Hidden Button which sends required Data
  wait(NEX_WAIT);
  nextionCommand("click " + hiddenObject + ",1");                     //
  String tempStr = "";
  wait(NEX_WAIT);
  tempStr = nextionReceive();
  return tempStr;
}
/*
 ********************************************************************************************************************
 * Set the contents of a Nextion Text Field
 ********************************************************************************************************************
*/
void nextionSetText(String component, String detail){
  nextionCommand(component + ".txt=\"" + detail + "\"");
}
/*
 ********************************************************************************************************************
 * Set a Nextion Value
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
void nextionCommand(String cmd){
  while (nextion.available()) nextion.read();      //read any current data from the Nextion?
  nextion.print(cmd);
  nextion.write(0xFF);
  nextion.write(0xFF);
  nextion.write(0xFF);
}
/*
 ********************************************************************************************************************
 * Wait passed value of milliseconds
 ********************************************************************************************************************
*/
void wait(uint16_t period){
  unsigned long start = millis();
  while((millis()-start < period)){ }
}
