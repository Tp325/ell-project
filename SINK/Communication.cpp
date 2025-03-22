#include "Communication.h"
SX1278 radio = new Module(Nss, Dio0, Rst, Dio1);
DynamicJsonDocument doc(1024);
int state;
int trasmitState;
volatile bool receiveFlag = false;
void setReceiveFlag() {
  // Serial.println("flag");
  receiveFlag = true;
  if (isSended == 1) {
    receiveFlag = false;
    isSended = 0;
  }
}
Communication::Communication() {
}
void Communication::begin() {
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
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
  // Serial.print(F("[SX1278] Starting to listen ... "));
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
    msgFromDisplay = Serial2.readStringUntil('\n');
    // Serial.printf("receive from display:%s", msgFromDisplay);
    if (!isFull(buffDataFromDisplay)) {
      enqueueData(buffDataFromDisplay, msgFromDisplay.c_str());
    }
  }
}
void Communication::sendToNode() {
  while (!isEmpty(buffDataFromDisplay)) {
    msgToNode = dequeue(buffDataFromDisplay);
    isSended = 1;
    trasmitState = radio.transmit(msgToNode);
    if (trasmitState == RADIOLIB_ERR_NONE) {
      // Serial.print("send: ");
      // Serial.println(msgToNode);
      Serial.println("transmission finished!");
    } else {
      Serial.print("failed, code ");
      Serial.println(state);
    }
    vTaskDelay(5 / portTICK_PERIOD_MS);
    state = radio.startReceive();
  }
}

void Communication::receiveFromNode() {
  if (receiveFlag) {
    receiveFlag = false;
    state = radio.readData(msgFromNode);
    if (state == RADIOLIB_ERR_NONE) {
      if (!isFull(buffDataFromNode)) {
        // Serial.print("receive: ");
        // Serial.println(msgFromNode);
        enqueueData(buffDataFromNode, msgFromNode.c_str());
      }
    }
  }
}
void Communication::sendToDisplay() {
  while (!isEmpty(buffDataFromNode)) {
    // Serial.print("send to display: ");
    msgToDisplay = dequeue(buffDataFromNode);
    // Serial.println(msgToDisplay);
    Serial2.println(msgToDisplay);
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
