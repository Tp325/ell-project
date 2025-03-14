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

class Communication
{
private:
  int state;
  int transmissionState = RADIOLIB_ERR_NONE;

public:
  Communication();
  void begin();
  void sendToNode();
  void sendToDisplay();
  void receiveFromNode();
  void receiveFromDisplay();
  void connnectToWifi(String user, String pass);
  void connnectToMQTT(String server, String topicSubscribe);
  void mqttCallback(char *topic, byte *payload, unsigned int length);
  void mqttReconnect(String topicSubscribe);
  void processMqtt();
  void sendToMQTT(String topic, String msg);
  void analizeDataFromMQTT();
};
#endif