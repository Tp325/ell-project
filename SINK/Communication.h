#ifndef Communication_h
#define Communication_h
#include "config.h"
#include <RadioLib.h>
#include <HardwareSerial.h>
#include <driver/uart.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <PubSubClient.h>
#include <WiFiManager.h>
extern DynamicJsonDocument doc;
extern SX1278 radio;
extern volatile bool receiveFlag;
extern void setReceiveFlag();
class Communication {
private:

  int state;
  int transmissionState = RADIOLIB_ERR_NONE;
public:
  Communication();
  void begin();
  void connectToWifi();
  void connectToMQTT();
  void sendtoMQTT();
  void receiveFromMQTT();
  void connectLora();
  void sendToNode();
  void sendToDisplay();
  void receiveFromNode();
  void receiveFromDisplay();
};
#endif