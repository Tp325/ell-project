#include "config.h"


String StationID = "TESTH6CT";



//*********************** quản lý hồ****************
Pool pool[10];
int numberOfPool = 0;
float carrierFrequency = 434.0;
float bandwidth = 250.0;
uint8_t spreadingFactor = 7;
uint8_t codingRate = 8;
uint8_t syncWord = 0x33;
uint8_t outputPower = 20;
uint8_t preambleLength = 12;
uint8_t amplifierGain = 0;
//*************communication***********
String msgFromDisplay;
String msgToDisplay;
String msgToNode;
String msgFromNode;
String msgFromServer;
String msgToServer;
bool isSended;

// Thông tin MQTT Broker
bool isWifiConnect = 0;
const char *mqtt_server = "103.221.220.183";
const int mqtt_port = 1883;
const char *mqtt_user = "api1@Iotlab";
const char *mqtt_pass = "Iotlab@2023";
String topicReceive = "ellReceive";
String topicSend = "ellSend";


//**************CircularQueue************
CircularQueue *buffDataFromDisplay = createQueue(40);
CircularQueue *buffDataFromNode = createQueue(40);
CircularQueue *buffDataFromServer = createQueue(40);
CircularQueue *buffDataToServer = createQueue(40);