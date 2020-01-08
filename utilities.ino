void loopTimeReport(void){
  static long timeLastLap =0;
  long lapTime = millis() - timeLastLap;
  timeLastLap = millis();
  Serial.println("main loop time = " +String(lapTime) +" ms.");
}
