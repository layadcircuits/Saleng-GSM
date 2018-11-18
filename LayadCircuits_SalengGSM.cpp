/***************************************************************************
 This is an Arduino library designed specifically to work with the Layad Circuits' Saleng GSM Shield.
 
 This software is free provided that this notice is not removed and proper attribution 
 is accorded to Layad Circuits and its Author(s).
 Layad Circuits invests resources in producing free software. By purchasing Layad Circuits'
 products or utilizing its services, you support the continuing development of free software
 for all.
  
 Author(s): C.D.Malecdan for Layad Circuits Electronics Engineering
 Revision: 1.0 - 2017/08/17 - initial creation
 Layad Circuits Electronics Engineering Supplies and Services
 B314 Lopez Bldg., Session Rd. cor. Assumption Rd., Baguio City, Philippines
 www.layadcircuits.com
 general: info@layadcircuits.com
 sales: sales@layadcircuits.com
 +63-916-442-8565
 ***************************************************************************/
#include "Arduino.h"
#include "LayadCircuits_SalengGSM.h"
#include <SoftwareSerial.h>

LayadCircuits_SalengGSM::LayadCircuits_SalengGSM(SoftwareSerial *ss) {

  hwSerial = NULL;
  swSerial = ss;
  mySerial = swSerial;
}

LayadCircuits_SalengGSM::LayadCircuits_SalengGSM(HardwareSerial *hs) {
  swSerial = NULL;
  hwSerial = hs;
  mySerial = hwSerial;
}



void LayadCircuits_SalengGSM::begin(uint16_t baudrate) {
  delay(100); 
  if (hwSerial) hwSerial->begin(baudrate);
  if (swSerial) swSerial->begin(baudrate);
  #ifdef DEBUG_PRINT_TO_MONITOR
  Serial.begin(9600); 
  #endif
}

// This function has to be sent only after an intial delay of around 10 seconds to allow
// SIM800 to fully boot up and register into the GSM network
// this is a blocking function but should normally be inside setup()
void LayadCircuits_SalengGSM::initSalengGSM() 
{
	mySerial->println(F("ATE0"));
	delay(DELAY_BETWEEN_AT_CMNDS);
	mySerial->println(F("AT+CMGF=1"));
	delay(DELAY_BETWEEN_AT_CMNDS);
	mySerial->println(F("AT+CNMI=2,2,0,0"));
	delay(DELAY_BETWEEN_AT_CMNDS);
}

void LayadCircuits_SalengGSM::sendSMS(char * num, char * msg) 
{
	sprintf(_sms_msg,msg);
	sprintf(_sms_num,num);
	_sms_stage = SMS_STAGE1;
}

// returnes true if modem is clear to send a new SMS
bool LayadCircuits_SalengGSM::isFreeToSend() 
{
	if(_sms_stage == SMS_STAGE0) return true;
	else return false;
}






bool LayadCircuits_SalengGSM:: smsPoller()
{
  unsigned long timer;
  String textData = "";

  if(!mySerial->available()) return false;
    
  timer = millis();
 
  while(millis() - timer < 500) // acquire data
  {
      if(mySerial->available())
      {
        char inData = mySerial->read();
        textData += inData;
      }
  } 
  textData.trim();                                                         
     
  #ifdef DEBUG_PRINT_TO_MONITOR
  Serial.print(F("SMS received from ")); 
  #endif

  memset(smsSender,0,MAX_PHONENUMBER_LEN+1);
  memset(smsRxMsg,0,RECEIVE_BUFFER_SIZE);
  textData.toCharArray(smsRxMsg,RECEIVE_BUFFER_SIZE);
  String textSender = textData.substring(1,(textData.indexOf("\"")-1));                        //get sender from the message
  textSender.trim();
  textSender.toCharArray(smsSender,MAX_PHONENUMBER_LEN+1);

 char *ptr = strstr(smsRxMsg,"+CMT:");
 
 if(!strstr(smsRxMsg,"+CMT:")) return; // not an SMS
 else smsavailable = true;
 
  // only acquire numeric data.
 for(byte i=0;i<MAX_PHONENUMBER_LEN;i++)
 {
  char c = *(ptr+7+i);
  if((c>='0' && c<='9') || (c=='+'))
  {
    smsSender[i] = c;
  }
  else
  {
    smsSender[i] = 0;
    break;
  }
 }

  #ifdef DEBUG_PRINT_TO_MONITOR
  Serial.print(F("Sender: "));
  Serial.println(smsSender); 
  Serial.print(F("Message: "));
  Serial.println(smsRxMsg); 
  #endif
    
  // re-initialize
  initSalengGSM();
  delay(DELAY_BETWEEN_AT_CMNDS);
  mySerial->print(F("AT+CMGDA=\"DEL ALL\"\r\n")); 
}








