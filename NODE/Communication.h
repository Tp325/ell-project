#ifndef Communication_h
#define Communication_h
#include "config.h"
#include <RadioLib.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include "Sensor.h"
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
  void receiveFromSink();
  void sendToSink();
  void sendToSink(String msg);
  void analizeData();
};
#endif