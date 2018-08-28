# Saleng-GSM
This is an Arduino library for the Saleng GSM Shield from Layad Circuits. With proper pin change, this will work with any SIM800/SIM900 based boards


Function Definitions

void begin(uint16_t baud)
- Input Parameters:
  - baud - the baud rate of the SIM900/SIM800 module
- Description: initializes baudrate of the serial port


void initSalengGSM();
    
- Description: this function sends initial AT commands to the GSM module. As of this writing, the initial commands are meant to disable local echo, set SMS to text mode and to automatically receive SMS from the module via serial port.

bool isFreeToSend()
- Description: Returns true if a certain number of seconds has passed since the last SMS sent out. Otherwise, it returns false. This informs the user that the module is ready to send out a new SMS. 

void sendSMS(char * num, char * msg)
- Input Parameters:
   - *num - a pointer to a null-terminated char array containing the phone number where the SMS will be sent
   - *msg - a pointer to a null-terminated char array containing the message to be sent out. Limit this to 160 characters per message.
- Description: this initiates the transmission of an SMS but does not send out the AT commands. The actual AT commands for transmission are done in smsMachine()

void smsMachine()  
- Description: This is the state machine that implements the set of AT commands required to send out an SMS. This will generate the message based on the phone number and message defined in the last calling of sendSMS(). The Sketch must loop around this function at least every one second to properly work. This also waits for incoming SMS messages. 

bool receiveSMS() 
- Description: If an SMS is received, that is, if isSMSavailable() returns true, the message is copied into the receive buffer smsRxMsg and the phone number of the sender is copied to smsSender.

  
char * readSMS()
- Description: This resets the flag being returned by isSMSavailable() to false. This should be called every time the received SMS has already been proceed by the user's application
	
bool isSMSavailable()
- Description: This checks for incoming SMS messages received from the module. It returns true if an SMS was just received and returns false when otherwise. This must be called as frequent as possible in the loop to monitor incoming SMS.
