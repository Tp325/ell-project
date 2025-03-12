#include "Communication.h"
DynamicJsonDocument doc(1024);
SX1278 radio = new Module(Nss, Dio0, Rst, Dio1);
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
    radio.setPacketReceivedAction(setReceiveFlag);
  }
}
void Communication::sendToNode() {
  while (!isEmpty(buffDataFromDisplay)) {
    msgSendToNode = dequeue(buffDataFromDisplay);
    transmissionState = radio.transmit(msgSendToNode);
    if (transmissionState == RADIOLIB_ERR_NONE)
      Serial.println("transmission finished!");
    else {
      Serial.print("failed, code ");
      Serial.println(transmissionState);
    }
    radio.finishTransmit();
    state = radio.startReceive();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
void Communication::sendToDisplay() {
  while (!isEmpty(buffDataFromNode)) {
    msgSendToDisplay = dequeue(buffDataFromNode);
    Serial2.print(msgSendToDisplay);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Communication::receiveFromNode() {
  if (receiveFlag) {
    receiveFlag = false;
    state = radio.readData(msgFromNode);
    Serial.println(msgFromNode);
    if (state == RADIOLIB_ERR_NONE) {
      if (!isFull(buffDataFromNode)) {
        enqueueData(buffDataFromNode, msgFromNode.c_str());
      }
    }
  }
}
void Communication::receiveFromDisplay() {
  if (Serial2.available()) {
    msgFromDisplay = Serial2.readString();
    Serial.println(msgFromDisplay);
    if (!isFull(buffDataFromDisplay)) {
      enqueueData(buffDataFromDisplay, msgFromDisplay.c_str());
    }
  }
}