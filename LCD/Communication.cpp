#include "Communication.h"
DynamicJsonDocument doc(1024);
DynamicJsonDocument doc1(1024);

Communication::Communication() {
}
void Communication::begin() {
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  uart_driver_install(UART_NUM_2, 1024, 1024, 0, NULL, 0);
}
void Communication::sendToSink() {
  while (!isEmpty(buffDataToSink)) {
    Serial2.println(dequeue(buffDataToSink));
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void Communication::receiveFromSink() {
  if (Serial2.available()) {
    msgFromSink = Serial2.readStringUntil('\n');
    if (!isFull(buffDataFromSink)) {
      enqueueData(buffDataFromSink, msgFromSink.c_str());
    }
  }
}
void Communication::analizeDataToSink() {
  doc["ID"] = pool[lastPage != 0 ? lastPage : currentPage].poolID;  //
  doc["i"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].inStatus);
  doc["o"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].outStatus);
  doc["a"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].autoStatus);
  doc["ma"] = pool[lastPage != 0 ? lastPage : currentPage].maxValue;  // vì trong setting page giá trị current page =0 nên lưu vào biến last page là page hiện tại
  doc["md"] = pool[lastPage != 0 ? lastPage : currentPage].midValue;  //
  doc["mn"] = pool[lastPage != 0 ? lastPage : currentPage].minValue;  //
  serializeJson(doc, msgToSink);
  if (!isFull(buffDataToSink)) {
    enqueueData(buffDataToSink, msgToSink.c_str());
  }
}

void Communication::analizeData() {
  while (!isEmpty(buffDataFromSink)) {
    bufferMsgFromSink = dequeue(buffDataFromSink);
    deserializeJson(doc1, bufferMsgFromSink);
    IDOfPool = doc1["ID"].as<int>();
    if (doc1["mucn"].is<float>()) {
      pool[IDOfPool].poolID = doc1["ID"].as<int>();
      pool[IDOfPool].mucnuoc = doc1["mucn"].as<float>();
    } else {
      pool[IDOfPool].poolID = doc1["ID"].as<int>();
      pool[IDOfPool].autoStatus = doc1["a"].as<uint8_t>();
      pool[IDOfPool].outStatus = doc1["o"].as<uint8_t>();
      pool[IDOfPool].inStatus = doc1["i"].as<uint8_t>();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
