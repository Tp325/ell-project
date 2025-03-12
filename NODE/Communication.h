#ifndef Communication_h
#define Communication_h
#include "config.h"
#include <RadioLib.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
extern DynamicJsonDocument doc;
extern SX1278 radio;
extern volatile bool receiveFlag ;
extern void setReceiveFlag();
class Communication {
private:
  int state;
  int transmissionState = RADIOLIB_ERR_NONE;
public:
  Communication();
  void begin();
  void receiveFromSink();
  void replyToSink();
  void analysis_send_DataToSink();
  void sendToSink();
};
#endif