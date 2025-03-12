#include "config.h"

//*********************** quản lý hồ****************
Pool pool[10];
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
bool isHavingMsg = 0;

//**************CircularQueue************
CircularQueue *buffDataFromDisplay = createQueue(30);
CircularQueue *buffDataFromNode = createQueue(30);
