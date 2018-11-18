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

#ifndef LAYAD_CIRCUITS_SALENG_GSM_H
#define LAYAD_CIRCUITS_SALENG_GSM_H

#include "Arduino.h"

#include <Stream.h>
#include <SoftwareSerial.h>
enum {
	SMS_STAGE0 = 0,
	SMS_STAGE1,
	SMS_STAGE2,
	SMS_STAGE3,
	SMS_STAGE4,
	SMS_STAGE5,
	SMS_STAGE6,
	SMS_STAGE7,
	SMS_STAGE8	
};
//#define DEBUG_PRINT_TO_MONITOR
#define DELAY_BETWEEN_AT_CMNDS 800
#define SMS_INTERVAL 7000 // time between sms transmissions in ms
#define SMS_FINAL_DELAY_MS 8000 // waiting time before a new SMS can be sent
#define MAX_PHONENUMBER_LEN 14 // maximum length of the phone number
#define MAX_SMS_SIZE 160 // maximum size of an SMS message
#define RECEIVE_BUFFER_SIZE 210

class LayadCircuits_SalengGSM
{
public:
	LayadCircuits_SalengGSM(SoftwareSerial *);
	LayadCircuits_SalengGSM(HardwareSerial *);
    void begin(uint16_t baud);
    void initSalengGSM();
    bool isFreeToSend(); 
	void sendSMS(char * num, char * msg);
    void smsMachine();
    bool receiveSMS();  
    bool smsPoller();
    char smsRxMsg[RECEIVE_BUFFER_SIZE];
    char smsSender[MAX_PHONENUMBER_LEN+1];
	char * readSMS();
	bool isSMSavailable();

private:

	Stream *mySerial;
	 SoftwareSerial *swSerial;
	HardwareSerial *hwSerial;
    char _sms_msg[MAX_SMS_SIZE+1];
    char _sms_num[MAX_PHONENUMBER_LEN+1];
    uint8_t _sms_stage;
	bool smsavailable;
	
};
#endif
