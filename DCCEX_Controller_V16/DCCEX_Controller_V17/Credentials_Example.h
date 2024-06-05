//Credentials for the WiFi "Status" Page
//If DCCEX is in AP mode SSID and Password should be to connect to DCCEX
//If DCCEX is in STA mode they should be to connect to the Local Network
String ssid       = "ssid";
String password   = "password";
String host       = "10.0.0.19";        //The IP Address of DCCEX
uint16_t port     = 2560;

//Credentials for Nextion Update
//IF DCCEX is in AP mode, these credentials should be for the Local Network 
//and will also be used for OTA and to connect to the HFS http server for 
//the Nextion Upload

String ussid      = "ussid";
String upassword  = "upassword";
String tftHost    = "10.0.0.13";        //The IP Address of the HFS http tft server
String url        = "";                 //select this via the Nextion "Update" Page :-)
uint16_t tftPort  = 80;                 //This value you can get from the HFS HTTP File Server as well...
