#include "Communication.h"
DynamicJsonDocument doc(1024);
DynamicJsonDocument doc1(1024);

Communication::Communication() {
}
void Communication::begin() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  uart_driver_install(UART_NUM_2, 1024, 1024, 0, NULL, 0);
}
void Communication::sendToSink() {
  // {"ID":1,"in":false,"out":false,"auto":false,"max":3,"mid":0,"min":0}
  doc["ID"] = pool[lastPage != 0 ? lastPage : currentPage].poolID;  //
  doc["i"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].inStatus);
  doc["o"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].outStatus);
  doc["a"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].autoStatus);
  doc["ma"] = pool[lastPage != 0 ? lastPage : currentPage].maxValue;  // vì trong setting page giá trị current page =0 nên lưu vào biến last page là page hiện tại
  doc["md"] = pool[lastPage != 0 ? lastPage : currentPage].midValue;  //
  doc["mn"] = pool[lastPage != 0 ? lastPage : currentPage].minValue;  //
  serializeJson(doc, msgToSink);
  Serial2.print(msgToSink);
}
void Communication::receiveFromSink() {
  if (Serial2.available()) {
    msgFromSink = Serial2.readString();
    Serial.println(msgFromSink);
    if (!isFull(buffDataFromSink)) {
      enqueueData(buffDataFromSink, msgFromSink.c_str());
    }
  }
  while (!isEmpty(buffDataFromSink)) {
    bufferMsgFromSink = dequeue(buffDataFromSink);
    serializeJson(doc1, bufferMsgFromSink);
    IDOfPool = doc1["ID"].as<int>();
    pool[IDOfPool].poolID = doc1["ID"].as<int>();
    pool[IDOfPool].autoStatus = doc1["a"].as<uint8_t>();
    pool[IDOfPool].outStatus = doc1["o"].as<uint8_t>();
    pool[IDOfPool].inStatus = doc1["i"].as<uint8_t>();
  }
}