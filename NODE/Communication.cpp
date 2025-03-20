#include "Communication.h"
DynamicJsonDocument doc(1024);
SX1278 radio = new Module(Nss, Dio0, Rst, Dio1);
int state;
int trasmitState = -1;
volatile bool receiveFlag = false;
void setReceiveFlag() {
  Serial.println("flag");
  receiveFlag = true;
  if (isSended == 1) {
    receiveFlag = false;
    isSended = 0;
  }
}
Communication::Communication() {
}
void Communication::begin() {
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
  // sensor.begin();
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
void Communication::receiveFromSink() {
  if (receiveFlag) {
    receiveFlag = false;
    state = radio.readData(msgFromSink);
    if (state == RADIOLIB_ERR_NONE) {
      if (!isFull(buffDataFromSink)) {
        Serial.print("receive: ");
        Serial.println(msgFromSink);
        enqueueData(buffDataFromSink, msgFromSink.c_str());
      }
    }
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}
void Communication::analizeData() {
  while (!isEmpty(buffDataFromSink)) {
    buffMsgFromSink = dequeue(buffDataFromSink);
    DeserializationError error = deserializeJson(doc, buffMsgFromSink);
    if (!error) {
      IDOfPool = doc["ID"].as<int>();
      pool[IDOfPool].poolID = doc["ID"].as<int>();
      if (doc["i"].as<int>() == 3 || doc["i"].as<int>() == 2) {
        pool[IDOfPool].inStatus = bool(doc["i"].as<int>() - 2);
        doc["i"] = int(pool[IDOfPool].inStatus);
      }
      if (doc["o"].as<int>() == 3 || doc["o"].as<int>() == 2) {
        pool[IDOfPool].outStatus = bool(doc["o"].as<int>() - 2);
        doc["o"] = int(pool[IDOfPool].outStatus);
      }
      if (doc["a"].as<int>() == 3 || doc["a"].as<int>() == 2) {
        pool[IDOfPool].autoStatus = bool(doc["a"].as<int>() - 2);
        doc["a"] = int(pool[IDOfPool].autoStatus);
      }
      pool[IDOfPool].maxValue = doc["ma"].as<float>();
      pool[IDOfPool].midValue = doc["md"].as<float>();
      pool[IDOfPool].minValue = doc["mn"].as<float>();
      serializeJson(doc, buffMsgFromSink);
      if (!isFull(buffDataToSink)) {
        enqueueData(buffDataToSink, buffMsgFromSink.c_str());
      }
    } else {
      Serial.println("error read json");
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void Communication::sendToSink(String msg) {
  if (!isFull(buffDataToSink)) {
    enqueueData(buffDataToSink, msg.c_str());
  }
}
void Communication::sendToSink() {
  while (!isEmpty(buffDataToSink)) {
    vTaskDelay(20 / portTICK_PERIOD_MS);
    msgToSink = dequeue(buffDataToSink);
    isSended = 1;
    trasmitState = radio.transmit(msgToSink);
    if (trasmitState == RADIOLIB_ERR_NONE) {
      Serial.print("send: ");
      Serial.println(msgToSink);
      Serial.println("transmission finished!");
    } else {
      Serial.print("failed, code ");
      Serial.println(trasmitState);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    state = radio.startReceive();
  }
}
