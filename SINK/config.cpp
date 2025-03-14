#include "config.h"

// **************** **** mqtt *************
PubSubClient client(espClient);
String mqttUser = "api1@Iotlab";           // MQTT username
String mqttPassword = "Iotlab@2023";  // MQTT password
String mqttServer = "103.221.220.183";           // MQTT broker address
const int mqttPort = 1883;                   // Default MQTT port
String mqttTopicPublish = "luon/data";  // desired publish topic
String mqttTopicSubscribe = "luon/control";  // desired subscribe topic


//****************** Wifi ************************
WiFiClient espClient;
String ssid = "Phong_4";         // WiFi SSID
String password = "1234512345";  // WiFi passwordconst char *mqttTopicSubscribe = "luon/control";  // desired subscribe topic


//*********************** quản lý hồ****************
Pool pool[10];
int IDOfPool;

//***********Lora********************
int numberOfPool = 0;
float carrierFrequency = 434.0;
float bandwidth = 250.0;
uint8_t spreadingFactor = 7;
uint8_t codingRate = 8;
uint8_t syncWord = 0x34;
uint8_t outputPower = 20;
uint8_t preambleLength = 12;
uint8_t amplifierGain = 0;
//*************communication***********
String msgFromDisplay;
String msgSendToDisplay;
String msgSendToNode;
String msgFromNode;
String msgFromMQTT;
String buffermsgFromMQTT;
String msgToMQTT;


bool isHavingMsg = 0;

//**************CircularQueue************
CircularQueue *buffDataFromDisplay = createQueue(30);
CircularQueue *buffDataFromNode = createQueue(30);
CircularQueue *buffDataFromMqtt = createQueue(30);
CircularQueue *buffDataToMqtt = createQueue(30);
CircularQueue *buffDataToNode = createQueue(30);
CircularQueue *buffDataToDisplay = createQueue(30);