// keep this inside the main loop. it has to be executed at least every 100ms or faster
void LayadCircuits_SalengGSM::smsMachine() 
{
  char temp[32]; // temporary array
  static unsigned long timergsm;
  static unsigned long t; 
  
  smsPoller();
  
  if(millis() - t < 100) return;
  t = millis();
  
  
  switch(_sms_stage)
  {
    case SMS_STAGE0: // modem is idle at this moment
    return;

    case SMS_STAGE1:
      mySerial->print(F("AT+CMGF=1\r\n"));
	  #ifdef DEBUG_PRINT_TO_MONITOR
      Serial.print(F("sending SMS...")); 
	  #endif
      timergsm = millis(); 
      _sms_stage = SMS_STAGE2;  
    break;
    
    case SMS_STAGE2:
      if((millis() - timergsm) >= 1000) // check if 1 seconds has elapsed since stage 1
      {
        _sms_stage = SMS_STAGE3; 
      }
    break;
    
    case SMS_STAGE3:
      memset(temp,0,32);// clear this array
      sprintf(temp, "AT+CMGS=\"%s\"\r\n",_sms_num);
      mySerial->print(temp); 
	  #ifdef DEBUG_PRINT_TO_MONITOR
      Serial.print(F("Destination Number:")); 
	  Serial.println(_sms_num); 
	  #endif
      _sms_stage = SMS_STAGE4;   // move to next stage
      timergsm = millis(); // store current time
    break;
    
    case SMS_STAGE4:
      if((millis()-timergsm)>=1000) // check if 2 seconds has elapsed since stage 3
      {
        _sms_stage = SMS_STAGE5;   // move to next stage
      }
    break;
    
    case SMS_STAGE5:
    _sms_msg[MAX_SMS_SIZE]=0; // null terminate the sms message
    mySerial->print(_sms_msg);
    mySerial->print(F("\x1A"));// send last command to initiate SMS transmission
	#ifdef DEBUG_PRINT_TO_MONITOR
	Serial.print(F("Message:")); 
	Serial.println(_sms_msg); 
	#endif
    _sms_stage = SMS_STAGE6;   // move to next stage
    timergsm = millis();     // record current time
    break;
    
    case SMS_STAGE6:
    if((millis()-timergsm)>=SMS_FINAL_DELAY_MS)  // check if SMS_FINAL_DELAY_MS ms has elapsed since stage 5
    {
      _sms_stage = SMS_STAGE7;   // move to next stage
    }
    break;
    
    case SMS_STAGE7:
	initSalengGSM();
	#ifdef DEBUG_PRINT_TO_MONITOR
	Serial.println(F("Done. Ready to send next SMS")); 
	#endif
    _sms_stage = SMS_STAGE0; // move back to idle stage
    break;
  }
}



bool LayadCircuits_SalengGSM:: isSMSavailable()
{
	return smsavailable;
}

char* LayadCircuits_SalengGSM:: readSMS()
{
	smsavailable = false;
	return smsRxMsg;
}






