#ifndef Communication_h
#define Communication_h
#include "config.h"
#include <HardwareSerial.h>
#include <driver/uart.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
extern DynamicJsonDocument doc;
class Communication {
private:
public:
  Communication();
  void begin();
  void sendToSink();
  void receiveFromSink();
  void analizeData();
  void analizeDataToSink();
};
#endif