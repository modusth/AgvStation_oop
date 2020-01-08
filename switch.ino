void swInit(void){
  pinMode(swCall,INPUT_PULLUP);
  pinMode(swCancel,INPUT_PULLUP);
  pinMode(swEmer,INPUT_PULLUP);  
  pinMode(lpCall,OUTPUT);
  pinMode(lpCancel,OUTPUT);
  digitalWrite(lpCall,LOW);
  digitalWrite(lpCancel,LOW);
}

void swAction(void){
  static long timePushReset;
  if (!digitalRead(swCall)&&(!digitalRead(swCancel))){            // reset
    if (millis() - timePushReset >4000){
      timePushReset = millis();
    }
    if (millis() - timePushReset >2000){
      digitalWrite(lpCall,HIGH);
      digitalWrite(lpCancel,HIGH);
      resetProcess();
      while (!digitalRead(swCall) || (!digitalRead(swCancel)));        
    }
    return;
  }
  else{
    timePushReset = millis()+4000;
  }
  if (!digitalRead(swCall)){                                      // call 
    if (!swStateCall && !waitAgv){
      swStateCall =true;
      waitAgv = true;
      callState =1;
      digitalWrite(lpCall,HIGH);
      digitalWrite(lpCancel,LOW);
      Serial.println("swCall");
      hc12.removeMessage(hc12.genPackage("R1", txFrom, "REJ", ""));
      hc12.addMessage(hc12.genPackage("R1", txFrom, "REQ", ""));
    }
    else if (waitAgv){
      if (!sentReturn){
        Serial3.println(hc12.genPackage("A1", txFrom, "RET", ""));
        sentReturn =true;
      }
    }
  }
  else{
    swStateCall =false;
    sentReturn =false;
  }
/*  
  switch (callState){
    case 0:
      digitalWrite(lpCancel,HIGH);
      digitalWrite(lpCall,LOW);
      removeMessage(genPackage(txTo, txFrom, "REQ", ""));
      addMessage(genPackage(txTo, txFrom, "REJ", ""));
      break;
    case 1:
      digitalWrite(lpCall,HIGH);
      digitalWrite(lpCancel,LOW);
      removeMessage(genPackage(txTo, txFrom, "REJ", ""));
      addMessage(genPackage(txTo, txFrom, "REQ", ""));
      break;   
    case 2:
      // call sw blinking
      break; 
  }*/
  if (!digitalRead(swCancel)){                                    // Cancel
    if (!swStateCancel){
      swStateCancel =true;
      swActCancel =true;
      Serial.println("swCancel");
      digitalWrite(lpCancel,HIGH);
      hc12.removeMessage(hc12.genPackage("R1", txFrom, "REQ", ""));
      digitalWrite(lpCall,LOW);
    }
  }
  else{
    if (swStateCancel){
      swStateCancel =false;
    }
  }
  if (swActCancel){
    hc12.addMessage(hc12.genPackage("R1", txFrom, "REJ", ""));
    swActCancel = false;
    waitAgv = false;
  }

  if (digitalRead(swEmer)){                                       // Emergency
    if (!swStateEmer){
      swStateEmer =true;
      swActEmer =true;
      Serial.println("swEmer");
      hc12.removeMessage(hc12.genPackage("R1", txFrom, "NOR", ""));
    }
  }
  else{
    if (swStateEmer){
      swStateEmer =false;
      hc12.removeMessage(hc12.genPackage("R1", txFrom, "EST", ""));
      swDeActEmer =true;
    }
  }
  if (swActEmer){
    hc12.addMessage(hc12.genPackage("R1", txFrom, "EST", ""));
    swActEmer = false;
    EmerStop = true;
  }
  if (swDeActEmer){
    hc12.addMessage(hc12.genPackage("R1", txFrom, "NOR", ""));
    swDeActEmer = false;
    EmerStop = false;
  }
}

void resetProcess(void){
  swStateCall =false;
  waitAgv =false;
  sentReturn =false;
  swStateCancel =false;
  swActCancel =false;
  totalQueue =0;
  myQueue =0;
  Serial.println("reset");
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,8);
  display.setTextSize(3);
  display.print("RESET !");
  display.display();
  while (1);
}
