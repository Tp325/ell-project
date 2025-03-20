#ifndef Communication_h
#define Communication_h
#include "config.h"
#include <RadioLib.h>
#include <HardwareSerial.h>
#include <driver/uart.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
extern DynamicJsonDocument doc;
extern SX1278 radio;
extern volatile bool receiveFlag;
extern void setReceiveFlag();
extern int state;
extern int trasmitState;
class Communication {
private:
public:
  Communication();
  void begin();
  void receiveFromDisplay();
  void sendToNode();
  void receiveFromNode();
  void sendToDisplay();
};
#endif