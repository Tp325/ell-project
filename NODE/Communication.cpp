#include "Communication.h"
DynamicJsonDocument doc(1024);
DynamicJsonDocument doc1(1024);
SX1278 radio = new Module(Nss, Dio0, Rst, Dio1);
volatile bool receiveFlag = false;
void setReceiveFlag() {
  receiveFlag = true;
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
  state = radio.startReceive();
  radio.setPacketReceivedAction(setReceiveFlag);
}
void Communication::receiveFromSink() {
  if (receiveFlag) {
    receiveFlag = false;
    state = radio.readData(msgFromSink);
    Serial.println(msgFromSink);
    if (state == RADIOLIB_ERR_NONE) {
      if (!isFull(buffDataFromSink)) {
        enqueueData(buffDataFromSink, msgFromSink.c_str());
      }
    }
  }
}
void Communication::sendToSink() {
  while (!isEmpty(buffDataToSink)) {
    buffermsgToSink = dequeue(buffDataToSink);
    Serial.println(buffermsgToSink);
    transmissionState = radio.transmit(buffermsgToSink);
    if (transmissionState == RADIOLIB_ERR_NONE)
      Serial.println("transmission finished!");
    else {
      Serial.print("failed, code ");
      Serial.println(transmissionState);
    }
    radio.finishTransmit();
    state = radio.startReceive();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Communication::analysis_send_DataToSink() {
  while (!isEmpty(buffDataFromSink)) {
    buffermsgFromSink = dequeue(buffDataFromSink);
    deserializeJson(doc, buffermsgFromSink);
    IDOfPool = doc["ID"].as<int>();
    pool[IDOfPool].poolID = doc["ID"].as<int>();
    if (doc["i"].as<int>() == 3 || doc["i"].as<int>() == 2) {
      pool[IDOfPool].inStatus = bool(doc["i"].as<int>() - 2);
      doc1["ID"] = IDOfPool;
      doc1["a"] = int(pool[IDOfPool].autoStatus);
      doc1["i"] = int(pool[IDOfPool].inStatus);
      doc1["o"] = int(pool[IDOfPool].outStatus);
    }
    if (doc["o"].as<int>() == 3 || doc["o"].as<int>() == 2) {
      pool[IDOfPool].outStatus = bool(doc["o"].as<int>() - 2);
      doc1["ID"] = IDOfPool;
      doc1["a"] = int(pool[IDOfPool].autoStatus);
      doc1["i"] = int(pool[IDOfPool].inStatus);
      doc1["o"] = int(pool[IDOfPool].outStatus);
    }
    if (doc["a"].as<int>() == 3 || doc["a"].as<int>() == 2) {
      pool[IDOfPool].autoStatus = bool(doc["a"].as<int>() - 2);
      doc1["ID"] = IDOfPool;
      doc1["a"] = int(pool[IDOfPool].autoStatus);
      doc1["i"] = 0;
      doc1["o"] = 0;
    }
    pool[IDOfPool].maxValue = doc["ma"].as<float>();
    pool[IDOfPool].midValue = doc["md"].as<float>();
    pool[IDOfPool].minValue = doc["mn"].as<float>();
    serializeJson(doc1, msgToSink);
    Serial.println(msgToSink);
    if (!isFull(buffDataToSink)) {
      enqueueData(buffDataToSink, msgToSink.c_str());
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
