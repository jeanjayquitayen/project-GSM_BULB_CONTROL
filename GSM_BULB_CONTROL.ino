#include "LiquidCrystal_I2C.h"  //This is the library we will use to control the LCD
#include "GPRS_Shield_Arduino.h" //This is the library we will use to control the GSM
#include <SoftwareSerial.h> 
#include <Wire.h>
//SERIAL PIN ASSIGNMENT, BAUDRATE, PHONE NUMBER, MESSAGE
#define PIN_TX    3
#define PIN_RX    2
#define BAUDRATE  9600
#define MESSAGE_LENGTH 160

char message[MESSAGE_LENGTH];
int messageIndex = 0;
char phone[16];
char datetime[24];
char PHONE_NUMBER[]= "09567389688";
char MESSAGE[] =   "System is Ready.";
char bulb1ON[] = "bulb1 on";
char bulb2ON[] = "bulb2 on";
char err[] = "Sorry, You have entered an invalid keyword.";
const char Bulb1 = 7; //Bulb1 will be the assigned name of the actual pin (D7) on the arduino. 
const char Bulb2 = 8; //Bulb2 will be the assigned name of the actual pin (D8) on the arduino. 
bool Bulb1State = false; //we will use this to monitor the status of Bulb1
bool Bulb2State = false; //we will use this to monitor the status of Bulb2
GPRS GSMTEST(PIN_RX,PIN_TX,BAUDRATE);//RX,TX,BAUDRATE
LiquidCrystal_I2C lcd(0x27,20,4);

void setup() {

  pinMode(Bulb1,OUTPUT); //declare Bulb1 as output, this will allow us to trigger the relay.
  pinMode(Bulb2,OUTPUT); //declare Bulb2 as output, this will allow us to trigger the relay.
  digitalWrite(Bulb1,LOW); //let's makes sure that Bulb1 is off, so we set it to low.
  digitalWrite(Bulb2,LOW); //let's makes sure that Bulb2 is off, so we set it to low.

  
  Serial.begin(9600);
  Serial.setTimeout(10);
  lcd.init();
  lcd.backlight();
  
  lcd.print("BULB STATUS");
  lcd.setCursor(0,1);
  lcd.print("B1:OFF");
  lcd.setCursor(8,1);
  lcd.print("B2:OFF");
  while(!GSMTEST.init()) {
      delay(1000);
      //digitalWrite(7,LOW);
      Serial.print("INIT ERROR\r\n");
  } 
Serial.print("Initialization Successful!");
GSMTEST.sendSMS(PHONE_NUMBER,MESSAGE);
//DEFINE PHONE NUMBER AND TEXT
delay(1000);
}

void loop() {
   messageIndex = GSMTEST.isSMSunread();
   if (messageIndex > 0) { //AT LEAST, THERE IS ONE UNREAD SMS

        readSMS();
        LowerCase(message);
        if(strcmp(message,"bulb1 on")==0){
          if(!Bulb1State) ToggleBulb(Bulb1,HIGH,&Bulb1State);
        }
        else if(strcmp(message,"bulb1 off")==0){
          if(Bulb1State) ToggleBulb(Bulb1,LOW,&Bulb1State);
        }

       else if(strcmp(message,"bulb2 on")==0){
          if(!Bulb2State) ToggleBulb(Bulb2,HIGH,&Bulb2State);
        }

      else if(strcmp(message,"bulb2 off")==0){
          if(Bulb2State) ToggleBulb(Bulb2,LOW,&Bulb2State);
        }
      else{
        GSMTEST.sendSMS(phone,err);
      }
   }

   

}

void readSMS(){
        GSMTEST.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);           
      //IN ORDER NOT TO FUL SIM Memory, IS BETTER TO DELETE IT
      GSMTEST.deleteSMS(messageIndex);
      //Serial.print("FROM NUMBER: ");
      Serial.println(phone);  
      //Serial.print("DATE/TIME");
      Serial.println(datetime);        
      //Serial.print("RECEIVED MESSAGE: ");
      Serial.println(message);
      //GSMTEST.sendSMS(PHONE_NUMBER,MESSAGE);
}

void LowerCase(char *ch){
  
        for(int i=0;i<strlen(ch);i++){
          if(ch[i] >= 'A' && ch[i] <= 'Z') ch[i] = ch[i] + 0x20;
      }
}

void ToggleBulb(const int bulb,bool pinState,bool *flag){

  digitalWrite(bulb,pinState);
  *flag = pinState;
  if(bulb == Bulb1){
      lcd.setCursor(3,1);
      if(Bulb1State) lcd.print("ON ");
      else lcd.print("OFF ");
  }
  else if(bulb == Bulb2){
      lcd.setCursor(11,1);
      if(Bulb2State) lcd.print("ON ");
      else lcd.print("OFF ");
  }

}
