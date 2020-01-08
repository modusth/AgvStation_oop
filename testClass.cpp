
#include "testClass.h"
hc12::hc12(int pin, int br, int ch){
  pinMode(pin, OUTPUT);
  
  Serial3.begin(9600);            
  delay(500);
  digitalWrite(pin, LOW);
  Serial.println("> read current setting HC-12");
  Serial3.println("AT+V");
  if(recvReturn()){
    Serial3.println("AT+RX");
    if(recvReturn()){
      Serial.println("> config HC-12... ");
      Serial3.println("AT+B"+ String(br));
      if(recvReturn()){
        String channel = String(ch/100) + String((ch%100)/10) + String(ch%10);
        Serial3.println("AT+C"+channel);
        if(recvReturn()){
          digitalWrite(pin, HIGH);
          Serial.println("success");
        }
      }
    }
  }
  else{
    Serial.println("module HC-12 error!");
    digitalWrite(pin, HIGH);
  }
  
  delay(3000);
  Serial3.begin(9600);            
}

bool hc12::recvReturn(void){
  long timeSendConf = millis();
  bool rfPass = false;
  while (millis() - timeSendConf < 200){
    if (Serial3.available()){
      Serial.write(Serial3.read());
      rfPass = true;
    }
  }  
  return rfPass;
}

String hc12::genPackage(String from, String to, String command, String data){
  static long timeLastSendRf = 0;
  String newMessage = from +to +"," +command;
  if (data != ""){
    newMessage = newMessage +"," +data;
  }
  byte sum = genCheckSum(newMessage);
  byte sum1 = (sum/16);
  char sum1c;
  if (sum1 > 9)   sum1c = char(sum1+55);
  else            sum1c = char(sum1+48);
  byte sum2 = (sum%16); 
  char sum2c;
  if (sum2 > 9)   sum2c = char(sum2+55);
  else            sum2c = char(sum2+48);
  newMessage = "$" +newMessage +"*" +sum1c +sum2c ;

  //Serial.println("> generate package: " + newMessage);
  //Serial.println("sum =" + String(sum) + ", sum1 =" + String(sum1c) + ", sum2 =" + String(sum2c));
  return newMessage;
}

byte hc12::genCheckSum(String data){
  int dataLength = data.length();
  byte sum = 0;
  for(byte i=0; i<dataLength; i++){
    sum = sum^data[i]; 
  }
  return sum;
}

bool hc12::recv(void){
  static byte endPgState = 0;
  while (Serial3.available()){
    if (cntRfData == 0){
      strRfData = "";
    }
    char c = Serial3.read();
    if (cntRfData == 0){
      if (c != '$'){
        return false;
      }
    }
    strRfData += c;
    cntRfData++;          
  
    if (c == 13){
      endPgState = 1;
    }
    else if ((endPgState == 1) && (c == 10)){
      endPgState = 0;
      strRfData.trim();
      if (checkSum(strRfData)){
        Serial.print("< HC-12 received: " + strRfData +"      ");
        extractData(strRfData);
        unReadRfPackage = true;
        return true;
      }
      else{   
        cntRfData == 0;
        strRfData = "";
      }
    }
    else{
      endPgState = 0;
    }
  }
  return false;   
}

bool hc12::checkSum(String data){
  int dataLength = data.length();
  String cutData = data.substring(1, dataLength-3);
  String sumData = data.substring(dataLength-2, dataLength);

  int sum1c;
  if (byte(sumData[0]) >'9')    sum1c = (sumData[0]-55)*16;
  else                          sum1c = (sumData[0]-48)*16;
  int sum2c;
  if (byte(sumData[1]) > '9')   sum2c = sumData[1]-55;
  else                          sum2c = sumData[1]-48;
  int sumDataInt = sum1c + sum2c;

  byte sumCal =0;
  for(byte i=0; i<(dataLength-4); i++){
    sumCal = sumCal^cutData[i]; 
  }
  if (sumDataInt == sumCal){
    return true;
  }
  else{
    Serial.println("[!] Sum of package error: " +strRfData +"  /sumDataInt = " +String(sumDataInt,HEX) +"h  /sumCal = " +String(sumCal,HEX) +"h");
    return false;
  }
}

void hc12::extractData(String data){
  rxTo = data.substring(1,3);
  rxFrom = data.substring(3,5);
  rxCommand = data.substring(6,9);
  if (data.length() > 12){
    //rxData = data.substring(10,14);
    rxData = data.substring(10,data.length()-3);
  }
  else{
    rxData = "";
  }
}

String hc12::to(void) {
  return rxTo;
}

String hc12::from(void) {
  return rxFrom;
}

String hc12::command(void) {
  return rxCommand;
}

String hc12::data(void) {
  return rxData;
}

bool hc12::addMessage(String mess){
  if (availableMessageCount < maxMessage){
    return (addProcess(mess));
  }
  else{
    for (byte i=0; i< maxMessage; i++){
      message[i] = message[i+1];
    }
    availableMessageCount--;
    return (addProcess(mess));
  }
}

bool hc12::addProcess(String mess){
  for(byte i=0; i<availableMessageCount; i++){          // check repleat message
    if (message[i] == mess){
      Serial.println("addProcess: not pass repleat check");
      return false;
    }
  }
  message[availableMessageCount] = mess;                // add message
  availableMessageCount++;
  Serial.println("[i] add message");
  printMessList();
  return true;  
}

void hc12::printMessList(void){
  Serial.println("Message list: [availableMessageCount =" + String(availableMessageCount) + "]");
  for(byte i=0; i<availableMessageCount; i++){
    Serial.println(String(i) + ") " + message[i]);  
  }
  Serial.println("");
}

bool hc12::removeMessage(String mess){
  Serial.println("[i] remove message");
  mess.trim();
  if (availableMessageCount > 0){                       // check matching message
    for(byte i=0; i<maxMessage; i++){
      if (message[i] == mess){
        for(i; i<maxMessage; i++){                      // slide message
          message[i] = (i < (maxMessage-1)) ? message[i+1]: ""; 
        }
        availableMessageCount--;
        arrayPosition = 0;
        if (availableMessageCount == 0) message[0] = "";
        timeLastPcEcho = millis();
        printMessList();
        return true;
      }
    }
  }
  return false;
}

void hc12::sender(char mode, bool rdyToSend, long timeSendInterval, long timeRecvPol, long timeWaitForSend){
  static long timeLastSend = 0;
  bool sendConditionOk = false;
  switch (mode){
    case 't': // send by time
      sendConditionOk = ((millis() - timeLastSend) > (timeSendInterval+(txFrom[1] -48)*3)) ? true: false;
      break;
    case 'p': // send by Seperate Polling
      sendConditionOk = rdyToSend ? true: false;  
      break;
    case 'g': // send by group polling
      sendConditionOk = ((millis() - timeRecvPol > timeWaitForSend) && rdyToSend) ? true: false;
      break;  
  }
  if (sendConditionOk){
    //Serial.println("availableMessageCount ="+String(availableMessageCount));
    if (availableMessageCount > 0){
      if (arrayPosition >= availableMessageCount) arrayPosition = 0;
      Serial3.println(message[arrayPosition]);
      //Serial.println("> sent arrayPosition:"+String(arrayPosition)+", data:"+message[arrayPosition]);
      if (arrayPosition < availableMessageCount){
        arrayPosition++;
      }
    }
    timeLastSend = millis();
  }
}

byte hc12::messageCount(void) {
  return availableMessageCount;
}
