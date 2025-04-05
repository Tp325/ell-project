#ifndef Communication_h
#define Communication_h
#include "config.h"
#include <RadioLib.h>
#include <HardwareSerial.h>
#include <driver/uart.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFiManager.h>
#include <strings_en.h>
#include <wm_consts_en.h>
#include <wm_strings_en.h>
#include <wm_strings_es.h>
#include <PubSubClient.h>
extern DynamicJsonDocument doc;
extern SX1278 radio;
extern WiFiManager wm;
extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern volatile bool receiveFlag;
extern void setReceiveFlag();
extern int state;
extern int trasmitState;
class Communication {
private:
  static Communication* instance;
  void connectMqtt();
  void callbackmqtt(char* topic, byte* message, unsigned int length);
  static void callbackWrapper(char* topic, byte* payload, unsigned int length) {
    instance->callbackmqtt(topic, payload, length);
  }
  String msg;
public:
  Communication();
  void begin();
  void receiveFromDisplay();
  void sendToNode();
  void receiveFromNode();
  void sendToDisplay();
  void processWiFi();
  void processMQTT();
};
#endif