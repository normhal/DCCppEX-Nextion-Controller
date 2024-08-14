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
 *********************************************************************************************************************
 * Process Programming Page
 * Programming Commands supported:
 * Main Line Programming:
 * - Write CV Byte to Loco <w ADDR CV VALUE>
 *   - Bit programming not yet supported
 * Program Track Programming:
 * - Read Loco Address  <R>
 * - Write Loco Address <W>
 * - Read CV Value      <R CV CALLBACKNUM CALLBACKSUB>
 * - Write CV Value     <W CV VALUE>
 * 
 *********************************************************************************************************************
*/
void programPage(uint8_t button)
{
  if(message.startsWith("AD"))
  {
    g_programAddress = message.substring(2).toInt();
    return;
  }
  if(message.startsWith("CN"))
  {
    cv_Num = message.substring(2).toInt();
    return;
  }
  if(message.startsWith("CV"))
  {
    cv_Value = message.substring(2).toInt();
    return;
  }
  if(message.startsWith("C2"))
  {
    cv2_Value = message.substring(2).toInt();
    return;
  }
  if(message.startsWith("C3"))
  {    
    cv3_Value = message.substring(2).toInt();
    return;
  }
  if(message.startsWith("C4"))
  {    
    cv4_Value = message.substring(2).toInt();
    return;
  }
  if(message.startsWith("C5"))
  {
    cv5_Value = message.substring(2).toInt();
    return;
  }
  if(message.startsWith("C6"))
  {    
    cv6_Value = message.substring(2).toInt();
    return;
  }
  switch (button)
  {
    case Done_Press:
    {
      if(readEEPROMByte(eeJoinMode)) sendCMD("<1 JOIN>");
      else sendCMD("<1>");
      initPage(ProgReturnPage);
      break;
    }
    case Cancel_Press:
    {
      if(readEEPROMByte(eeJoinMode)) sendCMD("<1 JOIN>");
      else sendCMD("<1>");
      initPage(ProgReturnPage);
      break;
    }
    case Main_Press:
    {
      progMode = progMain;
      progType = progWrite;
      nextionSetText("S0", "Enter Loco or Acc Address");
      nextionCommand("ADText.pco=" + String(BLACK));
      nextionCommand("AD.pco=" + String(BLACK));
      nextionSetValue("Read", 0);
      nextionSetValue("Write", 1);
      nextionSetText("S1", "Enter Value to Write to CV or");
      nextionSetText("S2", "Specific CV Number and Value ");
      nextionSetText("S3", "Press CV Heading to Program");
      if(readEEPROMByte(eeJoinMode)) sendCMD("<1 JOIN>");
      else sendCMD("<1>");
      break;
    }
    case Prog_Press:
    {
      progMode = progProg;
      nextionSetValue("Write",0);
      nextionCommand("ADText.pco=" + String(GREY));
      nextionCommand("AD.pco=" + String(GREY));
      nextionSetText("S0", "Select Read or Write Mode");
      nextionSetText("S1","");
      nextionSetText("S2","");
      nextionSetText("S3","");
//      sendCMD("<0 PROG>");
      break;
    }
    case Read_Press:
    {
      if(progMode == progMain)
      {
        nextionSetText("S1", "Cannot Read on Main!");
        nextionSetText("S2","");
        nextionSetText("S3","");
        nextionSetValue("Read", 0);
      }else
      {
        progType = progRead;
        nextionSetText("S1", "Press CV Heading to Read or");
        nextionSetText("S2", "Enter Specific CV Number"); 
        nextionSetText("S3", "Press CV Heading to Read");
      }
      break;
    }
    case Write_Press:
    {
      progType = progWrite;
      nextionSetText("S1", "Enter Named CV Value or");
      nextionSetText("S2", "Specific CV Number and Value");
      nextionSetText("S3", "Press CV Heading to Program");
      break;
    }
// The following cases handle Touch activated Text Fields     
    case CVnText_Press:
      if(progType == progRead) readCV(cv_Num);
      else writeCV(cv_Num, cv_Value);
      nextionDataType = CV_VALUE;     
      break;
    case MinText_Press:
      if(progType == progRead) readCV(CV2);
      else writeCV(CV2, cv2_Value);
      nextionDataType = CV2_VALUE;
      break;
    case MidText_Press:
      if(progType == progRead) readCV(CV6);
      else writeCV(CV6, cv6_Value);
      nextionDataType = CV6_VALUE;
      break;
    case MaxText_Press:
      if(progType == progRead) readCV(CV5);
      else writeCV(CV5, cv5_Value);
      nextionDataType = CV5_VALUE;
      break;
    case AccelText_Press:
      if(progType == progRead) readCV(CV3);
      else writeCV(CV3, cv3_Value);
      nextionDataType = CV3_VALUE;
      break;
    case DecelText_Press:
      if(progType == progRead) readCV(CV4);
      else writeCV(CV4, cv4_Value);
      nextionDataType = CV4_VALUE;
      break;
    default:
      break;
  }
}
void readCV(uint8_t cv_Number)
{
  sendCMD("<R " + String(cv_Number) + " 5 6>");
  startTime = millis();             //Start a timeout Timer 
  replyExpected = true;
  nextionSetText("S1","");
  nextionSetText("S2","");
  nextionSetText(F("S3"), F("Waiting for CV to be Read"));
}

void writeCV(uint8_t cv_Number, uint8_t cv_Value)
{
  if(progMode == progMain)
  {
    nextionSetText(F("S0"), F("Main Values Sent"));
    sendCMD("<w " + String(g_programAddress) + " " + String(cv_Number) + " " + String(cv_Value) + ">");
  }else
  {
    nextionSetText(F("S0"), F("Prog Values Sent"));
    sendCMD("<W " + String(cv_Number) + " " + String(cv_Value) + ">");
  }
}
