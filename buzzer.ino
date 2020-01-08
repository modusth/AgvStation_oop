void buzzInit(void){
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,HIGH);
}

void buzzOn(bool on){
  if(on){
    digitalWrite(buzzer,LOW);
  }
  else{
    digitalWrite(buzzer,HIGH);
  }
}

void buzzBeep(bool setBuzz, bool type, long timeOn){
  static long timeStartBeep = 0;
  static long timePeriodBeep = 0;
  static bool typeBeep = 0;
  if (setBuzz){
    timeStartBeep = millis();
    timePeriodBeep = timeOn;
    typeBeep = type;
  }
  if (!typeBeep){
    if ((millis() - timeStartBeep) > timePeriodBeep){
      buzzOn(0);
    }
    else{
      buzzOn(1);
    }
  }
  else{
    if ((millis() - timeStartBeep) > timePeriodBeep){
      buzzOn(0);
    }
    else if (((millis() - timeStartBeep)/200)%2){
      buzzOn(1);
    }
    else{
      buzzOn(0);
    }
  }
}
