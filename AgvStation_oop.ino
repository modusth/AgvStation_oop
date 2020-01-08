// High resolution: 256 * 64

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>
#define  OLED_RESET 4
//#include "hc12.h";
#include "buzzer.h";
#include "testClass.h";
#include <libmaple/iwdg.h>
Adafruit_SSD1306 display(OLED_RESET);

const int     swCall    =PB5;
const int     swCancel  =PB9;
const int     swEmer    =PC14;
const int     lpCall    =PB14;
const int     lpCancel  =PB15;

int           totalQueue =0;
int           myQueue =0;
int           callState =0;         // 0 =off, 1 =call, 2 =agv docking
bool          received =true;
bool          EmerStop =false;
bool          waitAgv  =false;
bool          rdyToSend =false;
long          timeRecvPol;
long          timeWaitForSend;

String rxFrom     = "";
String rxTo       = "";
String rxCommand  = "";
String rxData     = "";
String txFrom     ="L2";            // <--------------- edit Line No.

bool swStateCall =false;
bool sentReturn =false;
bool swStateCancel =false;
bool swActCancel =false;
bool swStateEmer =false;
bool swActEmer =false;
bool swDeActEmer =false;

hc12 hc12(PB13,115200,10);            // pin, br, ch

void setup() {
  buzzInit();
  swInit();
  dpInit();
  dpShowWelcome();
  Serial.begin(115200);             // Debug
  //buzzOn(1);
  //hc12Init(13,9600,10);           // use HC-12
  //buzzOn(0);
  
  //delay(1000);
  timeWaitForSend = (txFrom[1] - 48)*25+80;
  //Serial.println("timeWaitForSend =" +String(timeWaitForSend));
  
  Serial.println("system start...\n");
  iwdg_init(IWDG_PRE_16, 75000);    // Timeout Watchdog = (1/40kHz) *16*(reload) = 30S
  //addMessage(genPackage("R1", txFrom, "RSN", ""));
  hc12.addMessage(hc12.genPackage("R1", txFrom, "RSN", ""));
}

void loop() {
  dpShowTitle();
  dpShowQueue();
  dpShowReady();
  dpRefresh(500, 30000);

  swAction();

  /*hc12Recv();
  rxDataProcess();
  hc12Sender('t',500);*/

  if (hc12.recv()) {
    rxTo      = hc12.to();
    rxFrom    = hc12.from();
    rxCommand = hc12.command();
    rxData    = hc12.data();
    rxDataProcess();
  }
   

  periodReq(10000);
  //loopTimeReport();
  iwdg_feed();
}


void periodReq(long timeWait){
  static long timeLastReq;
  String txCommand = "";
  if (hc12.messageCount() == 0){
    if (millis() - timeLastReq > timeWait){
      received = true;
      dpShowSignal();
      display.display();
      if (EmerStop)   txCommand = "EST";
      else            txCommand = "NOR";          
      Serial3.println(hc12.genPackage("R1", txFrom, txCommand, ""));
      timeLastReq = millis();
      received = false;
      dpShowSignal();
      display.display();      
    }
  }
  else {
    timeLastReq = millis();
  }
}

void rxDataProcess(void){
    if ((rxFrom == "R1") && ((rxTo == txFrom) || (rxTo == "XX"))){
      if ((rxCommand == "POL") && (rxTo == "XX")){
        rdyToSend =true;
        timeRecvPol = millis();
        Serial.println("received polling");
      }
			else if (rxCommand == "RSN") {
				hc12.removeMessage(hc12.genPackage("R1", txFrom, "RSN", ""));
			}
			else if (rxCommand == "REQ") {
        if (rxData != ""){
          int a = (rxData[0]-48)*10 + (rxData[1]-48);
          int b = (rxData[2]-48)*10 + (rxData[3]-48);
          if (((0 < a) && (a < 100)) && ((0 < b) && (b < 100))){
            myQueue = a;
            totalQueue = b;
          }
          if (myQueue != 0){                                      // reload status from Center display
            waitAgv = true;
            callState =1;
            digitalWrite(lpCall,HIGH);
            digitalWrite(lpCancel,LOW);
          }
        }
        hc12.removeMessage(hc12.genPackage("R1", txFrom, "REQ", ""));
      }
      else if (rxCommand == "REJ"){
        hc12.removeMessage(hc12.genPackage("R1", txFrom, "REJ", ""));
        digitalWrite(lpCancel,LOW);
        myQueue =0;
        totalQueue =0;
      }
      else if (rxCommand == "EST"){
        hc12.removeMessage(hc12.genPackage("R1", txFrom, "EST", ""));
      }
      else if (rxCommand == "NOR"){
        hc12.removeMessage(hc12.genPackage("R1", txFrom, "NOR", ""));
      }
      else if (rxCommand == "RST"){
        Serial3.println(hc12.genPackage("R1", txFrom, "RST", ""));
        resetProcess();
      }
    }
    else if ((rxFrom == "A1") && (rxTo == txFrom)){
      if (rxCommand == "RET"){
        waitAgv =false;
        digitalWrite(lpCall,LOW);
        myQueue =0;
        totalQueue =0;
      }
    }
    else{
      Serial.println("incorrect address");
    }
}
