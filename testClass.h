
#ifndef testClass
#define testClass

#include <Arduino.h>
class hc12{
  String  rxTo = "";
  String  rxFrom = "";
  String  rxCommand = "";
  String  rxData = "";
  String  txFrom = "L2";

  public:
    hc12(int pin, int br, int ch);
   
    String  genPackage(String from, String to, String command, String data);
    bool    recv(void);
    bool    checkSum(String data);
    String  to(void);
    String  from(void);
    String  command(void);
    String  data(void);
    bool    addMessage(String mess);
    bool    removeMessage(String mess);
    void    sender(char mode, bool rdyToSend, long timeSendInterval, long timeRecvPol, long timeWaitForSend);
    byte    messageCount(void);
    void    printMessList(void);

  private:
    #define maxMessage 10
    bool    unReadRfPackage = false;
    byte    cntRfData = 0;
    byte    availableMessageCount = 0;
    byte    arrayPosition = 0;
    long    timeLastPcEcho = 0;
    String  message[maxMessage];
    String  strRfData = "";
  
    bool    addProcess(String mess);
    void    extractData(String data);
    byte    genCheckSum(String data);
    bool    recvReturn(void);
};

#endif
