#include "config.h"


// String StationID = "TESTH6CT";
// String StationID = "TramluonNgaNam1";
// String StationID = "TramluonNgaNam2";
// String StationID = "TramluonNgaNam3";
String StationID = "TramluonNgaNam4";
// String StationID = "TramluonNgaNam5";
// String StationID = "TramluonNgaNam6";






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
bool haveToReset = 0;

// Thông tin MQTT Broker
bool isWifiConnect = 0;
const char *mqtt_server = "103.221.220.183";
const int mqtt_port = 1883;
const char *mqtt_user = "api1@Iotlab";
const char *mqtt_pass = "Iotlab@2023";

// String topicReceive = "TramluonNgaNam1Receive";
// String topicSend = "TramluonNgaNam1Sent";

// String topicReceive = "TramluonNgaNam2Receive";
// String topicSend = "TramluonNgaNam2Sent";

// String topicReceive = "TramluonNgaNam3Receive";
// String topicSend = "TramluonNgaNam3Sent";

String topicReceive = "TramluonNgaNam4Receive";
String topicSend = "TramluonNgaNam4Sent";

// String topicReceive = "TramluonNgaNam5Receive";
// String topicSend = "TramluonNgaNam5Sent";

// String topicReceive = "TramluonNgaNam6Receive";
// String topicSend = "TramluonNgaNam6Sent";

//**************CircularQueue************
CircularQueue *buffDataFromDisplay = createQueue(70);
CircularQueue *buffDataFromNode = createQueue(70);
CircularQueue *buffDataFromServer = createQueue(70);
CircularQueue *buffDataToServer = createQueue(70);