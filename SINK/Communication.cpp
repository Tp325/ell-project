#include "Communication.h"
SX1278 radio = new Module(Nss, Dio0, Rst, Dio1);
DynamicJsonDocument doc;
volatile bool receiveFlag = false;
void setReceiveFlag() {
  receiveFlag = true;
}
Communication::Communication() {
}
void Communication::begin() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  // Cài đặt driver UART2 với buffer RX 1024 byte và TX 1024 byte
  uart_driver_install(UART_NUM_2, 1024, 1024, 0, NULL, 0);
  Serial.println("[SX1278] Initializing ... ");
  state = radio.begin(carrierFrequency, bandwidth, spreadingFactor, codingRate, syncWord, outputPower, preambleLength, amplifierGain);
  // state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
  } else {
    Serial.print("failed, code ");
    Serial.println(state);
    while (true) { vTaskDelay(100 / portTICK_PERIOD_MS); }
  }
  radio.setPacketReceivedAction(setReceiveFlag);
  Serial.print(F("[SX1278] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }
}

void Communication::receiveFromDisplay() {
  if (Serial2.available()) {
    msgFromDisplay = Serial2.readString();
    if (!isFull(buffDataFromDisplay)) {
      enqueueData(buffDataFromDisplay, msgFromDisplay.c_str());
    }
  }
}
void Communication::sendToNode() {
  while (!isEmpty(buffDataFromDisplay)) {
    msgToNode = dequeue(buffDataFromDisplay);
    state = radio.transmit(msgToNode);
    if (state == RADIOLIB_ERR_NONE) {
      Serial.print("send: ");
      Serial.println(msgToNode);
      Serial.println("transmission finished!");
    } else {
      Serial.print("failed, code ");
      Serial.println(state);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
    radio.startReceive();
  }
}

void Communication::receiveFromNode() {
  if (receiveFlag) {
    receiveFlag = false;
    state = radio.readData(msgFromNode);
    if (state == RADIOLIB_ERR_NONE) {
      if (!isFull(buffDataFromNode) && msgFromNode != msgToNode) {
        Serial.print("receive: ");
        Serial.println(msgFromNode);
        enqueueData(buffDataFromNode, msgFromNode.c_str());
      }
    }
  }
}
void Communication::sendToDisplay() {
  while (!isEmpty(buffDataFromNode)) {
    Serial.print("send to display: ");
    msgToDisplay = dequeue(buffDataFromNode);
    Serial.println(msgToDisplay);
    Serial2.print(msgToDisplay);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
