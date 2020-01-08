void dpInit(void){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //initialize I2C addr 0x3c
  display.clearDisplay();                     // clears the screen and buffer
  display.drawPixel(127, 63, WHITE);
}

void dpShowWelcome(void){
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("AGV System start...");
  display.setTextSize(3);
  display.setCursor(0,25);
  display.println("REDTECH");
  display.setTextColor(BLACK, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void dpShowTitle(void){
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("AGV." +txFrom);
  display.setCursor(18,18);
  display.setTextSize(1);
  display.println("QUEUE      TOTAL");
  
}

void dpShowReady(void){
  static long timeBlinkDp;
  static bool booBlinkDp;
  if (millis() - timeBlinkDp > 500){
    booBlinkDp = !booBlinkDp;
    timeBlinkDp = millis();
    display.setTextSize(1);
    if (booBlinkDp){
      display.setTextColor(WHITE);
    }
    else{
      display.setTextColor(BLACK);
    }
    display.setCursor(98,0);
    display.println("READY");
  }
}

void dpShowQueue(void){
  display.setTextColor(WHITE,BLACK);
  display.setCursor(7,30);
  display.setTextSize(4);
  
  if (EmerStop){
    display.print("STOP!");
  }
  else{
    if (myQueue/10 == 0) display.print(" ");  else display.print(myQueue/10);
    display.print(myQueue%10);
    display.print("/");
    if (totalQueue/10 != 0) display.print(totalQueue/10); 
    display.print(totalQueue%10);
    if (totalQueue/10 == 0) display.print(" ");
  }
}

void dpRefresh(int timeRefresh, int timeClear){
  static long timeLastClear;
  static long timeLastRefresh;
  if (millis() - timeLastClear > timeClear){
    display.clearDisplay();
    display.display();
    timeLastClear = millis();
  }
  if (millis() - timeLastRefresh > timeRefresh){
    display.display();
    timeLastRefresh = millis();
  }
}

void dpShowSignal(void){
  display.setCursor(120,0);
  if (received) display.setTextColor(BLACK, WHITE); else display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.println(" ");
}
