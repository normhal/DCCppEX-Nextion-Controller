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
 **************************************************************************************************************************
 *  Process the handling of Downloading and installing an updated
 *  Nextion "TFT" file.
 *  Once all required fields and values have been provided, the routine
 *  diconnects from the existing Network (In case the Controller was connected to 
 *  a DCCEX in AP mode) and connects to the network which has the HFS server containing
 *  the updated "TFT" file. 
 *  The default name for the file is "newTft.tft" but can be overwritten.
 *  Action according to its button message
 *  First look for any action common to all Pages
 *  Thereafter direct Button input to Routine appropriate to active Nextion Page
 **************************************************************************************************************************
*/
void nextionUpload(uint8_t button)
{
  #if defined WIFI  && defined NEXTION_UPLOAD
    updated = false;
    if(message.startsWith("SS"))
    {
      ussid = message.substring(2);
      return;
    }
    if(message.startsWith("PA"))
    {
      upassword = message.substring(2);
      return;
    }
    if(message.startsWith("IP"))
    {
      tftHost = message.substring(2);
      return;
    }
    if(message.startsWith("PO"))
    {
      tftPort = message.substring(2).toInt();
      return;
    }
    if(message.startsWith("TN"))
    {
      url = ('/' + message.substring(2));
      return;
    }
    switch(button)
    {
      case Cancel_Press:
      #if defined ESP32 || defined ESP8266
        if(restartNeeded == true) ESP.restart();
      #elif defined PICO_W
        if(restartNeeded == true) rp2040.restart();
      #endif
        else 
        {
          client.connect(host.c_str(), port);
          initPage(ConfigPage);
        }
        break;
      case Scan_Press:
        listName = uSSIDs;
        returnPage = UpdatePage;
        initPage(SelectionPage);
        break;
      case TFT_Press:
        listName = TFTs;
        returnPage = UpdatePage;
        button=0;                   //make sure previous button value is invalid
        initPage(SelectionPage);
        break;
       case UploadButtonOn:
      {
        nextionSetText("Line1", "Connecting to HFS Server...");
        nextionSetText("Line2", "Press Proceed when Connected");
        nextionSetText("Line3", "");
        nextionSetText("Line4", "Waiting for Connection...");
        connectOTAUpload();
        break;
      }
      case UploadButtonOff:
      {
        nextionSetText("Line1", "Starting Update");
        nextionSetText("Line2", "");
        nextionSetText("Line3", "");
        nextionSetText("Line4", "");
        wait(500);
        doUpload();
        break;
      }
      case OTA_Button_On:
      {
        nextionSetText("Line1", "Preparing for OTA...");
        nextionSetText("line2", "Ready for OTA when Connected");
        connectOTAUpload();
        break;
      }
      case OTA_Button_Off:
      {
        nextionSetText("Line1", "OTA Cancelled");
        nextionSetText("Line2", "");
        nextionSetText("Line3", "");
        nextionSetText("Line4", "");
        break;
      }
      default:
        break;
    }
  #endif
}
/*
 *********************************************************************************************
 * Prepare Controller for OTA or Nextion Update Transfer
 *********************************************************************************************
*/
bool connectOTAUpload()
{
#if defined WIFI
  if(ussid != ssid)
  {
    nextionSetText("Line1", "Disconnecting from Existing Network");
    nextionSetText("Line4", "WiFi Disconnected...");
    restartNeeded = true;
    client.stop();                                   //Terminate Client connection
    wait(100);
    WiFi.disconnect();                               //disconnect WiFi connection
    wifiImage = WIFI_X;    
    wait(100);
    WiFi.begin(ussid.c_str(), upassword.c_str());
    wait(100);
    WiFi.mode(WIFI_STA);
    wait(100);
    nextionSetText("Line1", "Connecting to New SSID");
    Console.print("SSID being used: ");
    Console.println(ussid);
    Console.print("Password being used: ");
    Console.println(upassword);
    wifiTimeout = (millis() + (retries * 500));
    nextionSetText("Line4", "Waiting for Connection...");
    return(true);
  }else                     //SSID == ussid
  {
    restartNeeded = false;
    if(client.connected())  return(true);
    nextionSetText("Line1", "Trying to Connect...");
    Console.print("uSSID being Tried: ");
    Console.println(ussid);
    Console.print("uPassword being Tried: ");
    Console.println(upassword);
    WiFi.begin(ussid.c_str(), upassword.c_str());
    wifiTimeout = (millis() + (retries * 500));
    nextionSetText("Line4", "Waiting for Connection...");
  }
  return(true);
#endif
}
/*
 *****************************************************************************************************************
 * Do Everything for the Upload to the Nextion
 * Connect, Fetch and Upload
 *****************************************************************************************************************
 */
void doUpload()
{
#if defined NEXTION_UPLOAD && defined WIFI
  Console.print("Connecting to ");
  Console.println(tftHost);
  nextionSetText("Line1","Connecting to:");
  nextionSetText("Line2",tftHost);
  WiFiClient client;
  wait(500);
  if (!client.connect(tftHost.c_str(), tftPort))
  {
    Console.println("Connection failed");
    nextionSetText("Line3", "Connection Failed");
    return;
  }else{
    Console.println("Connected!");
    nextionSetText("Line3", "Connected!");
  }
    Console.println("Requesting URL: " + url);
    nextionSetText("Line4", "Fetching " + url); //Requesting URL:" + url);
    nextionSetText("Line5", "Do Not Interrupt Upload!");
    wait(100);
    unsigned long timeout = millis();
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + tftHost.c_str() + "\r\n" +
               "Connection: close\r\n\r\n");
    while(client.available() == 0)
    {
      if(millis() - timeout > 1000)
      {
        Console.println(">>> Client Timeout!");
        nextionSetText("Line4", "Client Timeout!");
        client.stop();
        return;
      }
    }
    int contentLength;
    int code;
    Console.println("Getting File Details...");
    nextionSetText("Line4", "Starting Download");
    nextionSetText("Line5", "Don't Interrupt File Transfer!");
    wait(100);
    nextion.end();
    while(client.available())
    {
      String line = client.readStringUntil('\n');
      if(line.startsWith("HTTP/1.1 "))
      {
        line.remove(0, 9);
        code = line.substring(0, 3).toInt();
        if(code != 200)
        {
          line.remove(0, 4);
          Console.println(line);
          break;
        }
      }else{
        if(line.startsWith("Content-Length: "))
        {
          line.remove(0, 16);
          contentLength = line.toInt();
        }else{
          if(line == "\r")
          {
            line.trim();
            break;
          }
        }
      }  
    }
    // Update the nextion display
    if(code == 200)
    {
      Console.println("File received. Update Nextion...");
      bool result;
      // initialize NextionUpload
      wait(1000);
      NextionUpload nextion1(115200);
      wait(100);
      // prepare upload: setup serial connection, send update command and send the expected update size
      result = nextion1.prepareUpload(contentLength);
      if(!result)
      {
        Console.println("Error: " + nextion1.statusMessage);
      }else{
        result = nextion1.upload(client);
        if(result)
        {
          updated = true;
          Console.println("\nSuccesfully updated Nextion!");
        }else{
          Console.println("\nError updating Nextion: " + nextion1.statusMessage);
        }
        // end: wait(delay) for the nextion to finish the update process, send nextion reset command and end the serial connection to the nextion
        nextion1.end();
      }
    }
  Console.println("Closing connection\n");
  #if defined ESP32 || defined ESP8266
     ESP.restart();
  #elif defined PICO_W
    rp2040.restart();
  #endif
#endif
}
