#ifndef config_h
#define config_h
#include <Arduino.h>
#include "CircularQueue.h"
#include <WiFi.h>
#include <PubSubClient.h>

//******************LOra*****************
#define Nss 5
#define Rst 4
#define Dio0 2
#define Dio1 14
#define ledRed 13
extern float carrierFrequency;
extern float bandwidth;
extern uint8_t spreadingFactor;
extern uint8_t codingRate;
extern uint8_t syncWord;  // tùy chỉnh đường mạng
extern uint8_t outputPower;
extern uint8_t preambleLength;
extern uint8_t amplifierGain;

// ************chân nạp code************************
#define CHIP_ENABLE_PIN 4   // Chân kích hoạt Chip Enable trên ESP32
#define RESET_PIN 18        // Chân Reset trên ESP32
#define DATA_COMMAND_PIN 5  // Chân Data/Instruction trên ESP32
#define WRITE_PIN 2         // Chân Write trên ESP32
#define READ_PIN 0          // Chân Read trên ESP32

// // MQTT
// #define MESSAGE_FORMAT "<command>:<value>"
// #define COMMAND_VALVE_OPEN "val_open"
// #define COMMAND_VALVE_CLOSE "val_close"

// **************** **** mqtt *************
extern PubSubClient client;
extern String mqttUser;            // MQTT username
extern String mqttServer;          // MQTT broker address
extern const int mqttPort;              // Default MQTT port
extern String mqttPassword;        // MQTT password
extern String mqttTopicPublish;    // desired publish topic
extern String mqttTopicSubscribe;  // desired subscribe topic

//****************** Wifi ************************
extern WiFiClient espClient;
extern  String ssid;      // WiFi SSID
extern String password;  // WiFi password

//*********************** quản lý hồ****************
extern int numberOfPool;
struct Pool {
  int poolID;
  float mucnuoc;
  bool inStatus;
  bool outStatus;
  bool autoStatus;
  float maxValue;
  float midValue;
  float minValue;
};
extern Pool pool[10];
extern int IDOfPool;

//*************communication***********
#define MAX_PACKET_SIZE 200
extern String msgFromDisplay;
extern String msgSendToDisplay;
extern String msgSendToNode;
extern String msgFromNode;
extern String msgFromMQTT;
extern String buffermsgFromMQTT;
extern String msgToMQTT;
extern bool isHavingMsg;

//************* CircularQueue*****************
extern CircularQueue *buffDataFromDisplay;
extern CircularQueue *buffDataFromNode;
extern CircularQueue *buffDataFromMqtt;
extern CircularQueue *buffDataToMqtt;

#endif