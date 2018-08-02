/***************************************************************************
 Example Code to control via SMS the builtin LED onboard the Arduino board wired to pin 13.
 To execute a command, the user sends an SMS containing one of the two keywords available.
 Open the serial monitor and set the baud rate to 9600 to view debug messages.
 
 This is an Arduino example sketch designed specifically to work with the Layad Circuits' Saleng GSM Shield.
 
 This software is free provided that this notice is not removed and proper attribution 
 is accorded to Layad Circuits and its Author(s).
 Layad Circuits invests resources in producing free software. By purchasing Layad Circuits'
 products or utilizing its services, you support the continuing development of free software
 for all.
  
 Author(s): C.D.Malecdan for Layad Circuits Electronics Engineering
 Revision: 1.0 - 2018/06/20 - initial creation
 Layad Circuits Electronics Engineering Supplies and Services
 B314 Lopez Bldg., Session Rd. cor. Assumption Rd., Baguio City, Philippines
 www.layadcircuits.com
 general: info@layadcircuits.com
 sales: sales@layadcircuits.com
 +63-916-442-8565
 ***************************************************************************/
#include <LayadCircuits_SalengGSM.h>
#include <SoftwareSerial.h>

// You may edit these keywords to your liking. Do note the following pointers
// we recommend using at least 4 characters
// avoid pure numeric characters
// shorter keywords must not match characters within a longer keyword (e.g. dont use SALE if you already have a keyword called SALENG)
// do not use the following: OK, ERROR, CMT, CMTI, Ready, ready
#define KEYWORD1 "turnon"
#define KEYWORD2 "turnoff"

SoftwareSerial gsmSerial(2,3);
// if you are using Arduino Mega or if you plan to use Serial 0 (pins D0 and D1), use the next line instead
// #define mySerial Serial1 // define as Serial, Serial1, Serial2 or Serial3

LayadCircuits_SalengGSM salengGSM = LayadCircuits_SalengGSM(&gsmSerial);

void setup()
{
  pinMode(LED_BUILTIN,OUTPUT);
  salengGSM.begin(9600); // this is the default baud rate
  Serial.begin(9600);
  Serial.print(F("Preparing Saleng GSM Shield.Pls wait for 10 seconds..."));
  delay(10000); // allow 10 seconds for modem to boot up and register
  salengGSM.initSalengGSM();
  Serial.println(F("Done"));
  Serial.println(F("Send the keywords \"turnon\" or \"turnoff\" via SMS to turn the builtin LED on or off"));
}

void loop()
{
  salengGSM.smsMachine(); // we need to pass here as fast as we can. this allows for non-blocking SMS transmission
  if(salengGSM.isSMSavailable()) // we also need to pass here as frequent as possible to check for incoming messages
  {
     salengGSM.readSMS(); // updates the read flag
     //Serial.print("Sender=");
     //Serial.println(salengGSM.smsSender);
     //Serial.print("Whole Message=");
     //Serial.println(salengGSM.smsRxMsg); // if we receive an SMS, print the contents of the receive buffer

     
     if(strstr(salengGSM.smsRxMsg,KEYWORD1))// check if keyword 1 was received
     {
      // do something here
       digitalWrite(LED_BUILTIN,HIGH);
     }
     else if(strstr(salengGSM.smsRxMsg,KEYWORD2))// check if keyword 2 was received
     {
      // do something here
       digitalWrite(LED_BUILTIN,LOW);
     }          
  }
}
